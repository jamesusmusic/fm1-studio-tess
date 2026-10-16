# Install Studio Tess Performance 6

Main firmware: **v1.4.1-studio-tess.6**, dual core.

1. On the current firmware, save projects and export a **full backup**. Keep that
   file unchanged. New project saves cannot be read by Audio5/.4.
2. Extract this ZIP. Open Terminal in the extracted `FM1_Studio_Tess_Performance_6`
   folder. Connect the FM-1 using a USB data cable and stable USB power.
3. Close other MIDI/editor connections. Use your existing installer Python
   environment. On your Mac, its previous activation command was:

   ```bash
   source "$HOME/Documents/CUSTOM FM-1/FM1_Melodee_Tess_Integration/.venv/bin/activate"
   ```

4. Check the connected device, then install:

   ```bash
   python3 -u source/tools/fm1_install.py --info
   python3 -u source/tools/fm1_install.py firmware-candidates/studio-melodee-tess-1.4.1-dual-performance6.fwsc
   ```

   Answer `y` if asked. Keep USB connected through restart and let any first-start
   migration finish. Check About for **v1.4.1-studio-tess.6**.

5. Read `CONTROLS_AND_TESTING.md`. Old projects start with the new processing
   bypassed. For independent note lengths, use SEQ > NOTE LENGTH. For FM performance
   controls, use ENV > FM AMP / FM TONE.

The Python installer requires `mido` and `python-rtmidi` if they are not already
installed. The existing environment used for earlier updates should already have them.
Commands are relative to the extracted folder; no Downloads path is assumed.

## Rollback to Audio Hardening 5

Use your pre-install backup together with:

```bash
python3 -u source/tools/fm1_install.py rollback/studio-melodee-tess-1.4.1-dual-audio5.fwsc
```

Then restore the **pre-Performance-6 full backup** using the matching Audio5
backup/editor tools. Flashing Audio5 alone does not convert HMF2 projects back.
Its matching source is in `rollback/source-audio5.tar.gz`; extract that to obtain
the Audio5 source/tools and web editor. Audio5 crash symbols are also included.

## Single-core comparison only

Do not choose this for the main test. If comparing a reproducible audio fault:

```bash
python3 -u source/tools/fm1_install.py diagnostics/single-core/studio-melodee-tess-1.4.1-single-performance6-DIAGNOSTIC.fwsc
```

Its About version ends in `.6-single`. It uses the same project format and
controls, with the private diagnostic build switch. Return to the main dual-core
firmware for normal testing. Do not treat a single-core comparison as evidence
of the cause until the same patch/settings and fault have been compared.

## Hardware evidence if artifacts return

Before restarting, run:

```bash
python3 -u source/tools/usb_audio_stats.py --complete --seconds 30 --interval 1 > fm1-audio-report.jsonl
```

Play for around 15 seconds, then STOP and let the tail finish. Keep the report,
patch name, FX and performance settings. This candidate has software validation;
the intermittent physical FM6 Bells distortion and actual device CPU load remain
unverified on hardware.
