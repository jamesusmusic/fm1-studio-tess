# SPDX-License-Identifier: GPL-3.0-only
# Offline bridge routing contracts — By: Inputsignwave.
import importlib.util
import sys
import types
import unittest
from pathlib import Path

# No MIDI backend/connected hardware required for these contracts.
sys.modules.setdefault('mido', types.SimpleNamespace(Message=lambda kind, **kw: (kind, kw)))
spec = importlib.util.spec_from_file_location('bridge', Path(__file__).resolve().parents[1]/'tools/seaboard_usb_bridge.py')
bridge = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bridge)

class BridgeTests(unittest.TestCase):
    def test_exact_and_unique(self):
        self.assertEqual(bridge.choose(['RISE 25'], None, ('rise',), 'input'), 'RISE 25')
        self.assertEqual(bridge.choose(['MIDI A', 'MIDI B'], 'MIDI B', (), 'output'), 'MIDI B')
        for requested in (None, 'MIDI'):
            with self.assertRaises(ValueError):
                bridge.choose(['MIDI A', 'MIDI B'], requested, ('midi',), 'output')
    def test_channel_bytes_preserved(self):
        sent=[];output=types.SimpleNamespace(send=sent.append)
        for ch in range(16):
            for kind in ('note_on','note_off','pitchwheel','aftertouch','control_change'):
                msg=types.SimpleNamespace(type=kind,channel=ch,bytes=lambda: [0x91,60,100])
                bridge.forward(msg,output)
                self.assertIs(sent[-1],msg)
        for kind in ('sysex','active_sensing','reset'):
            n=len(sent);bridge.forward(types.SimpleNamespace(type=kind),output);self.assertEqual(n,len(sent))
    def test_shutdown(self):
        sent=[];bridge.silence(types.SimpleNamespace(send=sent.append))
        self.assertEqual([m[1]['control'] for m in sent],[64,120])
        self.assertTrue(all(m[1]['channel']==0 for m in sent))

if __name__=='__main__': unittest.main()
