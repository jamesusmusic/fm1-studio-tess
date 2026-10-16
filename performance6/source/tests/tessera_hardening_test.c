/* SPDX-License-Identifier: GPL-3.0-only */
/* Regression tests for the studio dual-core stability update. The actual
 * CPU1 mailbox/ownership protocol is covered by dual_core_test/audio_worker_test.
 * Here a pending audio step is delivered when the UI edit unmasks interrupts. */
#include <stdint.h>
#include <limits.h>
static void test_irq_off(void), test_irq_on(void);
#define TESS_IRQ_OFF() test_irq_off()
#define TESS_IRQ_ON() test_irq_on()
#define main hostsim_main
#include "hostsim.c"
#undef main

static unsigned locked, enters, exits, irq_errors, step_on_unlock, irq_steps;
static int failures;
static uint32_t test_random = 0x7A55123Bu;
static uint32_t draw(void)
{
    test_random ^= test_random << 13;
    test_random ^= test_random >> 17;
    return test_random ^= test_random << 5;
}
static void test_irq_off(void)
{
    if (locked) irq_errors++;
    locked = 1;
    enters++;
}
static void test_irq_on(void)
{
    uint8_t packed[TESSERA_PK_BYTES];
    if (!locked) irq_errors++;
    tessera_pack(packed);
    if (!tessera_valid(packed)) irq_errors++;
    locked = 0;
    exits++;
    if (step_on_unlock && song.playing) {
        for (uint32_t tr = 0; tr < TESSERA_TRACKS; tr++)
            if (tessera.mode[tr]) tessera_clock_step(&trk[tr]);
        irq_steps++;
    }
}
static void check(const char *what, int ok)
{
    printf("tessera hardening: %s %s\n", what, ok ? "ok" : "FAIL");
    failures += !ok;
}
static void reset(void)
{
    song.playing = 0;
    tessera_stop_all();
    host_tracks_init();
    tessera_reset();
    song.sel = 0;
}

static void edit_stress(void)
{
    void (*edit[4])(uint32_t, int32_t) = {
        tessera_knob, tessera_evolve_knob, tessera_form_knob, tessera_tweak_knob
    };
    int32_t delta[] = {INT_MIN, INT_MAX, -128, 128, -1, 1, -7, 7, 0};
    reset();
    for (uint32_t tr = 0; tr < TESSERA_TRACKS; tr++) {
        song.sel = (uint8_t)tr;
        tessera_knob(0, (int32_t)tr + 1);
    }
    seq_start();
    step_on_unlock = 1;
    unsigned before = enters;
    for (uint32_t n = 0; n < 200000u; n++) {
        song.sel = (uint8_t)(draw() % (TESSERA_TRACKS + 2u));
        edit[draw() % 4u](draw() % 6u, delta[draw() % (sizeof delta / sizeof delta[0])]);
    }
    step_on_unlock = 0;
    check("200000 UI edits with pending playback steps, extreme deltas and invalid tracks/knobs",
          enters > before && enters == exits && !locked && !irq_errors && irq_steps > 10000u);
    seq_stop();
    int clear = !song.playing;
    for (uint32_t tr = 0; tr < TESSERA_TRACKS; tr++) {
        clear &= !tessera.active_gate[tr] && !trk[tr].seq_n;
        for (uint32_t v = 0; v < NVOICE; v++) {
            if (trk[tr].v[v].gate)
                printf("held after stop: track %u voice %u note %u mode %u\n", tr, v, trk[tr].v[v].note, tessera.mode[tr]);
            clear &= !trk[tr].v[v].gate;
        }
    }
    check("transport stop releases every generator note after edit stress", clear);
}

static void mutation_tests(void)
{
    reset();
    int ok = 1;
    for (uint32_t len = 2; len <= 16; len++) {
        uint16_t mask = (uint16_t)(0xFFFFu >> (16u - len));
        tessera.mode[0] = TESS_ENG_TURING;
        tessera.turing[0].len = (uint8_t)len;
        for (uint32_t amount = 1; amount <= 100; amount += 9) {
            tessera.evolve[0].amount = (uint8_t)amount;
            for (uint32_t all = 0; all < 2; all++) {
                tessera.turing[0].reg = all ? mask : 0;
                for (uint32_t n = 0; n < 200; n++) {
                    tessera_mutate(0);
                    uint32_t bits = tessera.turing[0].reg & mask;
                    ok &= bits != 0 && bits != mask;
                }
            }
        }
    }
    check("Turing mutation avoids all-zero/all-one loops for LEN 2..16 and AMOUNT 1..100", ok);
    ok = 1;
    tessera.mode[0] = TESS_ENG_EUCLID;
    tessera.evolve[0].amount = 100;
    for (uint32_t steps = 1; steps <= 32; steps++) {
        tessera.euclid[0].steps = (uint8_t)steps;
        tessera.euclid[0].pulses = 1;
        tessera.euclid[0].rot = 0;
        for (uint32_t n = 0; n < 2000; n++) {
            tessera_mutate(0);
            ok &= tessera.euclid[0].pulses >= 1 &&
                  tessera.euclid[0].pulses <= (steps > 1 ? steps - 1 : 1) && tessera.euclid[0].rot < steps;
        }
    }
    check("Euclid evolution stays within pulse/rotation bounds for STEPS 1..32", ok);
    ok = 1;
    tessera.mode[0] = TESS_ENG_MARKOV;
    for (uint32_t n = 0; n < 10000u; n++) {
        tessera_mutate(0);
        ok &= tessera.markov[0].tension <= 100;
    }
    check("Markov mutation reflects at the pitch-leap limits", ok);
    ok = 1;
    for (uint32_t mode = 1; mode < TESS_ENG_COUNT; mode++) {
        tessera.mode[0] = (uint8_t)mode;
        tessera.evolve[0].amount = 0;
        tessera_state_t before = tessera;
        uint32_t random_before = rng_state;
        tessera_mutate(0);
        ok &= !memcmp(&before, &tessera, sizeof before) && rng_state == random_before;
    }
    check("AMOUNT 0 preserves all state and random sequence for every automatic mutation", ok);
    ok = 1;
    for (uint32_t old = 0; old < 256; old++)
        for (uint32_t n = 0; n < 20; n++) {
            uint8_t next = tess_new_seed((uint8_t)old);
            ok &= next && next != old;
        }
    check("Euclid ROLL uses one bounded random draw and always changes seed", ok);
}

static void note_zero_test(void)
{
    reset();
    tessera_knob(0, TESS_ENG_TURING);
    trk_note_on(&trk[0], 0, 100);
    trk[0].seq_notes[0] = 0;
    trk[0].seq_n = 1;
    tessera.last_note[0] = 0;
    tessera.active_gate[0] = 1;
    tessera_knob(0, 1);
    int released = !tessera.active_gate[0] && !trk[0].seq_n;
    for (uint32_t v = 0; v < NVOICE; v++) released &= !trk[0].v[v].gate;
    check("switching generator modes releases MIDI note 0 instead of treating it as an idle arp owner", released);
}

static void section_tests(void)
{
    reset();
    tessera.evolve[0].sec = 1;
    tessera.sec[0][1].p0 = 255;
    tessera.sec[0][1].p1 = 255;
    tessera.mode[0] = TESS_ENG_EUCLID;
    tessera.euclid[0].steps = 3;
    tess_load_sec(0);
    check("FORM Euclid clamps remembered pulses/rotation to a shortened STEPS",
          tessera.euclid[0].pulses == 3 && tessera.euclid[0].rot < 3);
    tessera.mode[0] = TESS_ENG_MARKOV;
    tess_load_sec(0);
    check("FORM Markov bounds stale section values before probability calculation", tessera.markov[0].tension == 100);
    tessera.mode[0] = TESS_ENG_MORPH;
    tess_load_sec(0);
    check("FORM Morph bounds stale section blend positions", tessera.morph[0].morph_pos == 127);
}

static void validation_tests(void)
{
    reset();
    uint8_t packed[TESSERA_PK_BYTES], corrupt[TESSERA_PK_BYTES];
    tessera_pack(packed);
    tessera.evolve[0].step_cnt = 73;
    tessera.evolve[0].valid = 7;
    tessera.sec[0][2].note[5] = 99;
    tessera_state_t before = tessera;
    int ok = tessera_valid(packed) && !tessera_valid(NULL);
    for (uint32_t pos = 0; pos < sizeof packed; pos++) {
        for (uint32_t value = 0; value < 256; value++) {
            memcpy(corrupt, packed, sizeof corrupt);
            corrupt[pos] = (uint8_t)value;
            if (!tessera_valid(corrupt)) ok &= !tessera_unpack(corrupt);
            ok &= !memcmp(&before, &tessera, sizeof before);
        }
    }
    check("46080 record-byte mutations validate without touching any live state; invalid loads are atomic", ok);
    memcpy(corrupt, packed, sizeof corrupt);
    memset(corrupt + 33, 255, 12); /* legacy section bytes may belong to another generator */
    check("version-1 section bytes remain readable without changing the save format", tessera_unpack(corrupt));
}

int main(void)
{
    edit_stress();
    mutation_tests();
    note_zero_test();
    section_tests();
    validation_tests();
    return failures != 0;
}
