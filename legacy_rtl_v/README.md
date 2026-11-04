# rtl_v/ —— PDS 交付用 Verilog-2005 源码

本目录是 `rtl/`（SystemVerilog）经 sv2v 转换生成的 Verilog-2005 交付件，
供上板阶段使用：紫光 PDS 对部分高级 SystemVerilog 语法支持不全，上板
工程直接添加本目录的 `.v` 文件即可，无需再做语法改写。`soc_top.v` 为
顶层，端口与 `rtl/soc_top.sv` 一致。

## 规则

- **自动生成，禁止手改**。修改一律改 `rtl/`，然后 `make verilog` 重新
  生成，两份源码在同一提交内入库。
- `make vcheck`（已挂入 `make lint`）重新生成到临时目录并与本目录逐文件
  diff，不一致即失败——RTL 改动提交门禁之一。
- sv2v 版本变化可能改变输出格式：工具链升级后须重新生成并随同提交
  （与时序基线的工具链口径一致）。

## 转换口径

- 仿真断言等 `` `ifndef SYNTHESIS `` 块已剥离（`-DSYNTHESIS`）。
- core_pkg 包内容（参数/枚举/结构体）已按模块内联，各 `.v` 自包含、
  无跨文件依赖，编译顺序无关。
- imem 的存储数组经后处理改回 reg（原因见 `imem.v` 内注释与
  `scripts/rtl_v/fix_imem_v.py` 头注释）；**程序内容的加载方式
  （bitstream 初始化文件 / 引导逻辑）由上板工程决定**，本仓库不替
  上板阶段拍板。

## lint 口径

交付件生成时须过 Verilator -Wall；仅豁免 UNDRIVEN（imem 存储数组无驱动
是设计意图）与 UNUSEDSIGNAL/WIDTHEXPAND（sv2v 译法固有的风格噪音，如
`'0` 译为 `1'sb0` 靠零扩展）。严格风格 lint 的把关在 `rtl/` 源码上
（`make lint`，零告警），本目录 lint 的职责是检出转换级破坏。

## 上板注意（本仓库无法验证的部分，须以 PDS 实测为准）

- BRAM（imem/dmem）与 DSP（乘法器）的推断效果须经 PDS 综合报告确认。
  推断模板在 `rtl/` 源中按 Pango 习惯编写，sv2v 转换保持结构不变，
  但「转换后 PDS 仍按模板推断」这一点本仓库环境无法验证。
- 语义等价性由仿真背书：测试台可直接以本目录为 DUT 源码重建
  （`make test RTL_DIR=rtl_v`，首次建立时已全量通过），每次 RTL
  迭代随验证流程执行。
