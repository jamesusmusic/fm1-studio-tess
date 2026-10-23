#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Channel-preserving RISE -> FM-1 live bridge — By: Inputsignwave.
"""List ports first; run with exact --input and --output names. Ctrl-C panics USB zone."""
import argparse
import time
import mido


def choose(names, requested, words, role):
    if requested in names:
        return requested
    matches = [n for n in names if (requested.casefold() in n.casefold() if requested else any(w in n.casefold() for w in words))]
    if len(matches) != 1:
        raise ValueError(f"Choose one {role} with --{role}; candidates: {matches or names}")
    return matches[0]


def forward(message, output):
    # No updater/editor SysEx or active-sensing/timeouts on this live-only path.
    # All channel voice messages and realtime clock/transport retain their bytes.
    if message.type not in ('sysex', 'active_sensing', 'reset'):
        output.send(message)


def silence(output):
    output.send(mido.Message('control_change', channel=0, control=64, value=0))
    output.send(mido.Message('control_change', channel=0, control=120, value=0))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--list', action='store_true')
    parser.add_argument('--input', help='Exact RISE MIDI input name (unique substring also accepted)')
    parser.add_argument('--output', help='Exact FM-1 MIDI output name (unique substring also accepted)')
    args = parser.parse_args()
    inputs, outputs = mido.get_input_names(), mido.get_output_names()
    if args.list:
        print('INPUTS:'); print('\n'.join(inputs) or '(none)')
        print('OUTPUTS:'); print('\n'.join(outputs) or '(none)')
        return
    try:
        src = choose(inputs, args.input, ('seaboard', 'rise'), 'input')
        dst = choose(outputs, args.output, ('melodee', 'inputsignwave', 'fm-1'), 'output')
        if src == dst or any(w in dst.casefold() for w in ('update', 'uboot', 'seaboard', 'rise')):
            raise ValueError('Output must be the running FM-1 MIDI port, not the controller or updater')
    except ValueError as error:
        parser.error(str(error))
    with mido.open_output(dst, autoreset=False) as output:
        print(f'{src} -> {dst}; channels unchanged. Ctrl-C to stop and release the USB zone.', flush=True)
        try:
            with mido.open_input(src, callback=lambda msg: forward(msg, output)):
                while True:
                    time.sleep(0.1)
        except KeyboardInterrupt:
            pass
        finally:
            silence(output)


if __name__ == '__main__':
    main()
