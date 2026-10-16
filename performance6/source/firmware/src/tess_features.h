/* SPDX-License-Identifier: GPL-3.0-only */
#pragma once
enum { HARM_OFF, HARM_KEY, HARM_SONG };
typedef struct { uint8_t mode, low, high, outside, bias, resolve, midi, channel; } harm_track_t;
typedef struct { uint8_t root, chord, key, scale, bars; } harm_row_t;
static harm_track_t harm_track[NTRK];
static harm_row_t harm_song[CHAIN_ROWS];
static uint8_t harm_edit_row;
#define TESS_FEATURE_BYTES (4u + 8u * NTRK + 5u * CHAIN_ROWS)
#define TESS_FULL_BYTES (TESSERA_PK_BYTES + TESS_FEATURE_BYTES)
#define TESS_OLD_OBJ_BYTES (4u + 4u * (4u + TESSERA_PK_BYTES))
static void features_init(void);
static void features_song_clear(void);
static int32_t features_pitch(track_t *t, int32_t proposed);
static void features_midi_on(track_t *t, uint32_t note, uint32_t velocity);
static void features_midi_off(track_t *t, uint32_t note);
static void features_midi_stop(uint32_t tr);
static void features_midi_poll(void);
static int features_capture_prepare(track_t *t);
static void features_capture_note(track_t *t, uint32_t note, uint32_t vel, uint32_t gate);
static void features_capture_end(track_t *t);
static void features_capture_guard(track_t *t);
static uint32_t features_chain_time;
static uint8_t features_notice;
static uint32_t features_pack(uint8_t *out);
static int features_valid(const uint8_t *in);
static void features_defaults(uint8_t *out);
static void features_apply(const uint8_t *in);
static uint32_t tess_full_pack(uint8_t *out);
static int tess_full_valid(const uint8_t *in, uint32_t size);
static int tess_full_unpack(const uint8_t *in, uint32_t size);

static void features_duration_stop(track_t *t);
static void features_duration_tick(track_t *t,uint32_t n);
static void features_duration_on(track_t *t,uint32_t note,uint32_t velocity);
static void features_capture_close(track_t *t);
