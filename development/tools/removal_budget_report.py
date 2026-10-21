#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Target removal-profile measurements — By: Inputsignwave.
"""Compare exact target artifacts; CPU values are static weights, not cycles."""
import argparse
import importlib.util
import json
import re
from pathlib import Path


def memory(folder):
    folder=Path(folder)
    data=(folder/'melodee.map').read_text()
    def symbol(name):
        m=re.search(r'0x([0-9a-f]+)\s+'+re.escape(name)+r'\b',data)
        if not m: raise ValueError('missing '+name)
        return int(m[1],16)
    banks={name or 'main':symbol('_resource_'+name+'end')-symbol('_resource_'+name+'start')
           for name in ('','data_','low_','retained_')}
    app=(folder/'melodee.bin').stat().st_size
    return {'app_bytes_including_stub':app,'app_body_bytes':app-3808,
            'app_body_limit':0x8c000,'app_slot_spare_bytes':0x8c000-(app-3808),
            'audio_banks_bytes':banks,'audio_banks_total_excluding_cache':sum(banks.values()),
            'main_audio_arena_minimum':49152,
            'ordinary_ram_consumed_through_bss_end':symbol('_resource_data_start')-0x1c08000}


def cpu(folder):
    path=Path(__file__).resolve().parents[1]/'tests/target_budget.py'
    spec=importlib.util.spec_from_file_location('target_budget',path)
    mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
    functions=mod.functions(Path(folder)/'melodee.dis')
    result={name:mod.cost(insns)['cost'] for name,insns in functions.items() if insns}
    for parent,children in mod.EXTRACTED.items():
        if any(child in result for child in children):
            result[parent]=result.get(parent,0)+sum(result.get(child,0) for child in children)
    return result


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('baseline');p.add_argument('candidate');p.add_argument('--output')
    args=p.parse_args()
    before=memory(args.baseline);after=memory(args.candidate)
    text=(Path(args.candidate)/'melodee.dis').read_text()
    labels=re.findall(r'^([A-Za-z_][A-Za-z_0-9.]*):$',text,re.M)
    forbidden=[n for n in labels if n.startswith(('sid_','cz_native_','dsk_render','dsk_hit','dsk_tick','fx_reverb'))]
    if forbidden: raise SystemExit('retired DSP still linked: '+', '.join(forbidden))
    a=cpu(args.baseline);b=cpu(args.candidate)
    result={'baseline':before,'candidate':after,
            'app_bytes_recovered':before['app_bytes_including_stub']-after['app_bytes_including_stub'],
            'audio_bank_bytes_recovered':after['audio_banks_total_excluding_cache']-before['audio_banks_total_excluding_cache'],
            'retired_dsp_absent':True,
            'static_target_cpu_weights':{n:{'before':a[n],'after':b[n]} for n in sorted(a.keys()&b.keys())},
            'limits':['App sizes include the unchanged 3808-byte boot stub; body limit is 0x8c000.',
                      'SRAM banks are disjoint; cache bank excluded. Do not treat total as one buffer.',
                      'Static CPU weights measure compiled control-flow cost, not hardware cycles or CPU percent.',
                      'Host allocation peaks cover tested scenarios; host pointer/layout differences remain.',
                      'No stack high-water, cache/bus contention or physical deadline measurement is claimed.']}
    out=json.dumps(result,indent=2)+'\n'
    if args.output:Path(args.output).write_text(out)
    print(out,end='')

if __name__=='__main__':main()
