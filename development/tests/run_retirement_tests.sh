#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-only
# Production removal profile and retained DSP parity — By: Inputsignwave.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/removal
cc_task=${CC:-cc}
flags='-O1 -ffp-contract=off -w -Ibuild/gen -Ifirmware/src'
$cc_task $flags tests/retirement_test.c -lm -o build/removal/production
build/removal/production
$cc_task $flags -DMELODEE_RETIRED_REFERENCE=1 tests/retirement_test.c -lm -o build/removal/reference
build/removal/production --parity > build/removal/production-pcm.txt
build/removal/reference --parity > build/removal/reference-pcm.txt
diff -u build/removal/reference-pcm.txt build/removal/production-pcm.txt
printf 'production: retained preset/FX PCM comparisons passed: '
wc -l < build/removal/production-pcm.txt
$cc_task $flags tests/studio8_navigation_test.c -lm -o build/removal/navigation
build/removal/navigation
$cc_task $flags -g -fsanitize=address,undefined tests/retirement_test.c -lm -o build/removal/sanitize
UBSAN_OPTIONS=halt_on_error=1 ASAN_OPTIONS=detect_leaks=0 build/removal/sanitize
$cc_task $flags -pthread tests/audio_matrix_test.c -lm -o build/removal/audio_matrix
build/removal/audio_matrix
$cc_task $flags tests/fx_performance.c -lm -o build/removal/fx_performance
build/removal/fx_performance > build/removal/fx_performance.csv

# New engine stays out of historical reference goldens; validate its production paths explicitly.
python3 tools/gen_wavetable.py firmware/src/wavetable_bank.h --check
$cc_task $flags tests/wavetable_test.c -lm -o build/removal/wavetable
build/removal/wavetable build/removal
python3 tests/wavetable_spectral.py build/removal
$cc_task $flags -g -fsanitize=address,undefined tests/wavetable_test.c -lm -o build/removal/wavetable_sanitize
UBSAN_OPTIONS=halt_on_error=1 ASAN_OPTIONS=detect_leaks=0 build/removal/wavetable_sanitize
$cc_task $flags -DMELODEE_USB_AUDIO=1 -Itests tests/ui_render.c -lm -o build/removal/ui_render
mkdir -p build/wavetable-ui/ppm
build/removal/ui_render build/wavetable-ui
$cc_task $flags tests/wavetable_emulator_fixture.c -lm -o build/removal/emulator_fixture
build/removal/emulator_fixture build/removal/wavetable-fixture.nor

# Combined Performance 10 additions have their own signal/lifetime fixtures.
for unit in wavetable_layers fmkit sound_fx; do
    $cc_task $flags -g -fsanitize=address,undefined -fno-sanitize-recover=all tests/${unit}_test.c -lm -o build/removal/${unit}
    UBSAN_OPTIONS=halt_on_error=1 ASAN_OPTIONS=detect_leaks=0 build/removal/${unit}
done
$cc_task -O2 -g -fsanitize=address,undefined -fno-sanitize-recover=all -DPLATE_RATE=11025 tests/plate_test.c firmware/src/plate.c -o build/removal/plate
build/removal/plate
