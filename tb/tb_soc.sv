// SoC 集成测试平台：加载程序运行，自动判定结果
// 用法: tb_soc +HEX=<程序.hex> [+EXPECT=<停机原因码,默认1>] [+MAGIC=<GPIO魔数>] [+TRACE]
//       tb_soc +HEX=<ACT.elf.hex> +TOHOST=<tohost地址>   （官方 ACT 自检模式）
// - UART TX 监视器：解码串行输出并原样打印
// - UART RX 激励：复位后向核心发送字节 0xA5
// - GPIO_IN 固定驱动 0x12345678
// - 结束判据（普通模式）：停机原因码匹配 EXPECT；给 MAGIC 时要求 gpio_out 等于它
// - 结束判据（ACT 模式）：监视 dmem 中 tohost 单元，1=通过、3=失败
// - 签名转储（随机测试用）：+SIGSTART=<起始> +SIGEND=<结束> +SIGFILE=<路径>，
//   结束时把 dmem [起始,结束) 区间按每行一个 32 位十六进制字写出
//   （与 Sail --test-signature 的输出格式一致）
// - 中断透明性验证（层 B）：+IRQCNT=<计数器地址> 启用中断模式——关闭固定
//   RX 激励（0xA5 字节会拉起 MEIP 干扰注入计数），结束时层级读取 handler
//   陷入计数并与 +IRQEXP=<期望数> 核对（不等判 FAIL，防「中断根本没发生」
//   的假通过）；+IRQPLAN=<计划文件> 按周期调度注入（见注入引擎注释）
// - 总线监听激励（应用 demo）：+MONSCRIPT=<脚本.hex> 时 mbus_gen 按脚本向
//   uart_mon 的监听输入 mon_rxd 发 8N1 波形（BITDIV=32 与 uart_mon 默认值
//   一致）；结束时核对脚本是否发完。不给脚本则 mon_rxd 保持空闲电平
`timescale 1ns/1ps
`default_nettype none

module tb_soc;

  import core_pkg::*;

  localparam int BIT_NS = 160;   // UART 默认 DIV=16，每比特 16×10ns

  // 仿真用 RAM 容量（ACT 自检 ELF 嵌入了期望签名，远超 64KiB；不影响 RTL 默认）
  parameter int MEM_BYTES = 1024 * 1024;

  logic clk, rst_n;
  wire  halted;
  halt_code_t halt_code;
  wire  dbg_mispredict;
  wire  [31:0] gpio_out;
  logic [31:0] gpio_in;
  wire  uart_txd;
  logic uart_rxd;
  wire  mon_rxd;   // 监听输入：mbus_gen 驱动，无脚本时保持空闲电平
  wire  gen_done;

  mbus_gen #(.BITDIV(32)) mbus_gen_i (
    .clk(clk), .rst_n(rst_n), .txd(mon_rxd), .done(gen_done)
  );

  soc_top #(.RAM_BYTES(MEM_BYTES)) dut (
    .clk(clk), .rst_n(rst_n),
    .halted(halted), .halt_code(halt_code), .dbg_mispredict(dbg_mispredict),
    .gpio_out(gpio_out), .gpio_in(gpio_in),
    .uart_txd(uart_txd), .uart_rxd(uart_rxd), .mon_rxd(mon_rxd)
  );

  always #5 clk = ~clk;

  int mispredict_cnt;
  always @(posedge clk) if (dbg_mispredict) mispredict_cnt <= mispredict_cnt + 1;

  // 周期计数（running 起算）：中断注入计划的周期基准，结束时打印供
  // gen_plan.py 生成计划（run A 无注入基线实测总周期数）
  int cycle_cnt;
  always @(posedge clk) if (running) cycle_cnt <= cycle_cnt + 1;

  // ---- ACT tohost 监视（+TOHOST=<addr> 启用）----
  localparam int MEM_AW = $clog2(MEM_BYTES / 4);

  logic [31:0] tohost_addr;
  logic        act_done;
  logic [31:0] act_tohost;

  // 签名转储与回写轨迹（模块级：always 块共用）
  int    sig_start, sig_end;
  string sig_file;
  string trace_file;
  int    trace_fd;
  logic        running;      // 复位释放后置 1（避免与异步复位混用告警）

  // ---- 中断注入引擎（层 B；plusarg 在主 initial 解析）----
  // +IRQPLAN 计划每行 "<周期> <类型> [字节hex]"（gen_plan.py 生成）：
  //   类型 0 = MTI：一次性沉积 poke mtimecmp = mtime+4。mtimecmp 平时仅在
  //     总线写时被驱动，沉积赋值无真实驱动冲突（MULTIDRIVEN 主位置在
  //     mtimer.sv，由 tb/waive_soc.vlt 精确豁免，仅用于 tb_soc 构建）；
  //     handler 经总线写全 1 撤销 MTIP，poke 无需 release。
  //   类型 1 = UART RX：从该周期起驱动 rxd 发一个字节（8N1，DIV=16 约 160
  //     拍），收完 rx_valid 拉起 MEIP；handler 读 DATA 撤销。
  // MSI 不经计划：由程序前导置位一次。计划间隔 >=700 拍（gen_plan.py 保证），
  // UART 帧不会重叠；等周期用 negedge 避开与 posedge 采样的竞争。
  localparam int PLAN_MAX = 512;
  int    plan_cycle [PLAN_MAX];
  int    plan_type  [PLAN_MAX];
  int    plan_arg   [PLAN_MAX];
  int    plan_n;
  int    plan_done_cnt;
  int    irq_cnt_addr;     // 非 0 = 中断模式（pango_irq_count 的 dmem 地址）
  int    irq_exp;
  string plan_file;

  initial begin : irq_engine
    int fd, c, t, b, n;
    string line;
    plan_n = 0;
    plan_done_cnt = 0;
    wait (rst_n);            // 等主 initial 完成 plusarg 解析
    if (plan_file != "") begin
      fd = $fopen(plan_file, "r");
      if (fd == 0) $fatal(1, "tb_soc: 无法打开注入计划 %s", plan_file);
      while ($fgets(line, fd) != 0) begin
        // 先解两字段公共前缀（MTI 行无第三字段；Verilator $sscanf 对缺失
        // 尾字段的行返回值不可靠，三段式一次解会丢 MTI 行）
        n = $sscanf(line, "%d %d", c, t);
        if (n == 2) begin
          if (plan_n >= PLAN_MAX) $fatal(1, "tb_soc: 注入计划超过 %0d 条", PLAN_MAX);
          b = 0;
          if (t == 1 && $sscanf(line, "%d %d %h", c, t, b) != 3)
            $fatal(1, "tb_soc: 注入计划行格式错误（UART 注入缺字节值）: %s", line);
          plan_cycle[plan_n] = c;
          plan_type[plan_n]  = t;
          plan_arg[plan_n]   = b;
          plan_n++;
        end
      end
      $fclose(fd);
      $display("tb_soc: 注入计划 %0d 条（%s）", plan_n, plan_file);
    end
    for (int i = 0; i < plan_n; i++) begin
      while (cycle_cnt < plan_cycle[i]) @(negedge clk);
      if (plan_type[i] == 0) begin
        dut.mtimer_i.mtimecmp = dut.mtimer_i.mtime + 64'd4;
      end else begin
        logic [7:0] ub;
        ub = plan_arg[i][7:0];
        uart_rxd = 1'b0; #(BIT_NS);
        for (int k = 0; k < 8; k++) begin uart_rxd = ub[k]; #(BIT_NS); end
        uart_rxd = 1'b1; #(BIT_NS);
      end
      plan_done_cnt++;
    end
  end

  always @(posedge clk) begin
    if (running && tohost_addr != 0 && !act_done) begin
      if (dut.dmem_i.mem[tohost_addr[MEM_AW+1:2]] != 32'd0) begin
        act_done   <= 1'b1;
        act_tohost <= dut.dmem_i.mem[tohost_addr[MEM_AW+1:2]];
      end
    end
  end

  // ---- UART TX 监视器：逐字节解码并打印 ----
  initial begin : tx_monitor
    logic [7:0] ch;
    forever begin
      @(negedge uart_txd);
      #(BIT_NS / 2 + 10);
      for (int i = 0; i < 8; i++) begin #(BIT_NS) ch[i] = uart_txd; end
      #(BIT_NS);
      $write("%c", ch);   // 连续文本输出，便于脚本解析
    end
  end

  // ---- WB 回写轨迹（+TRACELOG 启用，调试用；文件在主 initial 块中打开） ----
  always @(posedge clk) begin
    if (trace_fd != 0 && running && dut.core_i.wb_reg_write && dut.core_i.wb_rd != 5'd0)
      $fwrite(trace_fd, "x%0d = %08x\n", dut.core_i.wb_rd, dut.core_i.wb_data);
  end

  // ---- UART RX 激励：复位结束后发送 0xA5 ----
  // 注意：全局激励，对所有程序生效（test_isa 依赖它验证 RX 路径）；
  // 新增测试若在早期读 UART，需知晓会收到这一固定字节。
  // 中断模式（+IRQCNT）下关闭：该字节收完后 rx_valid 长期置位，会成为
  // 计划外 MEI 中断源，破坏注入计数核对；此模式下 RX 由注入计划驱动
  initial begin : rx_driver
    logic [7:0] b;
    b = 8'hA5;
    uart_rxd = 1'b1;
    wait (rst_n);
    #5000;
    if (irq_cnt_addr != 0) begin
      wait (0);   // 中断模式：永久挂起本激励（进程不再唤醒）
    end
    uart_rxd = 1'b0; #(BIT_NS);
    for (int i = 0; i < 8; i++) begin uart_rxd = b[i]; #(BIT_NS); end
    uart_rxd = 1'b1; #(BIT_NS);
  end

  initial begin
    string hex;
    int    expect_code;
    int    magic;
    bit    magic_en;
    int    timeout_cycles;
    int    errors;
    string monscript_file;

    clk = 0; rst_n = 0; gpio_in = 32'h1234_5678;
    mispredict_cnt = 0;
    cycle_cnt = 0;
    errors = 0;

    if (!$value$plusargs("HEX=%s", hex)) hex = "build/test_isa.hex";
    if (!$value$plusargs("EXPECT=%d", expect_code)) expect_code = 1;
    // MAGIC 以"是否传参"为校验开关而非哨兵值：魔数恰为 0xFFFFFFFF 也能正常校验
    magic = 0;
    magic_en = $value$plusargs("MAGIC=%h", magic);
    tohost_addr = 0;
    act_done = 0;
    act_tohost = 0;
    running = 0;
    $value$plusargs("TOHOST=%h", tohost_addr);
    // 默认 200 万周期：适配当前测试规模（万条随机指令约数万周期）；
    // CoreMark 由 run_bench.sh 显式传 +TIMEOUT=100000000，规模变更时两处需同步
    if (!$value$plusargs("TIMEOUT=%d", timeout_cycles)) timeout_cycles = 2000000;
    // 签名转储区间（不给出 SIGFILE 则不转储）
    sig_start = 0; sig_end = 0;
    $value$plusargs("SIGSTART=%h", sig_start);
    $value$plusargs("SIGEND=%h", sig_end);
    if (!$value$plusargs("SIGFILE=%s", sig_file)) sig_file = "";
    if (!$value$plusargs("TRACELOG=%s", trace_file)) trace_file = "";
    // 中断模式（层 B）：+IRQCNT 给 handler 陷入计数器地址（非 0 启用），
    // +IRQEXP 给期望计数（含前导 MSI 一次），+IRQPLAN 给注入计划文件
    if (!$value$plusargs("IRQCNT=%h", irq_cnt_addr)) irq_cnt_addr = 0;
    if (!$value$plusargs("IRQEXP=%d", irq_exp)) irq_exp = -1;
    if (!$value$plusargs("IRQPLAN=%s", plan_file)) plan_file = "";
    if (!$value$plusargs("MONSCRIPT=%s", monscript_file)) monscript_file = "";
    if (irq_cnt_addr != 0 && irq_exp < 0)
      $fatal(1, "tb_soc: 中断模式需同时给出 +IRQEXP=<期望计数>");
    trace_fd = 0;
    if (trace_file != "") begin
      trace_fd = $fopen(trace_file, "w");
      if (trace_fd == 0) $fatal(1, "tb_soc: 无法打开轨迹文件 %s", trace_file);
    end

    $display("tb_soc: HEX=%s EXPECT=%0d MAGIC=%h", hex, expect_code, magic);
    $readmemh(hex, dut.imem_i.mem);
    $readmemh(hex, dut.dmem_i.mem);

    if ($test$plusargs("TRACE")) begin
      $dumpfile("build/wave_soc.fst");
      $dumpvars(0, tb_soc);
    end

    repeat (5) @(posedge clk);
    rst_n = 1;
    running = 1;

    // 等待停机 / ACT tohost / 超时
    fork
      wait (halted);
      wait (act_done);
      begin
        repeat (timeout_cycles) @(posedge clk);
        $fatal(1, "tb_soc: 超时未停机");
      end
    join_any

    @(posedge clk); #1;
    $display("tb_soc: 停机原因码=%0d, 分支预测失败次数=%0d, 周期数=%0d",
             halt_code, mispredict_cnt, cycle_cnt);
    if (plan_file != "")
      $display("tb_soc: 注入执行 %0d/%0d 条", plan_done_cnt, plan_n);

    // 签名转储：dmem [sig_start, sig_end)，每行一个 32 位字（同 Sail --test-signature）
    if (sig_file != "") begin
      int fd;
      fd = $fopen(sig_file, "w");
      if (fd == 0) $fatal(1, "tb_soc: 无法打开签名输出文件 %s", sig_file);
      for (int a = sig_start; a < sig_end; a += 4)
        $fwrite(fd, "%08x\n", dut.dmem_i.mem[a[MEM_AW+1:2]]);
      $fclose(fd);
      $display("tb_soc: 签名已转储 %s (%0d 字)", sig_file, (sig_end - sig_start) / 4);
    end

    if (tohost_addr != 0) begin
      // ACT 自检模式：只判 tohost
      if (act_tohost == 32'd1) $display("ACT-PASS");
      else begin
        errors++;
        $display("ACT-FAIL: tohost=%h", act_tohost);
      end
    end else begin
      if (halt_code != halt_code_t'(expect_code)) begin
        errors++;
        $display("FAIL: 停机原因码 %0d，期望 %0d", halt_code, expect_code);
      end
      if (magic_en && gpio_out !== 32'(magic)) begin
        errors++;
        $display("FAIL: gpio_out=%h，期望 %h", gpio_out, magic);
      end
    end

    // 中断计数核对（层 B）：handler 实际陷入数 == 期望（前导 MSI + 计划注入数）
    // 不等即 FAIL——少了说明注入未生效（假通过），多了说明计划外中断
    if (irq_cnt_addr != 0) begin
      if (32'(dut.dmem_i.mem[irq_cnt_addr[MEM_AW+1:2]]) != 32'(irq_exp)) begin
        errors++;
        $display("FAIL: 中断计数 %0d，期望 %0d（注入未发生或计划外中断）",
                 dut.dmem_i.mem[irq_cnt_addr[MEM_AW+1:2]], irq_exp);
      end else begin
        $display("tb_soc: 中断计数核对 %0d == %0d",
                 dut.dmem_i.mem[irq_cnt_addr[MEM_AW+1:2]], irq_exp);
      end
    end

    // 监听脚本核对：给了脚本就必须发完，否则说明固件提前停机或超时前
    // 激励未执行完，demo 结果不可信
    if (monscript_file != "" && !gen_done) begin
      errors++;
      $display("FAIL: 监听脚本未发送完毕（固件提前结束或激励被截断）");
    end

    if (errors == 0) $display("tb_soc: PASS");
    else             $fatal(1, "tb_soc: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
