#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Studio Tess reproducible build dependencies — By: Inputsignwave.
"""Fetch only the three pinned vendor packaging files; verify before replacing."""
import hashlib
from pathlib import Path
import sys
import time
import urllib.request

COMMIT = "d179b4484759423312073f5fbb232501aa491047"
FILES = {
    "uboot.boot": "4e3b4c220dc96641cb5a723f41e68ce41d5261ae9434bb33fbd7f2c59976ded4",
    "cfg_tool.bin": "276579954f076886a6a7694f65dc71c034a63a2c204b76749065c0ac7b010d1b",
    "cfg/eq_cfg_hw.bin": "41167491bffed4651750719c973d2758adeb9021a5670d02d6a53c85ed80ea7d",
}

def fetch(destination):
    for rel, expected in FILES.items():
        target = destination / "cpu/wl82/tools" / rel
        if target.is_file() and hashlib.sha256(target.read_bytes()).hexdigest() == expected:
            continue
        url = f"https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK/raw/{COMMIT}/cpu/wl82/tools/{rel}"
        for attempt in range(4):
            try:
                request = urllib.request.Request(url, headers={"User-Agent": "Studio-Tess-build"})
                with urllib.request.urlopen(request, timeout=30) as response:
                    data = response.read(65537)
                if hashlib.sha256(data).hexdigest() != expected:
                    raise ValueError(f"pinned SDK checksum mismatch: {rel}")
                target.parent.mkdir(parents=True, exist_ok=True)
                temporary = target.with_name(target.name + ".download")
                temporary.write_bytes(data)
                temporary.replace(target)
                break
            except Exception:
                if attempt == 3:
                    raise
                time.sleep(2 ** attempt)
    print(f"SDK: three SHA-256 verified files from {COMMIT}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("usage: get_studio7_sdk.py DESTINATION")
    fetch(Path(sys.argv[1]))
