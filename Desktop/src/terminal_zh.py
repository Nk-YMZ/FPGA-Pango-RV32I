"""开发板 ASCII 报告的中文显示转换。保留地址、计数、阈值及原始证据含义。
周期值按传入的 fclk 换算时间，无法识别的报告保持原文返回。
"""
import re


# 字段从开发板报告提取；地址按十六进制解析，时间单位换算使用调用者提供的时钟频率。
def terminal_lines(line, fclk=80_000_000):
    # 先提取 key=value 字段，再按报告前缀选择中文模板；原数值仍保留其单位含义。
    # 来自诊断索引的地址关联证据使用专用模板，其他未知内容直接显示。
    fields=dict(re.findall(r'(\w+)=([^\s]+)',line))
    def n(key,default=0):
        match=re.match(r'\d+',str(fields.get(key,default)))
        return int(match[0]) if match else default
    def device():
        try:return f'设备{int(fields.get("addr","0"),16):02d}'
        except ValueError:return '该设备'
    if line.startswith('地址不匹配依据：'):
        addresses=re.findall(r'\baddr=([0-9a-fA-F]{2})',line)
        return [f'地址不符：请求{int(addresses[0],16):02d}，回应{int(addresses[-1],16):02d}。']
    if line.startswith('板端依据：'):
        address=re.search(r'\baddr=([0-9a-fA-F]{2})',line)
        name=f'地址{int(address[1],16):02d}' if address else '同一地址'
        times=re.findall(r'\b\d\d:\d\d:\d\d\b',line)
        hint=f'（{times[0]}→{times[-1]}）' if len(times)>1 else ''
        return [f'{name}迟到回应{hint}。']
    if line.startswith('TIMEOUT '):
        return [f'{device()}超时（{n("thresh")/fclk*1000:g}ms）。']
    if line.startswith('DIAG offline '):return [f'{device()}离线。']
    if line.startswith('DIAG recover '):return [f'{device()}恢复回应。']
    if line.startswith('DIAG link-interference '):return [f'开发板检测到连续校验错误，提示链路干扰（连续{n("crc_burst")}帧）。']
    if line.startswith('DIAG retry-storm '):return [f'{device()}相同请求连续重试{n("retries")}次，开发板提示请求过密。']
    if line.startswith('FRAME crc_err'):
        raw=fields.get('raw_addr')
        if raw:return [f'校验失败（帧头{int(raw,16):02d}未校验，{n("len")}字节）。']
        return ['校验失败（地址未知）。']
    if line.startswith('FRAME len_err'):return ['报文过短或长度不符。']
    if line.startswith('FRAME unsupported'):return [f'收到校验正确但暂不支持的功能码{fields.get("fc","?")}。']
    if line.startswith('EXR '):
        code=fields.get('code','?')
        meanings={'01':'不支持该功能','02':'寄存器地址无效','03':'参数无效','04':'设备执行失败','06':'设备忙'}
        return [f'{device()}异常应答：{meanings.get(code,"异常码"+code)}（{code}）。']
    if line.startswith('OK summary reset'):return ['已重新开始当前统计窗口。']
    if line.startswith('OK alarm=0'):return ['告警已解除。']
    if line.startswith(('OK report=compact','report=compact')):return ['当前为精简回应输出：保留地址、数量和响应时间，省略数据预览。']
    if line.startswith(('OK report=full','report=full')):return ['当前为原格式输出：正常回应包含数据预览。']
    if line.startswith('OK clear'):return ['已清除统计、设备记录和告警；事件日志仍保留。']
    if line.startswith('OK logclear'):return ['事件日志已清空。']
    if line.startswith(('OK timeout=','timeout=')):return [f'回应时限：{n("timeout")/fclk*1000:g}毫秒。']
    if line.startswith('alarm='):return ['告警已锁存，需手动解除。' if n('alarm') else '当前告警未锁存。']
    if line.startswith('bitdiv='):return [f'监听接收位周期：{n("bitdiv")}个时钟周期。']
    if line.startswith('STAT '):
        return [f'累计收到{n("frames")}帧：请求{n("req")}次、正常回应{n("rsp")}次、异常应答{n("exr")}次。',
                f'累计故障：校验{n("crc")}次、格式{n("len")}次、超时{n("tout")}次；采集硬件溢出{n("ovf")}次。']
    if line.startswith('PERF '):return [f'累计处理{n("bytes")}字节；采集软件溢出{n("ev_ovf")}次，显示丢行{n("tx_drop")}条。']
    if line.startswith('SUM '):return [f'近{n("ms")/1000:g}秒：每秒处理{n("Bps")/1000:.2f}KB、有效问答{n("pairs_s")}次；错误率{fields.get("err","未提供")}，超时{n("tout")}次，采集丢失{n("lost")}条，显示累计丢行{n("tx_drop")}条。']
    if line.startswith('DEVS '):return [f'已记录{n("n")}台设备；设备表溢出{n("ovf")}次。']
    if line.startswith('DEV addr='):return [f'{device()}：请求{n("req")}次，回应{n("rsp")}次，异常应答{n("exr")}次，超时{n("tout")}次；最近响应{n("rlast")/fclk*1000:.3f}毫秒。']
    if line.startswith('LOG n='):return [f'现存{n("n")}条事件，累计{n("total")}条，较早覆盖{n("drop")}条。']
    if line.startswith('LOG #'):
        labels={'crc_err':'校验错误','len_err':'格式错误','timeout':'请求超时','interference':'链路干扰',
                'retry_storm':'重复请求过密','offline':'设备离线','recover':'恢复回应'}
        event=next((zh for key,zh in labels.items() if re.search(r'\b'+key+r'\b',line)), '板端事件')
        number=re.search(r'#(\d+)',line)
        return [f'事件{number[1] if number else ""}：{event}；地址{fields.get("addr","未提供")}，记录值{fields.get("aux","未提供")}。']
    if line.startswith('AUTOBAUD scan start'):return ['开发板正在重新识别总线波特率。']
    if line.startswith('AUTOBAUD lock'):return [f'开发板已锁定接收位周期{n("bitdiv")}，继续监听。']
    if line.startswith('ERR unknown'):return ['开发板未识别该命令；可输入help查看支持的操作。']
    if line.startswith('ERR bad-arg'):return ['命令参数不正确。']
    if line.startswith('cmds:'):return ['常用查询：stat总计、dev设备、perf处理统计、log事件。',
                                         '常用操作：alarm off解除告警、clear清统计、summary reset重新计时。',
                                         '输出格式：report compact精简回应，report full恢复数据预览。']
    if line.startswith('> '):return ['已发送命令：'+line[2:]]
    if line.startswith('app-mon:'):return ['开发板总线监听程序已启动。']
    return [line]
