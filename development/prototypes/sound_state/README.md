# Sound-state extension envelope prototype

By: Inputsignwave

23 October 2026. A codec experiment, **not a production storage migration**.
No flash addresses, object IDs or FBKT/FBK7 readers/writers are changed.

Measured current layout: 122 track parameters (wire permits fewer than 128),
27,904 bytes of bank payload and 36 unused tail bytes. The project reserved gap
before Tessera is zero. Two WT layers, their envelopes/filters and expanded FX
state cannot simply be appended to these records.

`codec.h` preserves every byte of an existing bank plus a separate extension.
It can reclaim space from repetitive payloads using bounded byte RLE; ISX1
stores explicit lengths and an independent CRC. Scratch encode returns failure
if it cannot fit. Decode validates structure and the full CRC in a first pass;
only then can it write outputs. Caller must additionally validate both the old
bank and the extension schema before any runtime change or A/B commit.

Tests pass under ASan/UBSan for zero, ascending, random, repeated-run and sparse
27,904-byte banks plus a 256-byte extension. Every source byte survives. Corrupt
and truncated inputs leave destinations untouched; oversized/incompressible
records fail safely. In particular the random payload needs 28,398 bytes and
**does not fit the existing allocation**. This prototype alone is therefore
not a universal storage solution and must not be shipped as one.

Next work is to select a versioned representation with guaranteed capacity, or
explicitly handle capacity failures without dropping any current step, recording,
retired payload or user preset. Evaluate better structured packing, measured
compression and scratch/stack ownership. Then integrate project capture/restore,
full bank, template, individual user presets, undo and editor transfer together.
Do not repurpose retired CZ data or change flash layout merely to make it fit.
