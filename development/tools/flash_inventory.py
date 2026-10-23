#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Linked symbol inventory for flash recovery — By: Inputsignwave.
"""Rank file-backed allocated ELF32 symbols; symbol sizes are not promised savings.

Optional --compare verifies byte arrays retained from a baseline. Pointer-bearing
objects/code can relocate, so are deliberately excluded from byte comparisons.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path


def symbols(path, contents=None):
    data = Path(path).read_bytes()
    if data[:6] != b'\x7fELF\x01\x01':
        raise ValueError('expected little-endian ELF32')
    header = struct.unpack_from('<16sHHIIIIIHHHHHH', data)
    sections = [struct.unpack_from('<IIIIIIIIII', data, header[6] + i * header[11])
                for i in range(header[12])]
    result = {}
    for section in sections:
        if section[1] != 2:
            continue
        strings = sections[section[6]]
        names = data[strings[4]:strings[4] + strings[5]]
        for offset in range(section[4], section[4] + section[5], section[9]):
            name, address, size, info, _, index = struct.unpack_from('<IIIBBH', data, offset)
            if not size or index >= len(sections):
                continue
            stored = sections[index]
            if stored[1] == 8 or not stored[2] & 2 or info & 15 not in (1, 2):
                continue
            name = names[name:names.index(0, name)].decode()
            begin = stored[4] + address - stored[3]
            if begin < stored[4] or begin + size > stored[4] + stored[5]:
                raise ValueError('symbol outside section: ' + name)
            result[name] = dict(size=size, kind='code' if info & 15 == 2 else 'data',
                                sha256=hashlib.sha256(data[begin:begin + size]).hexdigest())
            if contents is not None:
                contents[name] = data[begin:begin + size]
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('elf')
    parser.add_argument('--compare')
    parser.add_argument('--output')
    args = parser.parse_args()
    contents = {}
    current = symbols(args.elf, contents)
    report = dict(largest=[dict(name=name, **item) for name, item in
                           sorted(current.items(), key=lambda row: row[1]['size'], reverse=True)[:50]])
    if args.compare:
        previous = symbols(args.compare)
        compared = dict(current)
        if 'WT_FLASH' not in current and 'WT_HALF' in current:
            # Independent reconstruction from actual linked bytes, not the
            # checked-in raw test oracle or generator's intermediate arrays.
            packed = struct.unpack('<' + 'h' * (len(contents['WT_HALF']) // 2), contents['WT_HALF'])
            offsets = struct.unpack('<48H', contents['WT_ROW_AT'])
            decoded = bytearray()
            for offset in offsets:
                row = packed[offset:offset + 65]
                assert len(row) == 65
                row += tuple(-row[128 - i] for i in range(65, 128)) + (row[0],)
                decoded.extend(struct.pack('<129h', *row))
            compared['WT_FLASH'] = dict(size=len(decoded), kind='data', sha256=hashlib.sha256(decoded).hexdigest())
            report['original_wavetable_encoding'] = 'exact half rows, independently reconstructed from linked bytes'
        names = ['WT_FLASH', 'WL_FLASH', 'P5_FPACK', 'FM6_ADDED_DATA', 'FM6_FACTORY', 'FM6_ROM']
        names += [name for name in previous if name.startswith('AF_') and
                  name.endswith(('_DATA', '_G', '_HC', '_KM', '_KL', '_KR', '_EX'))]
        report['retained_arrays_identical'] = {name: compared.get(name) == previous[name] for name in names}
        report['retired_cz_editor_table_absent'] = 'CZ_PD' not in current
        if not all(report['retained_arrays_identical'].values()):
            raise SystemExit('retained asset changed: ' + json.dumps(report['retained_arrays_identical']))
    output = json.dumps(report, indent=2) + '\n'
    if args.output:
        Path(args.output).write_text(output)
    else:
        print(output, end='')


if __name__ == '__main__':
    main()
