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
// - UART 日志落盘（应用 demo 判定链）：+UARTLOG=<路径> 时 UART TX 监视器
//   解码的字节除打印到 stdout 外同时写入该文件，供 Makefile 与逐场景
//   golden 文件 diff（固件输出在仿真中逐拍确定，可直接文本比对）
// - GPIO 魔数结束（+GPIOEND，与 +MAGIC 同用）：中断驱动固件配置了 mtvec
//   后 ecall/ebreak 走标准陷入而不再停机，结束信号改为 GPIO 魔数写入——
//   检测到 gpio_out 低 16 位非零即结束仿真并核对是否等于 MAGIC（0x900D=PASS）。
//   只看低半字：gpio_out bit16 是应用故障告警锁存位（故障事件置位），
//   不应触发结束；finish/trap_handler 收尾写全字魔数会同时清掉告警位，
//   结束后的全字魔数核对不受影响
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

  mbus_gen #(.BITDIV(32), .SCRIPT_MAX(4096)) mbus_gen_i (
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

  // ---- BTB 收益量化（+BTBSTAT 启用打印；只读观察的测量设施，不影响判定）----
  // 统计 EX 级「未预测间接跳转」重定向次数：JALR 且 rs1 非 link 寄存器
  // （x1/x5）不享受 RAS 预测，必然在 EX 级解析真实目标后重定向，每次冲刷
  // IF/ID 两级、代价按 2 拍估算；次数×2 占总周期比例即 BTB 收益上限。
  // 指令按 idex_pc 从 imem 读回解码——不依赖核心内部控制结构，SV/Verilog
  // 两种 DUT 源码（rtl/ 与 rtl_v/）下层级路径均有效；!stall 保证访存
  // 停顿期间（ex_redirect 跨拍保持）不重复计数。
  localparam int IMEM_AW = $clog2(MEM_BYTES / 4);   // imem 字地址位宽
  int  jalr_unpred_cnt;
  /* verilator lint_off UNUSEDSIGNAL */   // 只需操作码与 rs1 字段，其余位天然未用
  wire [31:0] ex_instr = dut.imem_i.mem[dut.core_i.idex_pc[IMEM_AW+1:2]];
  /* verilator lint_on UNUSEDSIGNAL */
  wire        jalr_unpred_e = dut.core_i.ex_redirect && !dut.core_i.stall &&
                              ex_instr[6:0] == 7'b1100111 &&
                              ex_instr[19:15] != 5'd1 && ex_instr[19:15] != 5'd5;
  always @(posedge clk) if (running && jalr_unpred_e)
    jalr_unpred_cnt <= jalr_unpred_cnt + 1;

  // ---- 性能候选收益量化（+PERFSTAT 启用打印；只读观察的测量设施，不影响判定）----
  // 候选一（IF 级预测：取指级 BTB + RAS 前置）：统计 ID 级重定向次数——
  // JAL 直转、分支预测 taken、RAS 返回命中三类，预测全对也各付 1 拍气泡
  // （IF 级顺取被冲），取指级预测可消除；次数×1 占总周期比例即收益上限。
  // 候选二（store 拍放开 load 预取）：统计「EX 级 load 的预取因 MEM 级
  // store 占用数据口地址线而放弃」的次数——此类 load 回退 2 拍路径白付
  // 1 拍，次数×1 即收益。
  // 判定手法同 BTBSTAT：指令按 ifid_pc/idex_pc 从 imem 读回解码；
  // idex_valid 排除冲刷气泡拍；!stall_young 保证停顿期间（id_redirect 与
  // 总线请求跨拍保持）不重复计数；!trap_any 排除陷入冲刷拍（该拍 EX 指令
  // 被撤销，不会进入 MEM 付回退代价）；!wfi_wait 排除 WFI 睡眠期（睡眠
  // 期间 pc 被 wfi_wait 优先保持，id_redirect 反复拉起并不发生真重定向，
  // 应用固件大部分时间在睡觉，不门控会严重虚增）。所探信号均为核心端口
  // 或非结构内部信号，SV/Verilog（rtl_v）两种 DUT 源码下层级路径均有效。
  int  c1_jal_cnt, c1_br_cnt, c1_ret_cnt, c1_other_cnt, c2_stblk_cnt, wfi_cnt;
  /* verilator lint_off UNUSEDSIGNAL */   // 只需操作码字段，其余位天然未用
  wire [31:0] id_instr = dut.imem_i.mem[dut.core_i.ifid_pc[IMEM_AW+1:2]];
  /* verilator lint_on UNUSEDSIGNAL */
  wire        c1_evt = dut.core_i.id_redirect && !dut.core_i.stall_young &&
                       !dut.core_i.wfi_wait;
  wire        c2_evt = dut.core_i.idex_valid && !dut.core_i.stall_young &&
                       !dut.core_i.trap_any &&
                       ex_instr[6:0] == 7'b0000011 &&              // EX 级为 load
                       dut.core_i.dmem_valid && dut.core_i.dmem_write;  // MEM 级 store
  always @(posedge clk) if (running) begin
    if (c1_evt) begin
      case (id_instr[6:0])
        7'b1101111: c1_jal_cnt <= c1_jal_cnt + 1;   // JAL 直转
        7'b1100011: c1_br_cnt  <= c1_br_cnt + 1;    // 分支预测 taken
        7'b1100111: c1_ret_cnt <= c1_ret_cnt + 1;   // JALR（RAS 返回命中）
        default:    c1_other_cnt <= c1_other_cnt + 1;  // 不应出现，打印出来供审计
      endcase
    end
    if (c2_evt) c2_stblk_cnt <= c2_stblk_cnt + 1;
    if (dut.core_i.wfi_wait) wfi_cnt <= wfi_cnt + 1;
  end

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
  int    uartlog_fd;
  logic        running;      // 复位释放后置 1（避免与异步复位混用告警）

  // ---- UART RX 命令注入（应用连续运行形态测试）：+RXFILE=<hex 文件> ----
  // 条目格式同 mbus_gen 脚本：40 位 {idle_clks[31:0], byte[7:0]}，发本字节
  // 前先空闲 idle_clks 个 clk，0xFFFFFFFF 为结束标记。注入起点：有
  // MONSCRIPT 时等 mbus_gen 发完脚本再留 20000 clk（末帧墙钟收尾 + 报告
  // 打印完成），否则复位后 60000 clk（等固件初始化）。结束时核对发完。
  localparam int RXF_MAX = 512;
  logic [39:0] rxscript[RXF_MAX];
  logic        rxfile_done;

  initial begin : rxfile_engine
    rxfile_done = 1'b0;
    wait (rst_n);
    if (rxfile == "") begin
      wait (0);                         // 无命令脚本：永久挂起
    end
    if (monscript_file != "") begin
      wait (gen_done);
      repeat (20000) @(posedge clk);
    end else begin
      repeat (60000) @(posedge clk);
    end
    begin
      // 先探测文件存在性：$readmemh 对缺失文件仅告警，会把全零条目
      // 当有效注入（表现为 golden 不一致而非「文件没找到」，难定位）
      int fd;
      fd = $fopen(rxfile, "r");
      if (fd == 0) $fatal(1, "tb_soc: 无法打开命令注入脚本 %s", rxfile);
      $fclose(fd);
    end
    $readmemh(rxfile, rxscript);
    $display("tb_soc: 加载命令注入脚本 %s", rxfile);
    for (int i = 0; i < RXF_MAX; i++) begin
      automatic logic [39:0] e = rxscript[i];
      if (e[39:8] == 32'hFFFF_FFFF) break;
      repeat (e[39:8]) @(posedge clk);
      uart_rxd = 1'b0; #(BIT_NS);
      for (int b = 0; b < 8; b++) begin uart_rxd = e[b]; #(BIT_NS); end
      uart_rxd = 1'b1; #(BIT_NS);
    end
    $display("tb_soc: 命令注入脚本发送完毕");
    rxfile_done = 1'b1;
  end

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
  string monscript_file;
  string rxfile;

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
      if (uartlog_fd != 0) $fwrite(uartlog_fd, "%c", ch);
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
  // 中断模式（+IRQCNT）或命令注入模式（+RXFILE）下关闭：前者该字节收完
  // 后 rx_valid 长期置位会成为计划外 MEI 中断源；后者会与命令流混在一起。
  initial begin : rx_driver
    logic [7:0] b;
    b = 8'hA5;
    uart_rxd = 1'b1;
    wait (rst_n);
    #5000;
    if (irq_cnt_addr != 0 || rxfile != "") begin
      wait (0);   // 永久挂起本激励（进程不再唤醒）
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
    bit    timeoutpass;
    bit    gpioend;
    bit    btbstat;
    bit    perfstat;
    int    timeout_cycles;
    int    errors;
    string uartlog_file;

    clk = 0; rst_n = 0; gpio_in = 32'h1234_5678;
    mispredict_cnt = 0;
    cycle_cnt = 0;
    jalr_unpred_cnt = 0;
    c1_jal_cnt = 0; c1_br_cnt = 0; c1_ret_cnt = 0; c1_other_cnt = 0;
    c2_stblk_cnt = 0; wfi_cnt = 0;
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
    // UART 日志落盘（应用 demo 判定链用）
    if (!$value$plusargs("UARTLOG=%s", uartlog_file)) uartlog_file = "";
    uartlog_fd = 0;
    if (uartlog_file != "") begin
      uartlog_fd = $fopen(uartlog_file, "w");
      if (uartlog_fd == 0) $fatal(1, "tb_soc: 无法打开 UART 日志文件 %s", uartlog_file);
    end
    // 中断模式（层 B）：+IRQCNT 给 handler 陷入计数器地址（非 0 启用），
    // +IRQEXP 给期望计数（含前导 MSI 一次），+IRQPLAN 给注入计划文件
    if (!$value$plusargs("IRQCNT=%h", irq_cnt_addr)) irq_cnt_addr = 0;
    if (!$value$plusargs("IRQEXP=%d", irq_exp)) irq_exp = -1;
    if (!$value$plusargs("IRQPLAN=%s", plan_file)) plan_file = "";
    if (!$value$plusargs("MONSCRIPT=%s", monscript_file)) monscript_file = "";
    if (!$value$plusargs("RXFILE=%s", rxfile)) rxfile = "";
    timeoutpass = $test$plusargs("TIMEOUTPASS");
    // +GPIOEND：以 GPIO 魔数写入为结束信号（中断驱动固件无停机概念），
    // 须与 +MAGIC 同用；结束后的魔数核对复用下方既有逻辑
    gpioend = $test$plusargs("GPIOEND");
    btbstat = $test$plusargs("BTBSTAT");
    perfstat = $test$plusargs("PERFSTAT");
    if (gpioend && !magic_en)
      $fatal(1, "tb_soc: +GPIOEND 需与 +MAGIC=<魔数> 同用");
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

    // 等待停机 / ACT tohost / GPIO 魔数写入 / 超时
    fork
      wait (halted);
      wait (act_done);
      wait (gpioend && gpio_out[15:0] != 16'h0);
      begin
        repeat (timeout_cycles) @(posedge clk);
        if (timeoutpass)
          $display("tb_soc: 到达 %0d 周期超时点，连续运行形态按正常结束处理", timeout_cycles);
        else
          $fatal(1, "tb_soc: 超时未到达结束条件");
      end
    join_any

    @(posedge clk); #1;
    $display("tb_soc: 停机原因码=%0d, 分支预测失败次数=%0d, 周期数=%0d",
             halt_code, mispredict_cnt, cycle_cnt);
    if (btbstat)
      $display("tb_soc: BTB 量化：未预测间接跳转（JALR 且 rs1 非 x1/x5）重定向 %0d 次，按 2 拍/次估算代价 %0d 拍（总周期 %0d）",
               jalr_unpred_cnt, jalr_unpred_cnt * 2, cycle_cnt);
    if (perfstat) begin
      $display("tb_soc: PERFSTAT 候选一(IF级预测)：ID 级重定向 JAL=%0d 分支taken=%0d RAS返回=%0d 其他=%0d，合计 %0d 次 ×1 拍 ≈ 收益上限 %.2f%%（总周期 %0d）",
               c1_jal_cnt, c1_br_cnt, c1_ret_cnt, c1_other_cnt,
               c1_jal_cnt + c1_br_cnt + c1_ret_cnt + c1_other_cnt,
               100.0 * (c1_jal_cnt + c1_br_cnt + c1_ret_cnt + c1_other_cnt) / cycle_cnt,
               cycle_cnt);
      $display("tb_soc: PERFSTAT 候选二(store放开预取)：EX 级 load 预取被 MEM 级 store 放弃 %0d 次 ×1 拍 ≈ 收益上限 %.2f%%",
               c2_stblk_cnt, 100.0 * c2_stblk_cnt / cycle_cnt);
      $display("tb_soc: PERFSTAT 参考：WFI 睡眠 %0d 拍（占比 %.2f%%）",
               wfi_cnt, 100.0 * wfi_cnt / cycle_cnt);
    end
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
    end else if (!timeoutpass) begin
      // TIMEOUTPASS（连续运行形态）：无停机概念，跳过停机码/魔数判定，
      // 正确性由 UARTLOG golden diff 与下方脚本发完核对保证。
      // GPIOEND（中断驱动固件）：mtvec 已配置无停机概念，跳过停机码核对，
      // 结束与判定都由 GPIO 魔数承担
      if (!gpioend && halt_code != halt_code_t'(expect_code)) begin
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

    // 命令注入脚本核对：同理，命令未发完则响应缺失，golden diff 之前的
    // 运行已不可信
    if (rxfile != "" && !rxfile_done) begin
      errors++;
      $display("FAIL: 命令注入脚本未发送完毕（超时预算不足或注入引擎异常）");
    end

    if (errors == 0) $display("tb_soc: PASS");
    else             $fatal(1, "tb_soc: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
