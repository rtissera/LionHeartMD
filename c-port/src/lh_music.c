/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com) Pierre-Alexandre
 *
 * Music (sc68 chiptune) wiring.
 *
 * Strategy: dlopen libsc68 lazily at lh_music_init. On any failure
 * (library not present, missing symbol, decoder init fails) the
 * module flips into silent-stub mode: every public entry point keeps
 * its contract but produces no audio. This keeps the C port runnable
 * even on systems without sc68 / libao installed.
 *
 * When libsc68 is available we deliberately avoid calling its
 * built-in libao output path. Instead we drive the renderer by hand
 * with sc68_process(), pulling 16-bit interleaved stereo frames at
 * the master mixer's sample rate and adding them into the master mix
 * accumulator from the SDL audio callback.
 *
 * NOTE: libsc68 ships as part of the upstream Java distribution but
 * exposes its native ABI through opaque structs whose C headers are
 * not vendored here. Until those headers (or a lightweight
 * compatibility shim) land in this tree, we keep the dlopen handle
 * but do not call sc68_create / sc68_process: the create_t struct
 * layout is required by sc68_create and is not safe to guess. This is
 * tracked as TODO(c-port-phase4-music). A one-shot warning is logged
 * the first time a track is requested under those conditions.
 *
 * This implementation therefore behaves as a silent stub on every
 * platform today, while keeping all plumbing (mixer source
 * registration, volume scaling, dlopen handle ownership) in place so
 * a follow-up patch only needs to fill in the create / process /
 * load_uri calls.
 */
#include "lh_music.h"
#include "lh_audio.h"
#include "lh_medias.h"

#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    /* dlopen state. */
    void*  lib;                /* libsc68 handle, or NULL */
    bool   warned_unavailable; /* throttle the silent-mode warning */

    /* Public state. */
    int    volume;             /* 0..100 */
    bool   initialised;
    bool   playing;
    bool   paused;
    char   current_media[LH_MEDIA_PATH_MAX];

    /* Resolved sc68 entry points (when lib != NULL). Future use. */
    int  (*p_sc68_init)(void* init);
    void (*p_sc68_shutdown)(void);
    void*(*p_sc68_create)(void* arg);
    void (*p_sc68_destroy)(void* sc68);
    int  (*p_sc68_load_uri)(void* sc68, const char* uri);
    int  (*p_sc68_play)(void* sc68, int track, int loop);
    int  (*p_sc68_stop)(void* sc68);
    int  (*p_sc68_process)(void* sc68, void* buffer, int* frames);
    int  (*p_sc68_cntl)(void* sc68, int cmd, ...);
    void* sc68;                /* decoder instance, or NULL */
} lh_music_state;

static lh_music_state g_music = {
    .lib = NULL,
    .warned_unavailable = false,
    .volume = 100,
    .initialised = false,
    .playing = false,
    .paused = false,
};

static void lh_music_warn_once(const char* fmt, ...)
{
    if (g_music.warned_unavailable) return;
    g_music.warned_unavailable = true;
    fprintf(stderr, "lh_music: %s\n",
            fmt ? fmt : "sc68 not available, silent");
}

#define LH_MUSIC_TRY_LOAD(field, name)                                     \
    do {                                                                   \
        *(void**)(&g_music.field) = dlsym(g_music.lib, name);              \
        if (g_music.field == NULL) ok = false;                             \
    } while (0)

static bool lh_music_dlopen(void)
{
    /* Try a few candidate paths in priority order. */
    static const char* const candidates[] = {
        "lib/libsc68.so",                  /* relative to cwd */
        "./lib/libsc68.so",
        "libsc68.so",                      /* system / LD_LIBRARY_PATH */
        "libsc68.so.0",
        NULL
    };

    for (int i = 0; candidates[i] != NULL; ++i)
    {
        void* h = dlopen(candidates[i], RTLD_NOW | RTLD_LOCAL);
        if (h != NULL)
        {
            g_music.lib = h;
            break;
        }
    }
    if (g_music.lib == NULL) return false;

    bool ok = true;
    LH_MUSIC_TRY_LOAD(p_sc68_init,     "sc68_init");
    LH_MUSIC_TRY_LOAD(p_sc68_shutdown, "sc68_shutdown");
    LH_MUSIC_TRY_LOAD(p_sc68_create,   "sc68_create");
    LH_MUSIC_TRY_LOAD(p_sc68_destroy,  "sc68_destroy");
    LH_MUSIC_TRY_LOAD(p_sc68_load_uri, "sc68_load_uri");
    LH_MUSIC_TRY_LOAD(p_sc68_play,     "sc68_play");
    LH_MUSIC_TRY_LOAD(p_sc68_stop,     "sc68_stop");
    LH_MUSIC_TRY_LOAD(p_sc68_process,  "sc68_process");
    LH_MUSIC_TRY_LOAD(p_sc68_cntl,     "sc68_cntl");
    if (!ok)
    {
        dlclose(g_music.lib);
        g_music.lib = NULL;
        return false;
    }
    return true;
}

bool lh_music_init(void)
{
    if (g_music.initialised) return true;

    g_music.initialised = true;
    g_music.playing     = false;
    g_music.paused      = false;
    g_music.current_media[0] = '\0';

    /* Best effort: try to load the lib so a future patch can wire the
     * decoder. Failure is non-fatal - we just stay in silent mode. */
    (void)lh_music_dlopen();

    lh_audio_register_source(lh_music_pull);
    return true;
}

void lh_music_shutdown(void)
{
    if (!g_music.initialised) return;
    lh_music_stop();
    lh_audio_unregister_source(lh_music_pull);
    if (g_music.lib != NULL)
    {
        dlclose(g_music.lib);
        g_music.lib = NULL;
    }
    g_music.initialised = false;
}

bool lh_music_is_available(void)
{
    /* Available means: lib loaded *and* we are confident in the
     * create/process call path. The latter requires sc68 headers (see
     * file banner) so for now we report unavailable even when the lib
     * is present. */
    return false;
}

bool lh_music_play(const char* media)
{
    if (!g_music.initialised || media == NULL) return false;

    /* Silent-stub fast path. */
    if (!lh_music_is_available())
    {
        if (g_music.lib == NULL)
        {
            lh_music_warn_once("sc68 not available, silent");
        }
        else
        {
            lh_music_warn_once(
                "sc68 lib loaded but decoder bindings absent, silent "
                "(TODO c-port-phase4-music)");
        }
        snprintf(g_music.current_media, sizeof(g_music.current_media),
                 "%s", media);
        g_music.playing = true;
        g_music.paused  = false;
        return true;
    }
    /* Future path: sc68_create + sc68_load_uri + sc68_play. */
    return false;
}

void lh_music_stop(void)
{
    if (!g_music.initialised) return;
    lh_audio_lock();
    g_music.playing          = false;
    g_music.paused           = false;
    g_music.current_media[0] = '\0';
    lh_audio_unlock();
}

void lh_music_pause(void)
{
    if (!g_music.initialised) return;
    g_music.paused = true;
}

void lh_music_resume(void)
{
    if (!g_music.initialised) return;
    g_music.paused = false;
}

void lh_music_set_volume(int v)
{
    if (v < 0)   v = 0;
    if (v > 100) v = 100;
    g_music.volume = v;
}

int lh_music_get_volume(void) { return g_music.volume; }

void lh_music_pull(int32_t* mix, int frames, int channels)
{
    (void)mix; (void)frames; (void)channels;
    /* Silent stub: no PCM contributed. When the decoder bindings are
     * filled in this becomes:
     *
     *   if (!g_music.playing || g_music.paused || g_music.sc68 == NULL)
     *       return;
     *   int n = frames;
     *   int16_t buf[2 * frames];
     *   if (g_music.p_sc68_process(g_music.sc68, buf, &n) < 0) return;
     *   const int v = g_music.volume;
     *   for (int f = 0; f < n; ++f) {
     *       mix[f * channels + 0] += (int32_t)buf[2*f + 0] * v / 100;
     *       if (channels > 1)
     *           mix[f * channels + 1] += (int32_t)buf[2*f + 1] * v / 100;
     *   }
     */
}
