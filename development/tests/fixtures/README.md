# Studio upgrade fixtures

`studio-tess3.nor.gz` is the 1 MiB simulated NOR produced by source revision
`a3d9f1d1bd20a9357626c91136e73aba09003584` (Studio Tess .3). It holds four saved
projects, each with Turing/Euclid/Markov/Morph and EVOLVE/FORM configured by the
old upgrade harness. All tracks have non-default harmony, range and MIDI settings;
all 16 rows have explicit chords, keys, scales and bars. Track 1 OUTSIDE is 20,
21, 22 and 23 in slots A..D. `studio-tess3-state.bin` is slot D's 296-byte block.

The old, GPL-licensed harness and construction code are retained verbatim in
`studio3_fixture.c`. Compile it against that older checkout's generated headers
and test include directory, then run it from the older repository root:

```sh
cc -O1 -ffp-contract=off -w -Ibuild/gen -Ifirmware/src -Itests \
  /absolute/path/to/studio3_fixture.c -lm -o build/legacy_features_fixture
build/legacy_features_fixture
```

Outputs are `build/feature-validation/legacy3-features.nor` and
`build/feature-validation/legacy3-state.bin`. The checked-in NOR uses gzip with
mtime zero. No physical device image or user data is included.

`studio_upgrade_check.py` decompresses the fixture to a temporary directory and
runs `studio_storage_test`. That test models erase and page/header write failures,
checks the committed source copy, retries migration, verifies every saved block
and checks that a completed conversion does not write again.

## Performance 6 FBKT to Studio 7

`studio6-fbkt.bin` is a synthetic 27,904-byte bank written by the preserved
Performance 6 encoder, with all 32 banks populated with distinct four-note chords,
ties/rests, flags, hit/accent masks, gate bytes, probability and timing. It contains
no device/user data. SHA-256:
`0f822f241d9d6994b33d195cd72441602b2e26ffbec444c28141f07403f680f4`.

Construction source: `studio6_fixture.c`, compiled from development/ with its
include of the archived source and generated headers in build/gen.
`studio7_upgrade_test.c` imports it through old offsets, converts a separate
buffer, and compares every decoded pattern byte before/after conversion.
