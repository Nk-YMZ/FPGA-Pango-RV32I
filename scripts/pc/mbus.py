"""Modbus RTU 帧构造与解析（PC 侧联调脚本共用，纯标准库）。

与仿真侧 gen_script.py 的 CRC 实现互相独立（有意为之：交叉验证），
自检经已知答案向量（"123456789" → 0x4B37）与仿真场景字节流核对。
"""

# 与仿真 demo 场景一致的寄存器值，便于仿真/上板两面对账
REG_VALUES = (0x0100, 0x0258)     # 256、600


def crc16(data: bytes) -> int:
    """Modbus CRC16（多项式 0xA001 反向，初值 0xFFFF）"""
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ (0xA001 if crc & 1 else 0)
    return crc


def add_crc(payload: bytes) -> bytes:
    """补帧尾 CRC（低字节先发）"""
    c = crc16(payload)
    return payload + bytes([c & 0xFF, c >> 8])


def check_crc(frame: bytes) -> bool:
    return len(frame) >= 4 and add_crc(frame[:-2]) == frame


def build_fc03_request(addr: int, start: int, count: int) -> bytes:
    """FC03 读保持寄存器请求：addr fc start_hi start_lo n_hi n_lo crc"""
    return add_crc(bytes([addr, 0x03, start >> 8, start & 0xFF,
                          count >> 8, count & 0xFF]))


def build_fc03_response(addr: int, values: tuple) -> bytes:
    """FC03 正常回复：addr fc bytecount data... crc"""
    payload = bytes([addr, 0x03, 2 * len(values)])
    for v in values:
        payload += bytes([v >> 8, v & 0xFF])
    return add_crc(payload)


def selftest() -> None:
    assert crc16(b"123456789") == 0x4B37, hex(crc16(b"123456789"))
    # 与仿真场景（scripts/demo/gen_script.py 产物）逐字节互核对
    assert build_fc03_request(1, 0, 2) == bytes([0x01, 0x03, 0x00, 0x00,
                                                 0x00, 0x02, 0xC4, 0x0B])
    rsp = build_fc03_response(1, REG_VALUES)
    assert rsp == bytes([0x01, 0x03, 0x04, 0x01, 0x00, 0x02, 0x58,
                         0xFB, 0x55])
    assert check_crc(rsp) and not check_crc(rsp[:-1] + bytes([rsp[-1] ^ 0xFF]))
    print("mbus selftest: PASS")
