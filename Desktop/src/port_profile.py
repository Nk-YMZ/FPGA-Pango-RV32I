"""串口角色识别与身份保存。枚举 USB 标识、设备序列号及接口位置，将当前端口对应到角色。
已运行角色保持锁定，空闲角色按唯一身份和同类设备数量匹配；本模块不打开串口。
"""
import json
import re
from pathlib import Path

ROLES = ('board', 'device', 'controller')
BOARD_USB = (0x10c4, 0xea60)
BUS_USB = (0x1a86, 0x7523)


def describe(port):
    return {key: getattr(port, key, None) for key in
            ('device', 'vid', 'pid', 'serial_number', 'location')}


def usb_id(port):
    return port.get('vid'), port.get('pid')


# 按数字片段排序 COM 名称，COM9 排在 COM20 前，排序不按纯字符串字典序。
def port_order(port):
    return [int(s) if s.isdigit() else s for s in re.split(r'(\d+)', port['device'])]


class PortProfile:
    def __init__(self, path):
        self.path = Path(path)
        self.roles = {}
        self.error = None
        try:
            data = json.loads(self.path.read_text(encoding='utf-8'))
            self.roles = {role: item for role, item in data.get('roles', {}).items()
                          if role in ROLES and isinstance(item, dict)}
        except (OSError, ValueError, AttributeError):
            pass

    def resolve(self, ports, current, locked=()):
        """有序列号先匹配序列号，其次USB位置；未记录的同类双适配器可互换角色。"""
        ports = sorted((describe(p) for p in ports), key=port_order)
        result = {role: current[role] for role in locked}
        used = set(result.values())
        for role in ROLES:
            if role in result:
                continue
            saved = self.roles.get(role, {})
            # 匹配范围先限定为相同 VID/PID 家族，再检查唯一序列号、USB 接口位置和当前端口。
            family = usb_id(saved)
            candidates = [p for p in ports if p['device'] not in used and usb_id(p) == family]
            match = None
            # 重复序列号不能当成唯一身份，继续检查物理接口位置。
            for key in ('serial_number', 'location'):
                matches = [p for p in candidates if saved.get(key) and p.get(key) == saved[key]]
                if len(matches) == 1:
                    match = matches[0]
                    break
            if match is None:
                expected = family if saved else BOARD_USB if role == 'board' else BUS_USB
                matches = [p for p in ports if p['device'] == current.get(role) and p['device'] not in used
                           and usb_id(p) == expected]
                if len(matches) == 1:
                    match = matches[0]
            if match is None and role == 'board':
                matches = [p for p in ports if p['device'] not in used and usb_id(p) == (family if saved else BOARD_USB)]
                if len(matches) == 1:
                    match = matches[0]
            if match:
                result[role] = match['device']
                used.add(match['device'])
        # 只有候选数量与缺失角色数量相等时才分配剩余适配器；存在多余同类设备时保留待选状态。
        missing = [role for role in ('device', 'controller') if role not in result]
        families = {BUS_USB} | {usb_id(self.roles[role]) for role in ('device', 'controller')
                                if role in self.roles and self.roles[role].get('vid') is not None}
        candidates = [p for p in ports if p['device'] not in used and usb_id(p) in families]
        # 多出同类设备时不随便占用；两路RS485并在同一总线，可确定排序后分配角色。
        if len(candidates) == len(missing):
            for role, port in zip(missing, candidates):
                result[role] = port['device']
        return {role: result.get(role, '') for role in ROLES}

    # 保存已选择设备的身份信息，先写临时文件再替换配置文件；写入失败保留错误状态。
    def remember(self, selected, ports):
        by_name = {p.device: describe(p) for p in ports}
        roles = dict(self.roles)
        for role, name in selected.items():
            if name in by_name:
                roles[role] = by_name[name]
        if roles == self.roles:
            return
        try:
            self.path.parent.mkdir(parents=True, exist_ok=True)
            temporary = self.path.with_suffix('.tmp')
            temporary.write_text(json.dumps({'version': 1, 'roles': roles}, ensure_ascii=False, indent=2), encoding='utf-8')
            temporary.replace(self.path)
            self.roles = roles
            self.error = None
        except OSError as exc:
            self.error = f'端口设置未能保存：{exc}'
