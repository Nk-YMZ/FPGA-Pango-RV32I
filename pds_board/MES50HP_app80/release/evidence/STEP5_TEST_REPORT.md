# 自动波特率修正版：已上板验证

2026-10-22，MES50HP，80 MHz，CPU仍为原aggressive17。修正版工程已通过PDS命令行下载到实际板卡，done=1。旧工程及位流保留。

## 改动

- 固件扫描保留48字符基础窗口；已开始且无串口错误的短帧最多增加36字符收尾，避免刚好切候选时截断帧。
- 收到第一帧CRC有效证据后，最多等待2秒确认下一帧，适配稀疏轮询。仍然要求两帧CRC有效；单帧或垃圾不会无限拖住候选。
- 手动timeout周期数跨自动重锁与clear保留，复位后恢复默认。
- 未修改CPU RTL、外设RTL、引脚、PLL和时钟约束；仅更新片上存储器中的固件。

## 真实硬件结果

| 流量 | 切换方向 | 旧版 | 修正版 |
|---|---|---|---|
| 原测试普通轮询 | 9600→19200 | 约3.0秒锁定 | 3.042秒锁定 |
| 原测试普通轮询 | 19200→9600 | 75秒、266次问答未锁定 | 1.611秒锁定 |
| 每轮额外间隔1秒 | 9600→19200 | 未做对照 | 16.308秒锁定 |
| 每轮额外间隔1秒 | 19200→9600 | 未做对照 | 7.709秒锁定 |

每次锁定后都重新检查3次正常问答：6帧、51字节、寄存器256/600，CRC/帧错误/超时/溢出均0。四次锁定后timeout均直接读回8000000周期（100ms），验证前没有重新设置门限。

四种故障（CRC错误、200ms延迟、不回复、先正常3次后离线）和normal恢复也已在修正版完整重跑通过。告警置位、恢复后锁存、手动清除正确；事件日志包含timeout/offline/recover，时间戳顺序及logclear作用正确。物理LED后续实物确认结果见FINAL_HARDWARE_TEST_REPORT.md。

## 软件与实现验证

- 8项定向测试直接编译实际main.c中的扫描函数，覆盖帧边界、稀疏确认、单帧等待上限、连续不完整帧、错误CRC/帧错误拒绝、32位时间戳绕回及手动门限保留。
- 标准单元、CPU冒烟、SoC集成与18项协议宿主测试通过（协议宿主启用ASan/UBSan）。
- 15项应用demo及2项命令接口golden对账通过。
- Windows原环境缺失部分库且存在C++运行库不匹配，因此在现有Ubuntu环境补齐build-essential、Verilator和zlib开发依赖完成回归。使用Verilator5.032、RISC-V GCC14.2；仅对测试台时钟的BLKSEQ告警作调用参数豁免，文本比较去除Windows CRLF差异。产品RTL无改动。
- 原版固件在同一回归环境也有4处超时事件绝对时间戳与旧golden不同；修正版按当前确定的输出更新5处时间戳。事件类型、顺序、数量、响应周期及其他文本逐字相同；保留原版基线对照和旧golden，不通过放宽功能判据掩盖错误。
- PDS80MHz全部约束通过：setup +0.507ns，估计Fmax83.382MHz，与原工程相同。LUT4224、FF2858、DRM64、PLL1保持不变。

## 交付和回退

当前工程：D:/1014/doublefixed/project1020_mes50hp_app80_abfix/project1020_mes50hp_app80_abfix.pds

当前位流：D:/1014/doublefixed/project1020_mes50hp_app80_abfix/generate_bitstream/mes50hp_app80_top.sbit

本交付目录firmware、generate_bitstream、report_timing、device_map已同步修正版。原工程D:/1014/doublefixed/project1020_mes50hp_app80保持原样；原main.c、HEX和SBIT备份在fixes/autobaud_v2/original。

自动波特率硬件原始记录：logs/step5_v2_20261022_213345。四种故障复验：logs/step4_faults_20261022_213602。旧版失败报告保留在fixes/autobaud_v2/original/STEP5_TEST_REPORT_v1.md。

自动波特率复验结束时：板上为修正版，9600监听、100ms超时，PuTTY恢复，COM8 normal设备运行，板上保留复验历史。后续测试和交接状态以FINAL_HARDWARE_TEST_REPORT.md、FINAL_SESSION_STATE.json为准。

本轮完成自动波特率修复及其回归；随后三地址故障隔离、独立恢复和10分钟长跑均通过，物理LED结果见FINAL_HARDWARE_TEST_REPORT.md。稀疏或无流量时锁定耗时依赖有效帧数量，不保证固定几秒。统一操作教学已整理在统一操作与查看.md。

源码已提交到独立分支codex/autobaud-v2-20261022，提交9f996ec；原rv32i-main分支指针保留。提交身份为本次代理Codex，本机全局Git配置未改。

故障复验的no_reply场景还自然跨过一次32位监听时间戳绕回（4224869338→10445830），5次超时计数仍正确；此记录不替代10分钟长跑。
