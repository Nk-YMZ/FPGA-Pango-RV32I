// 振动分析接口。输入窗口为 1024 个有符号 16 位样本，信号采样率为 8000 Hz。
// 结果包括主峰、目标频带占比、时域幅值、峰值因子及背景扰动。
#ifndef VIBRATION_H
#define VIBRATION_H
#include <stdint.h>
#define VIB_N 1024u
#define VIB_SAMPLES 124u
#define VIB_BINS 64u
#define VIB_FCLK 90000000u
typedef struct {
    // peak_bin 为 FFT 下标，peak_hz 为 Hz；high_permille 为 0..1000 的频带功率占比。
    // alarm 表示频带占比达到设定值。
    uint32_t peak_bin, peak_hz, high_permille, alarm;
    // 64 组相对峰值功率，0..65535；每组取 8 个 FFT 频点中的最大值。
    uint16_t spectrum[VIB_BINS];
    // rms、peak、noise 单位为采样值；crest10 为峰值因子的十倍。
    uint32_t rms,peak,crest10,noise;
} vib_result_t;
// samples 指向按时间排列的完整窗口；result 由调用者提供并接收全部计算结果。
// 分析工作区在模块内静态保存，同一时刻只执行一次分析调用。
void vib_analyze(const int16_t *samples, vib_result_t *result);
// 仅接收地址 1、125 个寄存器的数据区，首寄存器为循环序号，后 124 个为采样点。
// 此入口只解码及入队，实际组窗与计算由主循环调用 vib_service 完成。
void app_vib_samples(uint8_t addr, const uint8_t *data, uint32_t registers);
void vib_service(void);
int vib_command(const char *command);
int vib_stream_enabled(void);
#endif
