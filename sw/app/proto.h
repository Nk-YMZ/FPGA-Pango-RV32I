// 通信解析接口和统计结构。调用者按接收顺序提供带时间戳的字节记录。
// proto_tick 仅在软硬件接收队列均排空后调用；时间间隔采用 32 位无符号差值。
// 设备记录、配对周期及事件钩子的单位和分类见各声明旁注释。
#ifndef PROTO_H
#define PROTO_H

#include <stdint.h>

// 全局统计按整帧分类累计；UART 停止位错误按字节另计，不另占帧分类。
// resp_cycles 仅在一次请求与正常/异常回应成功匹配时更新。
typedef struct {
    uint32_t frames;        // 完整帧数（含各类错误帧）
    uint32_t crc_err;       // CRC 校验失败帧数
    uint32_t frame_err;     // 串口帧错误（停止位错误）字节数
    uint32_t len_err;       // 长度/格式异常帧数
    uint32_t unsupported;   // CRC 正确但功能码未支持的帧数（不算设备故障）
    uint32_t requests;      // FC03 请求数
    uint32_t responses;     // FC03 正常回复数
    uint32_t exc_rsp;       // 异常回复数（fc|0x80：设备在线但拒绝，算有效应答）
    uint32_t timeouts;      // 请求超时数（阈值内未观察到有效回复）
    uint32_t resp_cycles;   // 最近一次响应时间（请求末字节→回复首字节，clk）
} proto_stats_t;

// gap_thresh：帧间空闲阈值（clk），调用者按 BITDIV 换算 t3.5 传入；
// timeout_thresh：请求待回复超时阈值（clk），超时报「未观察到有效回复」。
// 分类不变式：frames == requests+responses+exc_rsp+crc_err+len_err+unsupported
void proto_init(uint32_t gap_thresh, uint32_t timeout_thresh);

// 仅控制正常回应的文本预览；整帧处理和全部诊断保持不变。
void proto_set_compact_report(int compact);
int proto_compact_report(void);

// 喂入一条记录：ts 为接收完成时刻时间戳，flags bit0=frame_err
void proto_feed(uint32_t ts, uint8_t data, uint8_t flags);

// 无新字节时的帧收尾与超时检查：now 与最后字节时间戳之差超阈值则关闭当前帧；
// 待回复请求超 timeout_thresh 未观察到有效回复则上报并解除挂起。
// 注意：只能在「已追平硬件队列」（FIFO 已空）时调用——若 FIFO 里还有
// 待消费记录，本帧后续字节可能就在队列里，墙钟收尾会把帧错切。
void proto_tick(uint32_t now);

// 存在未收尾帧或待回应请求时返回非零。
int proto_pending(void);

// 清统计与解析状态（未收尾帧、待回复请求一并作废），保留阈值配置
void proto_clear(void);

// 运行时查询/修改待回复超时阈值（clk）；配置命令接口用
uint32_t proto_get_timeout(void);
void     proto_set_timeout(uint32_t t);

const proto_stats_t *proto_stats(void);

// paired_rsp 只统计已匹配待回应请求的正常或异常回应；
// responses/exc_rsp 还包含孤立回应，后者不产生配对周期样本。
uint32_t proto_paired_rsp(void);

// Modbus CRC16（proto.c 内部帧校验同款实现）；自动波特率扫描验证器
// （main.c）用它判定候选位速率下收到的帧是否有效
uint16_t proto_crc16(const uint8_t *d, uint32_t n);

// ---- 故障事件钩子 ----
// 诊断上报点同步抛出事件，由应用侧（main.c）实现：进事件环形日志、
// 驱动 GPIO 告警；本层不接触 MMIO。ts 为事件时刻（帧事件取末字节
// 时间戳，超时取判定时刻）；addr 仅对设备相关事件有效（其余为 0）；
// aux 语义随类型：crc_err/len_err=帧长，timeout=全局累计超时数，
// interference=连续坏帧数，retry-storm=连续重试数，offline=该设备
// 累计超时数，recover=0。
enum {
    EV_CRC_ERR = 1,     // CRC 校验失败帧（单帧，不告警）
    EV_LEN_ERR,         // 长度/格式异常帧（单帧，不告警）
    EV_TIMEOUT,         // 待回复超时（告警）
    EV_INTERFERENCE,    // DIAG 链路干扰（告警）
    EV_RETRY_STORM,     // DIAG 重试风暴（告警）
    EV_OFFLINE,         // DIAG 设备离线（告警）
    EV_RECOVER          // DIAG 设备恢复（不告警，也不自动撤告警）
};
void app_log_event(uint32_t ts, uint8_t type, uint8_t addr, uint32_t aux);

// ---- 设备画像（多设备分账） ----
// 只给 CRC 校验通过且可解析的 FC03 帧建账：crc_err/len_err 帧的地址域
// 不可信、unsupported 帧方向未知，均只计总线级不归因设备；广播（addr=0）
// 不建账。槽位按地址首次出现顺序分配，表满时建账失败计 proto_dev_ovf
// （按失败次数而非去重地址数——表满无处记录哪些地址溢出过，同一新
// 地址的请求与回复各计一次；不静默丢弃）。
#define PROTO_MAX_DEV 8

typedef struct {
    uint8_t  addr;
    uint8_t  offline;       // 已上报离线（再次应答报恢复并清零，见文件头）
    uint32_t requests;      // FC03 请求数
    uint32_t responses;     // 正常回复数
    uint32_t exc_rsp;       // 异常回复数
    uint32_t timeouts;      // 该设备请求超时数
    uint32_t resp_n;        // 响应时间样本数（已配对响应）
    uint32_t resp_last;     // 最近一次响应时间（clk）
    // resp_n 为零时尚无配对时间样本；首次配对后 min/max 才具有实际响应时间含义。
    uint32_t resp_min;
    uint32_t resp_max;
} proto_dev_t;

// 已建账设备数 / 表满建账失败计数（口径见上）；proto_dev 按首次出现
// 顺序取设备（越界返 0）
uint32_t proto_dev_count(void);
uint32_t proto_dev_ovf(void);
const proto_dev_t *proto_dev(uint32_t idx);

#endif
