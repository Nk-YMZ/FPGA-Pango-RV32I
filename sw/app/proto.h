/* 协议解析核心（与硬件无关）：由调用者喂入带硬件时间戳的字节记录，
 * 完成按空闲间隔切帧、Modbus CRC16 校验、FC03 解析、请求-回复配对与统计。
 *
 * 取数与处理严格分离：本层不接触任何 MMIO，记录由谁喂入（轮询主循环 /
 * 中断服务程序）对解析逻辑不可见，后续升级中断驱动时本层不变。
 * 输出经 app_put_* 钩子（由应用侧实现），本层同样不直接操作串口。 */
#ifndef PROTO_H
#define PROTO_H

#include <stdint.h>

typedef struct {
    uint32_t frames;        /* 完整帧数（含各类错误帧） */
    uint32_t crc_err;       /* CRC 校验失败帧数 */
    uint32_t frame_err;     /* 串口帧错误（停止位错误）字节数 */
    uint32_t len_err;       /* 长度/格式异常帧数 */
    uint32_t unsupported;   /* CRC 正确但功能码未支持的帧数（不算设备故障） */
    uint32_t requests;      /* FC03 请求数 */
    uint32_t responses;     /* FC03 正常回复数 */
    uint32_t resp_cycles;   /* 最近一次响应时间（请求末字节→回复首字节，clk） */
} proto_stats_t;

/* gap_thresh：帧间空闲阈值（clk），调用者按 BITDIV 换算 t3.5 传入 */
void proto_init(uint32_t gap_thresh);

/* 喂入一条记录：ts 为接收完成时刻时间戳，flags bit0=frame_err */
void proto_feed(uint32_t ts, uint8_t data, uint8_t flags);

/* 无新字节时的帧收尾：now 与最后字节时间戳之差超阈值则关闭当前帧。
 * 注意：只能在「已追平硬件队列」（FIFO 已空）时调用——若 FIFO 里还有
 * 待消费记录，本帧后续字节可能就在队列里，墙钟收尾会把帧错切。 */
void proto_tick(uint32_t now);

const proto_stats_t *proto_stats(void);

#endif
