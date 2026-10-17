#!/usr/bin/env python3
"""结果显示（阶段二/三联调）：打开板载结果串口，把分析仪固件的文本输出
解析成结构化事件显示，退出时给统计摘要。

用法：
  python3 display.py --port /dev/ttyUSB2 [--baud 115200] [--fclk 50000000]
  python3 display.py --file build/demo_out_normal.txt   # 解析已有日志
  python3 display.py --selftest   # 无硬件自检（实解析 demo golden 文件）

固件输出协议（sw/app/，整行 ASCII）：banner / REQ / RSP / EXR / TIMEOUT /
DIAG / FRAME / STAT / DEVS / DEV / PERF / LOG / AUTOBAUD / OK / ERR /
timeout=<n> / alarm=<0|1> / cmds: / RESULT。DIAG 为跨帧模式诊断行：
DIAG link-interference / retry-storm / offline / recover（见 proto.h 文件头
口径）。LOG 为故障事件日志导出（概要行 + 逐条事件）；AUTOBAUD 为自动
波特率（误配触发扫描 / 锁定）。
--fclk 用于把周期数换算成时间，须按 PDS 工程实际时钟设置（示例 50 MHz）。
依赖 pyserial（仅实际开串口时需要）。
Windows 要点：串口名用 --port COMx（如 COM3）；pyserial 用
pip install pyserial 安装；统一自检入口为 selftest.py（纯 Python）。
"""
import argparse
import re
import sys
import time
from pathlib import Path

GOLDEN_DIR = Path(__file__).resolve().parent.parent / "demo" / "golden"

# key=value 片段（值含 cyc 后缀的去掉）
_KV = re.compile(r"(\w+)=([0-9a-fA-F,]+?)(cyc)?(?=\s|$)")


def _kvs(text: str) -> dict:
    """提取 key=value 片段（丢弃 cyc 后缀捕获组）"""
    return {k: v for k, v, _ in _KV.findall(text)}


def parse_line(line: str):
    """解析一行固件输出 → (kind, fields)；未识别返回 ("other", {})。
    kind: banner/req/rsp/exr/timeout/frame/stat/perf/ok/err/cfg/help/result"""
    line = line.rstrip("\r\n")
    if not line:
        return ("other", {})
    if line.startswith(("app-mon:", "app-demo:")):
        return ("banner", {"text": line})
    if line.startswith("RESULT:"):
        return ("result", {"pass": "PASS" in line})
    m = re.match(r"^(REQ|RSP|EXR|TIMEOUT)\s+(.*)$", line)
    if m:
        return (m.group(1).lower(), _kvs(m.group(2)))
    m = re.match(r"^DIAG\s+([\w-]+)(.*)$", line)
    if m:
        f = _kvs(m.group(2))
        f["kind"] = m.group(1)
        return ("diag", f)
    m = re.match(r"^FRAME\s+(\w+)(.*)$", line)
    if m:
        f = _kvs(m.group(2))
        f["kind"] = m.group(1)
        return ("frame", f)
    if line.startswith("AUTOBAUD "):
        # 自动波特率："AUTOBAUD scan start" / "AUTOBAUD lock bitdiv=<n>"
        return ("autobaud", _kvs(line))
    if line.startswith("LOG "):
        # 事件日志：概要行 "LOG n=<n> drop=<n>"；条目行
        # "LOG #<seq> ts=<cyc> <事件名> [addr=hh] aux=<n>"
        rest = line[4:]
        f = _kvs(rest)
        m = re.match(r"^#(\d+)\s+ts=\d+\s+([\w-]+)", rest)
        if m:
            f["seq"], f["kind"] = m.group(1), m.group(2)
        return ("log", f)
    m = re.match(r"^(STAT|PERF|DEVS|DEV)\s+(.*)$", line)
    if m:
        return (m.group(1).lower(), _kvs(m.group(2)))
    if line.startswith(("OK", "ERR")):
        tag, _, rest = line.partition(" ")
        return (tag.lower(), {"text": rest})
    if re.match(r"^\w+=", line):
        # 纯 key=value 行（配置查询响应，如 timeout=5632 / bitdiv=32）
        return ("cfg", _kvs(line))
    if line.startswith("cmds:"):
        return ("help", {"text": line})
    return ("other", {"text": line})


def fmt_us(cyc: str, fclk: float) -> str:
    return f"{int(cyc) / fclk * 1e6:.1f}us"


def show(kind: str, f: dict, fclk: float) -> str:
    """事件格式化（cyc 字段附带按 fclk 换算的时间）"""
    if kind == "req":
        return f"请求  addr={f['addr']} 起始={f['start']} 数量={f['nreg']}"
    if kind == "rsp":
        s = f"回复  addr={f['addr']} 数据={f.get('data', '')}"
        if "resp" in f:
            s += f" 响应时间={f['resp']}cyc({fmt_us(f['resp'], fclk)})"
        return s
    if kind == "exr":
        return (f"异常回复  addr={f['addr']} fc={f.get('fc')} "
                f"code={f.get('code')}"
                + (f" 响应时间={f['resp']}cyc({fmt_us(f['resp'], fclk)})"
                   if "resp" in f else ""))
    if kind == "timeout":
        return (f"超时  addr={f['addr']} 阈值={f['thresh']}cyc"
                f"({fmt_us(f['thresh'], fclk)}) 内未观察到有效回复")
    if kind == "frame":
        return f"帧错误  {f['kind']}" + (f" fc={f['fc']}" if "fc" in f else "")
    if kind == "diag":
        names = {"link-interference": "链路干扰（连续 CRC 错误突发）",
                 "retry-storm": "重试风暴",
                 "offline": "设备离线", "recover": "设备恢复"}
        s = f"诊断  {names.get(f['kind'], f['kind'])}"
        if "addr" in f:
            s += f"  addr={f['addr']}"
        if "crc_burst" in f:
            s += f"  连续坏帧={f['crc_burst']}"
        if "retries" in f:
            s += f"  连续重试={f['retries']}"
        if "tout" in f:
            s += f"  累计超时={f['tout']}"
        return s
    if kind in ("stat", "perf", "devs", "dev"):
        return f"{kind.upper()}  " + " ".join(f"{k}={v}" for k, v in f.items())
    if kind == "autobaud":
        return (f"自动波特率  锁定 bitdiv={f['bitdiv']}"
                f"（{fmt_us(f['bitdiv'], fclk)}/位）" if "bitdiv" in f
                else "自动波特率  检测到误配，开始候选扫描")
    if kind == "log":
        if "kind" in f:
            s = f"日志 #{f['seq']}  {f['kind']}"
            if "addr" in f:
                s += f" addr={f['addr']}"
            return (s + f"  ts={f['ts']}cyc({fmt_us(f['ts'], fclk)})"
                    f" aux={f['aux']}")
        return f"日志概要  存活={f.get('n')}  覆盖丢弃={f.get('drop')}"
    if kind in ("ok", "err", "cfg", "help", "banner", "result"):
        return f.get("text", "") if kind != "result" else \
            ("RESULT: PASS" if f["pass"] else "RESULT: FAIL")
    return f.get("text", "???")


def run_stream(lines, fclk: float, quiet: bool = False) -> dict:
    """逐行解析显示，返回行类型计数（selftest 与串口循环共用）"""
    counts: dict[str, int] = {}
    for line in lines:
        kind, f = parse_line(line)
        counts[kind] = counts.get(kind, 0) + 1
        if not quiet and line.strip():
            ts = time.strftime("%H:%M:%S")
            print(f"[{ts}] {show(kind, f, fclk)}")
    return counts


def selftest() -> None:
    """实解析全部 demo golden：每行都必须被识别（无 other），关键场景
    的结构化字段逐一断言"""
    fails = 0

    def check(cond, msg):
        nonlocal fails
        if not cond:
            fails += 1
            print(f"  FAIL: {msg}")

    for gf in sorted(GOLDEN_DIR.glob("*.txt")):
        counts = run_stream(gf.read_text(encoding="utf-8").splitlines(),
                            50e6, quiet=True)
        check("other" not in counts,
              f"{gf.name}: 有未识别行 {counts.get('other')}")

    # normal：一问一答，resp=1728，统计 frames=2，画像一台设备，RESULT PASS
    lines = (GOLDEN_DIR / "normal.txt").read_text(encoding="utf-8").splitlines()
    kinds = [parse_line(l)[0] for l in lines if l.strip()]
    check(kinds == ["banner", "req", "rsp", "stat", "devs", "dev", "result"],
          f"normal 行序列 {kinds}")
    f = dict([parse_line(l)[1] for l in lines if l.startswith("RSP")][0])
    check(f.get("resp") == "1728", f"normal resp={f}")
    f = dict([parse_line(l)[1] for l in lines if l.startswith("STAT")][0])
    check(f.get("frames") == "2" and f.get("req") == "1"
          and f.get("rsp") == "1", f"normal STAT={f}")
    # bad_crc：坏 CRC 帧 + 超时（不当成有效回复），画像计入超时
    kinds = [parse_line(l)[0]
             for l in (GOLDEN_DIR / "bad_crc.txt").read_text(encoding="utf-8").splitlines()
             if l.strip()]
    check(kinds == ["banner", "req", "frame", "timeout", "stat", "devs",
                    "dev", "result"],
          f"bad_crc 行序列 {kinds}")
    # offline：离线→恢复→再离线的诊断序列（跨帧模式诊断）
    diags = [parse_line(l)[1]["kind"]
             for l in (GOLDEN_DIR / "offline.txt").read_text(encoding="utf-8").splitlines()
             if l.startswith("DIAG")]
    check(diags == ["offline", "recover", "offline"],
          f"offline DIAG 序列 {diags}")
    # app（命令接口）：timeout 查设、OK/ERR、help
    lines = (GOLDEN_DIR / "app.txt").read_text(encoding="utf-8").splitlines()
    kinds = [parse_line(l)[0] for l in lines if l.strip()]
    check("cfg" in kinds and "ok" in kinds and "err" in kinds
          and "help" in kinds, f"app 行序列 {kinds}")
    cfgs = [parse_line(l)[1] for l in lines if l.startswith("timeout=")]
    check([c["timeout"] for c in cfgs] == ["5632", "7000"],
          f"app timeout 查询序列 {cfgs}")
    # baudscan（自动波特率）：误配触发扫描 → 锁定 52（demo 固件缩小版
    # 候选表内速率），锁定后恢复正常问答
    lines = (GOLDEN_DIR / "baudscan.txt").read_text(encoding="utf-8").splitlines()
    autos = [parse_line(l)[1] for l in lines if l.startswith("AUTOBAUD")]
    check(len(autos) == 2 and autos[1].get("bitdiv") == "52",
          f"baudscan AUTOBAUD 序列 {autos}")
    # app_fault（事件日志/告警命令）：offline 场景的 5 条事件导出 +
    # alarm 查询 1→0（alarm off 生效）+ logclear 后空日志
    lines = (GOLDEN_DIR / "app_fault.txt").read_text(encoding="utf-8").splitlines()
    logs = [parse_line(l)[1] for l in lines if l.startswith("LOG #")]
    check([f["kind"] for f in logs] ==
          ["timeout", "offline", "recover", "timeout", "offline"],
          f"app_fault 日志事件序列 {logs}")
    alarms = [parse_line(l)[1]["alarm"] for l in lines if l.startswith("alarm=")]
    check(alarms == ["1", "0"], f"app_fault alarm 查询序列 {alarms}")

    if fails == 0:
        print("display selftest: PASS（全部 golden 行识别 + 关键场景断言通过）")
    else:
        print(f"display selftest: FAIL（{fails} 项）")
        sys.exit(1)


def main() -> None:
    ap = argparse.ArgumentParser(description="分析仪结果串口显示")
    ap.add_argument("--port", default="/dev/ttyUSB2")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--fclk", type=float, default=50e6,
                    help="板上时钟频率（周期数→时间换算，按实际工程设置）")
    ap.add_argument("--file", help="解析已有日志文件而非串口")
    ap.add_argument("--selftest", action="store_true")
    args = ap.parse_args()

    if args.selftest:
        selftest()
        return
    if args.file:
        run_stream(open(args.file, encoding="utf-8"), args.fclk)
        return

    import serial    # 延迟导入：selftest/--file 不依赖 pyserial

    ser = serial.Serial(args.port, args.baud, timeout=0.5)
    print(f"display: {args.port} @ {args.baud}，fclk={args.fclk:.3g} Hz；"
          f"Ctrl-C 退出")
    try:
        while True:
            line = ser.readline()
            if line:
                run_stream([line.decode("utf-8", errors="replace")],
                           args.fclk)
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    sys.exit(main())
