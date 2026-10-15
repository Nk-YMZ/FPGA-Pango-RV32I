#!/usr/bin/env python3
"""生成 demo 监听脚本（mbus_gen 脚本 ROM 的 hex 映像，写 stdout）。

场景：1 条 FC03 读保持寄存器请求 + 4 字符空闲 + 1 条正常回复。
帧尾 CRC 由本脚本独立计算（crc16_modbus），与固件实现互为交叉验证。
每条目 40 位：{idle_clks[31:0], byte[7:0]}，idle=0xFFFFFFFF 为结束标记。

BITDIV 必须与 tb_soc 中 mbus_gen 参数及 uart_mon 复位默认值一致。
"""

BITDIV = 32
CHAR = 11 * BITDIV          # 一字符 = 起始位 + 8 数据位 + 停止位 = 11 位


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


def main() -> None:
    entries = []

    def send_frame(idle_before: int, data: bytes) -> None:
        for i, b in enumerate(data):
            entries.append((idle_before if i == 0 else 0, b))

    # FC03 请求：addr=01，起始寄存器 0x0000，读 2 个保持寄存器
    req = frame(bytes([0x01, 0x03, 0x00, 0x00, 0x00, 0x02]))
    # 正常回复：addr=01，字节数 04，寄存器值 0x0100(256)、0x0258(600)
    rsp = frame(bytes([0x01, 0x03, 0x04, 0x01, 0x00, 0x02, 0x58]))

    send_frame(50000, req)            # 开场空闲：留足固件启动与采集使能时间
    send_frame(4 * CHAR, rsp)         # 帧间 4 字符空闲（> t3.5=3.5 字符）
    entries.append((0xFFFFFFFF, 0))   # 结束标记

    for idle, b in entries:
        print(f"{idle:08x}{b:02x}")


if __name__ == "__main__":
    main()
