"""Modbus RTU 字节工具。构造 FC03 请求与回应，添加或验证 CRC16。
寄存器字段采用大端字节序，CRC 帧尾采用低字节在前的顺序。
"""
# FC03 默认寄存器值：256、600。
REG_VALUES = (0x0100, 0x0258)     # 256、600


def crc16(data: bytes) -> int:
    """Modbus CRC16（多项式 0xA001 反向，初值 0xFFFF）"""
    # 按低位先处理的 Modbus 多项式逐字节更新；每字节执行八次移位和条件异或。
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


# 帧长不足以包含 CRC 时判失败；比较最后两字节与其前全部字节的 CRC。
def check_crc(frame: bytes) -> bool:
    return len(frame) >= 4 and add_crc(frame[:-2]) == frame


# 请求固定为 8 字节：地址、功能码、起始寄存器、寄存器数量和 CRC。
def build_fc03_request(addr: int, start: int, count: int) -> bytes:
    """FC03 读保持寄存器请求：addr fc start_hi start_lo n_hi n_lo crc"""
    return add_crc(bytes([addr, 0x03, start >> 8, start & 0xFF,
                          count >> 8, count & 0xFF]))


# 回应字节数为 5 + 2×寄存器数量；每个寄存器先发送高字节。
def build_fc03_response(addr: int, values: tuple) -> bytes:
    """FC03 正常回复：addr fc bytecount data... crc"""
    payload = bytes([addr, 0x03, 2 * len(values)])
    for v in values:
        payload += bytes([v >> 8, v & 0xFF])
    return add_crc(payload)


def selftest() -> None:
    assert crc16(b"123456789") == 0x4B37, hex(crc16(b"123456789"))
    # 已知请求与回应序列的字节次序及 CRC 断言。
    assert build_fc03_request(1, 0, 2) == bytes([0x01, 0x03, 0x00, 0x00,
                                                 0x00, 0x02, 0xC4, 0x0B])
    rsp = build_fc03_response(1, REG_VALUES)
    assert rsp == bytes([0x01, 0x03, 0x04, 0x01, 0x00, 0x02, 0x58,
                         0xFB, 0x55])
    assert check_crc(rsp) and not check_crc(rsp[:-1] + bytes([rsp[-1] ^ 0xFF]))
    print("mbus selftest: PASS")
