/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com) Pierre-Alexandre
 *
 * Music (sc68 chiptune) wiring.
 *
 * Mirrors com.b3dgs.lionheart.Music / MusicPlayer using libsc68 to
 * decode the 18 .sc68 tracks shipped under
 * com/b3dgs/lionheart/music/*.sc68.
 *
 * libsc68 ships with the upstream Java distribution as a prebuilt
 * shared library (lib/libsc68.so on this branch). It is dynamically
 * loaded at runtime via dlopen so the C port still links and runs on
 * machines that lack the library; in that case all entry points
 * behave as silent stubs and the audio mixer simply receives no music
 * frames. A one-shot warning is logged the first time playback would
 * have occurred without libsc68.
 *
 * The implementation does not call sc68's built-in libao output. It
 * uses sc68_process to pull 16-bit stereo frames at the configured
 * device sample rate and adds them into the master mixer through the
 * lh_audio source registration.
 */
#ifndef LH_MUSIC_H
#define LH_MUSIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Initialise the music subsystem. Tries to dlopen libsc68; on failure
 * leaves the module in silent-stub mode (returns true regardless,
 * because absence of music is non-fatal). Must be called after
 * lh_audio_init. */
bool lh_music_init(void);

/* Tear down: stop, destroy decoder, dlclose libsc68. */
void lh_music_shutdown(void);

/* Whether libsc68 was successfully loaded at init time. */
bool lh_music_is_available(void);

/* Start a track from a logical media path (e.g.
 * "com/b3dgs/lionheart/music/intro.sc68"). Replaces any current
 * track. Returns false on resolve / load failure. In silent-stub mode
 * always returns true. */
bool lh_music_play(const char* media);

/* Stop the current track (if any). */
void lh_music_stop(void);

/* Pause / resume the current track. */
void lh_music_pause(void);
void lh_music_resume(void);

/* Master music volume on a 0..100 scale. */
void lh_music_set_volume(int v);
int  lh_music_get_volume(void);

/* Audio-thread mixer pull. Public so lh_audio can register it. */
void lh_music_pull(int32_t* mix, int frames, int channels);

#endif
