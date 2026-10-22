/* SPDX-License-Identifier: MIT
 * Compact plate adaptation — By: Inputsignwave.
 * Topology adapted from Rings reverb, Copyright 2014 Emilie Gillet.
 * Retain LICENSE-Mutable.txt when distributing this module.
 * Audition candidate: not yet connected to production FX or project storage. */
#ifndef INPUTSIGNWAVE_PLATE_H
#define INPUTSIGNWAVE_PLATE_H
#include <stdint.h>
#include <stddef.h>
#ifndef PLATE_RATE
#define PLATE_RATE 22050
#endif
#if PLATE_RATE != 22050 && PLATE_RATE != 11025
#error Supported audition rates are 22050 and 11025
#endif
#define PLATE_SCALE(n) (((n)*PLATE_RATE+24000)/48000)
#define PLATE_PRE (PLATE_RATE/10+1)
#define PLATE_SAMPLES (PLATE_SCALE(150)+PLATE_SCALE(214)+PLATE_SCALE(319)+PLATE_SCALE(527)+PLATE_SCALE(2182)+PLATE_SCALE(2690)+PLATE_SCALE(4501)+PLATE_SCALE(2525)+PLATE_SCALE(2197)+PLATE_SCALE(6312)+PLATE_PRE)
typedef struct {
    int32_t feedback, damping, tone, lowcut, highcut, width;
    int32_t depth; /* modulation depth in Q8 delay samples */
    int32_t predelay; /* delay samples */
    uint32_t increment; /* free Hz or transport-derived phase increment */
} plate_params;
typedef struct {
    int16_t *memory; /* caller-owned; never allocated in processing */
    uint16_t base[11], length[11], pos[11];
    plate_params current, target;
    int32_t damp[2], bass[2], treble[2], shelf[2], dc[2];
    uint32_t phase[2], saturations;
    int ready;
} plate_state;
/* init checks capacity, clears complete state and delay history. */
int plate_init(plate_state *, int16_t *, size_t);
void plate_set(plate_state *, const plate_params *);
/* Wet-only API: caller retains exact dry and handles resampling/send/mix.
 * mono input and stereo output are signed PCM16 units in int32 containers.
 * Uninitialised/failed state returns wet silence. One audio owner per state. */
void plate_tick(plate_state *, int32_t, int32_t *, int32_t *);
#endif
