/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com)
 *                          Pierre-Alexandre (contact@b3dgs.com)
 *
 * Music (sc68 chiptune) wiring.
 *
 * Mirrors com.b3dgs.lionheart.Music / MusicPlayer using libsc68 to
 * decode the .sc68 tracks shipped under
 * com/b3dgs/lionheart/music (intro.sc68, lava.sc68, etc.).
 *
 * Strategy: dlopen libsc68 lazily at lh_music_init.
 *
 *   - The vendored libsc68.so was originally built with libao /
 *     PortAudio / libz / libasound / libm as soft dependencies. Those
 *     are NOT recorded in the .so's NEEDED list so a plain dlopen with
 *     RTLD_NOW + RTLD_LOCAL fails with "undefined symbol: ao_driver_id"
 *     even on a Debian/Ubuntu host where the libs are installed. To
 *     work around this we proactively dlopen libao.so.4,
 *     libportaudio.so.2 and libz.so.1 with RTLD_GLOBAL right before
 *     loading libsc68 — those symbols then become resolvable to the
 *     subsequent libsc68 dlopen. None of those external entry points
 *     are actually called from this translation unit; the player is
 *     driven by hand through sc68_process() into the SDL mixer
 *     callback.
 *
 *   - On any failure (library not present, missing symbol, decoder
 *     init fails) the module flips into silent-stub mode: every public
 *     entry point keeps its contract but produces no audio. This keeps
 *     the C port runnable even on systems without sc68 installed.
 *
 *   - When libsc68 is available we deliberately avoid calling its
 *     built-in libao output path. Instead we drive the renderer by
 *     hand with sc68_process(), pulling 16-bit interleaved stereo
 *     frames at the master mixer's sample rate and adding them into
 *     the master mix accumulator from the SDL audio callback.
 *
 * sc68's public API used here is documented in vendor/sc68/sc68.h
 * (LGPL header preserved verbatim).
 */
#include "lh_music.h"
#include "lh_audio.h"
#include "lh_medias.h"
#include "../vendor/sc68/sc68.h"

#include <dlfcn.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LH_MUSIC_MAX_BLOCK 4096   /* stereo frames per sc68_process pass */

typedef struct {
    /* dlopen state. */
    void*  lib;                /* libsc68 handle, or NULL */
    void*  lib_ao;             /* libao.so.4 handle (RTLD_GLOBAL) */
    void*  lib_pa;             /* libportaudio.so.2 handle (RTLD_GLOBAL) */
    void*  lib_z;              /* libz.so.1 handle (RTLD_GLOBAL) */
    bool   warned_unavailable; /* throttle the silent-mode warning */

    /* Public state. */
    int    volume;             /* 0..100 */
    bool   initialised;
    bool   playing;
    bool   paused;
    bool   ended;              /* set by audio thread when SC68_END seen */
    char   current_media[LH_MEDIA_PATH_MAX];

    /* Resolved sc68 entry points (when lib != NULL). */
    int     (*p_sc68_init)(void* init);
    void    (*p_sc68_shutdown)(void);
    sc68_t* (*p_sc68_create)(void* arg);
    void    (*p_sc68_destroy)(sc68_t* sc68);
    int     (*p_sc68_load_uri)(sc68_t* sc68, const char* uri);
    int     (*p_sc68_play)(sc68_t* sc68, int track, int loop);
    int     (*p_sc68_stop)(sc68_t* sc68);
    int     (*p_sc68_process)(sc68_t* sc68, void* buffer, int* frames);

    sc68_t* sc68;              /* decoder instance, or NULL */
    bool    sc68_inited;       /* sc68_init() has been called */
} lh_music_state;

static lh_music_state g_music = {
    .lib = NULL,
    .lib_ao = NULL, .lib_pa = NULL, .lib_z = NULL,
    .warned_unavailable = false,
    .volume = 100,
    .initialised = false,
    .playing = false,
    .paused = false,
    .ended = false,
    .sc68 = NULL,
    .sc68_inited = false,
};

static void lh_music_warn_once(const char* fmt, ...)
{
    if (g_music.warned_unavailable) return;
    g_music.warned_unavailable = true;
    va_list ap;
    va_start(ap, fmt);
    fputs("lh_music: ", stderr);
    vfprintf(stderr, fmt ? fmt : "sc68 not available, silent", ap);
    fputc('\n', stderr);
    va_end(ap);
}

#define LH_MUSIC_TRY_LOAD(field, name)                                     \
    do {                                                                   \
        *(void**)(&g_music.field) = dlsym(g_music.lib, name);              \
        if (g_music.field == NULL) ok = false;                             \
    } while (0)

static void lh_music_preload_deps(void)
{
    /* libsc68.so does not record libao / libportaudio / libz in its
     * DT_NEEDED, but references their symbols. Bring them into the
     * global namespace so the libsc68 dlopen below can resolve. */
    static const char* const ao_names[] = {
        "libao.so.4", "libao.so", NULL
    };
    static const char* const pa_names[] = {
        "libportaudio.so.2", "libportaudio.so", NULL
    };
    static const char* const z_names[] = {
        "libz.so.1", "libz.so", NULL
    };
    for (int i = 0; ao_names[i] && !g_music.lib_ao; ++i)
        g_music.lib_ao = dlopen(ao_names[i], RTLD_NOW | RTLD_GLOBAL);
    for (int i = 0; pa_names[i] && !g_music.lib_pa; ++i)
        g_music.lib_pa = dlopen(pa_names[i], RTLD_NOW | RTLD_GLOBAL);
    for (int i = 0; z_names[i]  && !g_music.lib_z;  ++i)
        g_music.lib_z  = dlopen(z_names[i],  RTLD_NOW | RTLD_GLOBAL);
}

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

    lh_music_preload_deps();

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
    if (!ok)
    {
        dlclose(g_music.lib);
        g_music.lib = NULL;
        return false;
    }

    /* sc68_init(NULL) uses defaults (verified via disassembly). */
    if (g_music.p_sc68_init(NULL) != 0)
    {
        dlclose(g_music.lib);
        g_music.lib = NULL;
        return false;
    }
    g_music.sc68_inited = true;
    return true;
}

bool lh_music_init(void)
{
    if (g_music.initialised) return true;

    g_music.initialised      = true;
    g_music.playing          = false;
    g_music.paused           = false;
    g_music.ended            = false;
    g_music.current_media[0] = '\0';

    /* Best effort: try to load the lib so playback is real. Failure is
     * non-fatal — we just stay in silent mode. */
    (void)lh_music_dlopen();

    lh_audio_register_source(lh_music_pull);
    return true;
}

void lh_music_shutdown(void)
{
    if (!g_music.initialised) return;
    lh_music_stop();
    lh_audio_unregister_source(lh_music_pull);

    if (g_music.sc68_inited && g_music.p_sc68_shutdown != NULL)
    {
        g_music.p_sc68_shutdown();
        g_music.sc68_inited = false;
    }
    if (g_music.lib != NULL)
    {
        dlclose(g_music.lib);
        g_music.lib = NULL;
    }
    /* Leak the dependency handles intentionally: libao / libportaudio
     * may have spawned threads (especially via PulseAudio) and unmapping
     * them at shutdown can crash on exit. They are leaked exactly once
     * for the lifetime of the process. */
    g_music.initialised = false;
}

bool lh_music_is_available(void)
{
    return g_music.lib != NULL && g_music.sc68_inited
        && g_music.p_sc68_create != NULL
        && g_music.p_sc68_process != NULL;
}

static void lh_music_destroy_sc68_locked(void)
{
    if (g_music.sc68 != NULL && g_music.p_sc68_stop != NULL)
    {
        g_music.p_sc68_stop(g_music.sc68);
    }
    if (g_music.sc68 != NULL && g_music.p_sc68_destroy != NULL)
    {
        g_music.p_sc68_destroy(g_music.sc68);
    }
    g_music.sc68    = NULL;
    g_music.playing = false;
    g_music.paused  = false;
    g_music.ended   = false;
}

bool lh_music_play(const char* media)
{
    if (!g_music.initialised || media == NULL) return false;

    if (!lh_music_is_available())
    {
        if (g_music.lib == NULL)
            lh_music_warn_once("sc68 not available, silent");
        else
            lh_music_warn_once("sc68 lib loaded but init failed, silent");
        snprintf(g_music.current_media, sizeof(g_music.current_media),
                 "%s", media);
        g_music.playing = true;
        g_music.paused  = false;
        return true;
    }

    char fs_path[LH_MEDIA_PATH_MAX];
    if (!lh_medias_resolve(media, fs_path, sizeof(fs_path)))
    {
        fprintf(stderr, "lh_music: cannot resolve %s\n", media);
        return false;
    }

    /* Build the new instance OUTSIDE the audio lock (load can be slow),
     * then atomically swap under the lock. */
    sc68_t* sc = g_music.p_sc68_create(NULL);
    if (sc == NULL)
    {
        fprintf(stderr, "lh_music: sc68_create failed for %s\n", fs_path);
        return false;
    }
    if (g_music.p_sc68_load_uri(sc, fs_path) != 0)
    {
        fprintf(stderr, "lh_music: sc68_load_uri failed: %s\n", fs_path);
        g_music.p_sc68_destroy(sc);
        return false;
    }
    if (g_music.p_sc68_play(sc, -1, -1) != 0)
    {
        fprintf(stderr, "lh_music: sc68_play failed: %s\n", fs_path);
        g_music.p_sc68_destroy(sc);
        return false;
    }

    lh_audio_lock();
    lh_music_destroy_sc68_locked();
    g_music.sc68    = sc;
    g_music.playing = true;
    g_music.paused  = false;
    g_music.ended   = false;
    snprintf(g_music.current_media, sizeof(g_music.current_media),
             "%s", media);
    lh_audio_unlock();
    return true;
}

void lh_music_stop(void)
{
    if (!g_music.initialised) return;
    lh_audio_lock();
    lh_music_destroy_sc68_locked();
    g_music.current_media[0] = '\0';
    lh_audio_unlock();
}

void lh_music_pause(void)
{
    if (!g_music.initialised) return;
    lh_audio_lock();
    g_music.paused = true;
    lh_audio_unlock();
}

void lh_music_resume(void)
{
    if (!g_music.initialised) return;
    lh_audio_lock();
    g_music.paused = false;
    lh_audio_unlock();
}

void lh_music_set_volume(int v)
{
    if (v < 0)   v = 0;
    if (v > 100) v = 100;
    g_music.volume = v;
}

int lh_music_get_volume(void) { return g_music.volume; }

/*
 * Audio thread: pull up to `frames` stereo frames out of sc68 and add
 * them into the int32 mixer accumulator with the current music volume
 * applied. Operates entirely under the SDL audio lock (which the caller
 * has already taken in lh_audio_callback via the source registration).
 */
void lh_music_pull(int32_t* mix, int frames, int channels)
{
    if (!g_music.initialised || !g_music.playing || g_music.paused
        || g_music.ended || g_music.sc68 == NULL
        || g_music.p_sc68_process == NULL
        || mix == NULL || frames <= 0)
    {
        return;
    }

    int16_t buf[LH_MUSIC_MAX_BLOCK * 2]; /* stereo */
    int produced = 0;

    while (produced < frames)
    {
        int want = frames - produced;
        if (want > LH_MUSIC_MAX_BLOCK) want = LH_MUSIC_MAX_BLOCK;

        int n = want;
        const int r = g_music.p_sc68_process(g_music.sc68, buf, &n);
        if (r == SC68_ERROR)
        {
            g_music.ended = true;
            return;
        }
        if (n <= 0)
        {
            /* First call after sc68_play often returns 0 frames + flag
             * SC68_CHANGE; just retry for the remainder of the buffer
             * once. After that, give up to avoid spinning. */
            if (r & SC68_CHANGE) continue;
            break;
        }

        const int v   = g_music.volume;
        const int lim = n;
        for (int f = 0; f < lim; ++f)
        {
            const int32_t sl = (int32_t)buf[2 * f + 0] * v / 100;
            const int32_t sr = (int32_t)buf[2 * f + 1] * v / 100;
            const int dst    = (produced + f) * channels;
            mix[dst + 0] += sl;
            if (channels > 1)
                mix[dst + 1] += sr;
        }
        produced += n;

        if (r & SC68_END)
        {
            g_music.ended = true;
            break;
        }
    }
}
