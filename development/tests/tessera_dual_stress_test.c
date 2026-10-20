/* SPDX-License-Identifier: GPL-3.0-only */
/* The real generators, Prophet/FM6 and FX through a concurrent host CPU1
 * worker. UI edits occur between joined audio blocks, as on target. Compare
 * every sample to the same dual-core configuration with serial dispatch. */
#define MELODEE_DUAL_CORE_TEST_NO_MAIN
#include "dual_core_test.c"
#define STRESS_BLOCKS 16384u
static uint32_t midi_reference[4096], midi_time[4096], midi_count, midi_cursor;
static void drain_midi(int parallel, uint32_t block)
{
    while (mo_r != mo_w) {
        uint32_t pkt = midi_out_q[mo_r++ % MQ];
        if (!parallel) {
            assert(midi_count < 4096);
            midi_reference[midi_count] = pkt; midi_time[midi_count++] = block;
        } else {
            assert(midi_cursor < midi_count && pkt == midi_reference[midi_cursor] && block == midi_time[midi_cursor]);
            midi_cursor++;
        }
    }
}

static void run(int parallel, int32_t *pcm, uint8_t *saved)
{
    seq_stop();
    setup_case(7, 0, 0);
    recording_reset();
    tessera_reset();
    usb.config = 1;
    mo_r = mo_w;
    memset(fx_state, 0, sizeof fx_state);
    fx_bus_mix = fx_bus_target = 0;
    song.g[G_BPM] = 240;
    song.g[G_MFXTYPE] = FX_REVERB;
    song.g[G_MFX2TYPE] = song.g[G_MFX3TYPE] = FX_OFF;
    audio_worker_online = 1;
    serial_render = !parallel;
    uint32_t seed = 0x61524419u;
    for (uint32_t tr = 0; tr < TESSERA_TRACKS; tr++) {
        host_preset(&trk[tr], tr & 1u ? ENGI_FM6 : ENGI_PROPHET, 0);
        trk[tr].p[P_SDIV] = 2;
        trk[tr].p[P_FXTYPE] = tr == 0 ? FX_ECHO : tr == 1 ? FX_CHORUS : tr == 2 ? FX_TAPE : FX_ROOM;
        trk[tr].p[P_FXMIX] = 40;
        trk[tr].p[P_FXSEND] = 64;
        song.sel = (uint8_t)tr;
        tessera_knob(0, (int32_t)tr + 1);
        tessera_form_knob(0, 2);
        tessera_evolve_knob(1, -128);
        tessera_evolve_knob(1, 1);
        tessera_evolve_knob(2, 20);
        harm_track[tr] = (harm_track_t){HARM_KEY,36,84,12,60,80,1,(uint8_t)tr};
#ifdef STUDIO_PERFORMANCE_STRESS
        performance[tr]=(performance_t){tr&1u?DUR_HIT:DUR_FIXED,3,12,1,1,10,20,100,15,48,72,82,18};
#endif
        rec_set(&trk[tr], 1);
    }
    song.sel = 1;
    tessera_tweak_knob(3, 1); /* Euclid follows Turing */
    seq_start();
    void (*edit[4])(uint32_t, int32_t) = {
        tessera_knob, tessera_evolve_knob, tessera_form_knob, tessera_tweak_knob
    };
    int32_t out[2 * CTL];
    uint32_t jobs = audio_worker.jobs;
    int64_t energy = 0;
    for (uint32_t block = 0; block < STRESS_BLOCKS; block++) {
        if (!(block % 32u)) {
            seed = seed * 1664525u + 1013904223u;
            song.sel = (uint8_t)((seed >> 16) & 3u);
            /* Keep all four modes active while editing their parameters. */
            edit[(seed >> 20) & 3u](1u + ((seed >> 22) % 3u), seed & 0x100u ? 1 : -1);
        }
#ifdef STUDIO_PERFORMANCE_STRESS
        if(block==STRESS_BLOCKS-2048u)seq_stop();
        if(!(block%97u)){
            performance[1].brightness=(uint8_t)(40+(block%49));
            performance[3].contour=(uint8_t)(48+(block%33));
        }
#endif
        mix_block(out, CTL);
        drain_midi(parallel, block);
        assert(!p5_pending && !fm6_pending && !fx_bus_pending);
        assert(AW_LOAD(&audio_worker.request) == AW_LOAD(&audio_worker.complete));
        if (parallel) assert(!memcmp(pcm + block * 2u * CTL, out, sizeof out));
        else memcpy(pcm + block * 2u * CTL, out, sizeof out);
        for (uint32_t n = 0; n < 2u * CTL; n++) energy += llabs(out[n]);
    }
    uint8_t state[TESS_FULL_BYTES];
    tess_full_pack(state);
    if (parallel) assert(!memcmp(saved, state, sizeof state));
    else memcpy(saved, state, sizeof state);
    assert(energy > 0);
    if (parallel) assert(audio_worker.jobs > jobs);
    seq_stop();
    drain_midi(parallel, STRESS_BLOCKS);
    for (uint32_t tr = 0; tr < TESSERA_TRACKS; tr++) {
        assert(!tessera.active_gate[tr] && !trk[tr].seq_n && !(duration_active & (1u<<tr)));
        for (uint32_t v = 0; v < NVOICE; v++) assert(!trk[tr].v[v].gate);
    }
    serial_render = 0;
}

int main(void)
{
    pthread_t thread;
    assert(!pthread_create(&thread, 0, worker_thread, 0));
    while (!AW_LOAD(&audio_worker.ready)) sched_yield();
    int32_t *pcm = calloc(STRESS_BLOCKS * 2u * CTL, sizeof *pcm);
    uint8_t saved[TESS_FULL_BYTES];
    assert(pcm);
    run(0, pcm, saved);
    run(1, pcm, saved);
    assert(midi_count > 0 && midi_cursor == midi_count);
    free(pcm);
    assert(!pthread_cancel(thread));
    assert(!pthread_join(thread, 0));
    printf("tessera dual: %u blocks, four harmony-enabled generators, two Prophet/two FM6 tracks, FX, one-pass REC and 512 live edits: serial/concurrent PCM, %u MIDI packets/timestamps and full settings identical; all notes released\n", STRESS_BLOCKS, midi_count);
    return 0;
}
