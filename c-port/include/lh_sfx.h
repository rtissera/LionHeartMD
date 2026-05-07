/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com) Pierre-Alexandre
 *
 * Sound effects (WAV) loader + voice bank.
 *
 * Mirrors com.b3dgs.lionheart.Sfx + lionengine-audio-wav. Each unique
 * media path resolves to a cached PCM buffer (loaded with SDL_LoadWAV
 * and converted to the audio device's format). Playback is allocated
 * round-robin to one of LH_SFX_MAX_VOICES simultaneous voices.
 *
 * Thread model: lh_sfx_play / lh_sfx_load are called from the main
 * thread; the mixer pull (lh_sfx_pull) runs on the SDL audio thread.
 * State shared between the two is protected by lh_audio_lock().
 */
#ifndef LH_SFX_H
#define LH_SFX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LH_SFX_MAX_CACHED  32
#define LH_SFX_MAX_VOICES   8

/* Initialise the sfx subsystem (registers the mix source with lh_audio).
 * Must be called after lh_audio_init. Idempotent. */
bool lh_sfx_init(void);

/* Tear down: stop voices, free cache, unregister mixer source. */
void lh_sfx_shutdown(void);

/* Eagerly load and decode a sfx into the cache. Returns false on I/O
 * or decode failure. Subsequent lh_sfx_play hits the cache. */
bool lh_sfx_load(const char* media);

/* Play a sfx. Loads on demand if not cached. No-op if master volume
 * is zero or the sfx subsystem failed to initialise. */
void lh_sfx_play(const char* media);

/* Stop every active voice. Cached samples are kept. */
void lh_sfx_stop_all(void);

/* Per-sfx volume scale on a 0..100 scale (applied after master). */
void lh_sfx_set_volume(int v);
int  lh_sfx_get_volume(void);

/* Audio-thread mixer pull. Public so lh_audio can register it. */
void lh_sfx_pull(int32_t* mix, int frames, int channels);

#endif
