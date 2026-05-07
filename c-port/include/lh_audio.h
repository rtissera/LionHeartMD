/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com) Pierre-Alexandre
 *
 * SDL audio init / shutdown / master mixer.
 *
 * Owns the single SDL audio device and the master mixing callback. The
 * callback pulls samples from the sfx voice bank (lh_sfx) and the music
 * source (lh_music), sums them, applies the master volume, and writes
 * the result into the device buffer.
 *
 * All buffers are interleaved 16-bit signed stereo at the configured
 * sample rate (default: 44100 Hz / 2 ch). The Java reference uses the
 * same format via lionengine-audio-wav and libsc68.
 */
#ifndef LH_AUDIO_H
#define LH_AUDIO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LH_AUDIO_DEFAULT_RATE     44100
#define LH_AUDIO_DEFAULT_CHANNELS 2

/* Open the audio device. Idempotent. Returns false on failure. */
bool lh_audio_init(int sample_rate, int channels);

/* Close the audio device and free resources. Idempotent. */
void lh_audio_shutdown(void);

/* Whether the device is currently open. */
bool lh_audio_is_running(void);

/* Master volume on a 0..100 scale. Applied during the mix-down. */
void lh_audio_set_master_volume(int v);
int  lh_audio_get_master_volume(void);

/* Configured device parameters (after lh_audio_init). */
int lh_audio_get_sample_rate(void);
int lh_audio_get_channels(void);

/*
 * Internal mixer hooks (called from the SDL audio thread).
 *
 * lh_sfx and lh_music register their own pull functions; the master
 * callback invokes them under the audio device lock and adds their
 * output into the per-frame mixing accumulator. This indirection lets
 * lh_sfx and lh_music live in their own translation units while
 * keeping the audio thread topology in one place.
 */
typedef void (*lh_audio_pull_fn)(int32_t* mix, int frames, int channels);

/* Register/unregister mix sources. NULL fn unregisters. Up to 4 sources. */
bool lh_audio_register_source(lh_audio_pull_fn fn);
bool lh_audio_unregister_source(lh_audio_pull_fn fn);

/* Lock/unlock the audio device. Use around shared state mutations that
 * the audio thread also reads (voice tables, music decoder, etc.). */
void lh_audio_lock(void);
void lh_audio_unlock(void);

#endif
