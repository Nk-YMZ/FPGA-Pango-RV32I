#!/usr/bin/env python3
"""生成 demo 监听脚本（mbus_gen 脚本 ROM 的 hex 映像，写 stdout）。

用法：gen_script.py [--scenario 名称]，默认 normal；场景清单见 SCENARIOS。
帧尾 CRC 由本脚本独立计算（crc16_modbus），与固件实现互为交叉验证，
并经 CRC 标准目录已知答案向量（"123456789" → 0x4B37）核对。
每条目 40 位：{idle_clks[31:0], byte[7:0]}，idle=0xFFFFFFFF 为结束标记。

BITDIV 必须与 tb_soc 中 mbus_gen 参数及 uart_mon 复位默认值一致。
脚本 hex 是运行期加载（+MONSCRIPT），改场景不需要重新 verilate。
"""
import argparse

BITDIV = 32               # 位周期（clk），--bitdiv 可覆盖；必须与 mbus_gen
                          #（+MONBITDIV）及 uart_mon BITDIV（固件读回值）一致
CHAR = 11 * BITDIV          # 一字符 = 起始位 + 8 数据位 + 停止位 = 11 位

OPEN_IDLE = 50000           # 开场空闲：留足固件启动与采集使能时间
RSP_DELAY = 4 * CHAR        # 设备回复延迟（> t3.5=3.5 字符切帧阈值）


def crc16_modbus(data: bytes) -> int:
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ (0xA001 if crc & 1 else 0)
    return crc


def frame(payload: bytes) -> bytes:
    """补 Modbus RTU 帧尾 CRC（低字节先发）"""
    c = crc16_modbus(payload)
    return payload + bytes([c & 0xFF, c >> 8])


# FC03 请求：addr=01，起始寄存器 0x0000，读 2 个保持寄存器
REQ = frame(bytes([0x01, 0x03, 0x00, 0x00, 0x00, 0x02]))
# 正常回复：addr=01，字节数 04，寄存器值 0x0100(256)、0x0258(600)
RSP = frame(bytes([0x01, 0x03, 0x04, 0x01, 0x00, 0x02, 0x58]))


def scenario_normal() -> list:
    """正常一问一答（可行性基线；输出必须与初版 demo 逐字节一致）"""
    return [(OPEN_IDLE, REQ), (RSP_DELAY, RSP)]


def scenario_bad_crc() -> list:
    """回复 CRC 故意损坏：应判校验错误、不算有效回复，随后超时"""
    bad = bytearray(RSP)
    bad[-1] ^= 0xFF
    return [(OPEN_IDLE, REQ), (RSP_DELAY, bytes(bad))]


def scenario_truncated() -> list:
    """截断帧：回复只发 3 字节总线即静默 → 长度异常，随后超时"""
    return [(OPEN_IDLE, REQ), (RSP_DELAY, RSP[:3])]


def scenario_exception() -> list:
    """协议异常回复：功能码 | 0x80 + 异常码 02（非法数据地址）。
    异常回复是设备的有效应答（设备在线但拒绝），不应报超时。"""
    exr = frame(bytes([0x01, 0x83, 0x02]))
    return [(OPEN_IDLE, REQ), (RSP_DELAY, exr)]


def scenario_unsupported_fc() -> list:
    """未支持功能码（FC06 写单寄存器）：CRC 正确，不算设备故障，
    也不应挂起待回复（只有 FC03 请求挂起）。"""
    fc06 = frame(bytes([0x01, 0x06, 0x00, 0x00, 0x00, 0x03]))
    return [(OPEN_IDLE, fc06)]


def scenario_timeout() -> list:
    """无回复：请求发出后总线静默 → 超时报告，不误报其他故障"""
    return [(OPEN_IDLE, REQ)]


def scenario_broadcast() -> list:
    """广播请求（addr=0）：Modbus 广播不期待回复，应如实打印 REQ 但不
    挂起待回复、不报超时（FC03 广播本身非法，属防御性口径验证）"""
    breq = frame(bytes([0x00, 0x03, 0x00, 0x00, 0x00, 0x02]))
    return [(OPEN_IDLE, breq)]


def scenario_stress(pairs: int, gap: int) -> list:
    """压力测试：pairs 对请求+回复背靠背，帧间间隔统一 gap clk。
    间隔必须 > 切帧阈值（3.5 字符 = 1232 clk）否则并帧；当间隔小于
    固件帧处理周期（含报告打印）时 uart_mon FIFO 持续堆积，pairs
    足够大则溢出——临界点由 OVF_CNT 说话（测量性质，不进 demo 门禁）。"""
    seq = []
    for i in range(pairs):
        seq.append((OPEN_IDLE if i == 0 else gap, REQ))
        seq.append((gap, RSP))
    return seq


def scenario_multidev() -> list:
    """多设备分账：四台从站共线——01 两轮正常问答（第二轮回复延迟 8
    字符，拉开响应时间分布）、02 正常（不同寄存器值）、03 异常回复
    （在线但拒绝）、04 无回复超时。画像应逐台独立累计。
    04 的超时上报（16 字符）先于 01 第二轮请求（20 字符后）到来：新请求
    会覆盖单挂起槽位（design.md 6.4），间隔必须大于超时阈值。"""
    req2 = frame(bytes([0x02, 0x03, 0x00, 0x00, 0x00, 0x02]))
    rsp2 = frame(bytes([0x02, 0x03, 0x04, 0x00, 0x0a, 0x00, 0x14]))
    req3 = frame(bytes([0x03, 0x03, 0x00, 0x00, 0x00, 0x02]))
    exr3 = frame(bytes([0x03, 0x83, 0x02]))
    req4 = frame(bytes([0x04, 0x03, 0x00, 0x00, 0x00, 0x02]))
    return [
        (OPEN_IDLE, REQ), (RSP_DELAY, RSP),
        (RSP_DELAY, req2), (RSP_DELAY, rsp2),
        (RSP_DELAY, req3), (RSP_DELAY, exr3),
        (RSP_DELAY, req4),
        (20 * CHAR, REQ), (8 * CHAR, RSP),
    ]


SCENARIOS = {
    "normal": scenario_normal,
    "bad_crc": scenario_bad_crc,
    "truncated": scenario_truncated,
    "exception": scenario_exception,
    "unsupported_fc": scenario_unsupported_fc,
    "timeout": scenario_timeout,
    "broadcast": scenario_broadcast,
    "multidev": scenario_multidev,
}


def main() -> None:
    ap = argparse.ArgumentParser(description="生成 mbus_gen 监听脚本 hex 映像")
    ap.add_argument("--scenario", default="normal",
                    choices=sorted(SCENARIOS) + ["stress"],
                    help="场景名称（默认 normal）")
    ap.add_argument("--pairs", type=int, default=30, help="stress：请求-回复对数")
    ap.add_argument("--gap", type=int, default=2000,
                        help="stress：帧间间隔（clk，须大于 3.5 字符切帧阈值）")
    ap.add_argument("--bitdiv", type=int, default=BITDIV,
                        help="位周期（clk），回复延迟等按字符时间随之换算")
    args = ap.parse_args()

    if args.bitdiv != BITDIV:
        global CHAR, RSP_DELAY
        CHAR = 11 * args.bitdiv
        RSP_DELAY = 4 * CHAR

    if args.scenario == "stress":
        flow = scenario_stress(args.pairs, args.gap)
    else:
        flow = SCENARIOS[args.scenario]()

    entries = []
    for idle_before, data in flow:
        for i, b in enumerate(data):
            entries.append((idle_before if i == 0 else 0, b))
    entries.append((0xFFFFFFFF, 0))   # 结束标记

    for idle, b in entries:
        print(f"{idle:08x}{b:02x}")


if __name__ == "__main__":
    main()
