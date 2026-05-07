/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com) Pierre-Alexandre
 *
 * SDL audio device + master mixer implementation.
 */
#include "lh_audio.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

#define LH_AUDIO_MAX_SOURCES 4

typedef struct {
    SDL_AudioDeviceID device;
    SDL_AudioSpec     spec;
    int               master_vol;          /* 0..100 */
    int               sample_rate;
    int               channels;
    bool              running;
    lh_audio_pull_fn  sources[LH_AUDIO_MAX_SOURCES];
    int               source_count;
    int32_t*          mix_buf;             /* int32 accumulator, frames * ch */
    int               mix_buf_frames;
} lh_audio_state;

static lh_audio_state g_audio = {0};

/* SDL audio callback: clears the int32 accumulator, asks every source
 * to add its frames into it, scales by master volume, then clamps to
 * int16 stereo into the device buffer. */
static void lh_audio_callback(void* userdata, Uint8* stream, int len)
{
    (void)userdata;

    /* SDL gives us the device buffer in S16 interleaved frames. */
    const int channels    = g_audio.channels;
    const int sample_size = (int)sizeof(int16_t) * channels;
    const int frames      = len / sample_size;
    int16_t*  out         = (int16_t*)stream;

    if (frames <= 0)
    {
        memset(stream, 0, (size_t)len);
        return;
    }

    if (g_audio.mix_buf == NULL || g_audio.mix_buf_frames < frames)
    {
        free(g_audio.mix_buf);
        g_audio.mix_buf = (int32_t*)calloc((size_t)frames * channels,
                                           sizeof(int32_t));
        g_audio.mix_buf_frames = g_audio.mix_buf ? frames : 0;
        if (g_audio.mix_buf == NULL)
        {
            memset(stream, 0, (size_t)len);
            return;
        }
    }
    else
    {
        memset(g_audio.mix_buf, 0,
               (size_t)frames * channels * sizeof(int32_t));
    }

    for (int i = 0; i < g_audio.source_count; ++i)
    {
        if (g_audio.sources[i] != NULL)
        {
            g_audio.sources[i](g_audio.mix_buf, frames, channels);
        }
    }

    /* Scale + clamp into S16. */
    const int    vol     = g_audio.master_vol; /* 0..100 */
    const int    samples = frames * channels;
    const int32_t* in    = g_audio.mix_buf;
    if (vol <= 0)
    {
        memset(out, 0, (size_t)len);
        return;
    }
    if (vol >= 100)
    {
        for (int i = 0; i < samples; ++i)
        {
            int32_t s = in[i];
            if (s > 32767)  s = 32767;
            if (s < -32768) s = -32768;
            out[i] = (int16_t)s;
        }
        return;
    }
    for (int i = 0; i < samples; ++i)
    {
        int32_t s = (in[i] * vol) / 100;
        if (s > 32767)  s = 32767;
        if (s < -32768) s = -32768;
        out[i] = (int16_t)s;
    }
}

bool lh_audio_init(int sample_rate, int channels)
{
    if (g_audio.running)
    {
        return true;
    }

    if (sample_rate <= 0) sample_rate = LH_AUDIO_DEFAULT_RATE;
    if (channels   <= 0) channels    = LH_AUDIO_DEFAULT_CHANNELS;
    if (channels   != 1 && channels != 2) channels = 2;

    if (SDL_WasInit(SDL_INIT_AUDIO) == 0)
    {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        {
            fprintf(stderr, "lh_audio: SDL_InitSubSystem(AUDIO) failed: %s\n",
                    SDL_GetError());
            return false;
        }
    }

    SDL_AudioSpec want;
    SDL_zero(want);
    want.freq     = sample_rate;
    want.format   = AUDIO_S16SYS;
    want.channels = (Uint8)channels;
    want.samples  = 1024;
    want.callback = lh_audio_callback;
    want.userdata = NULL;

    SDL_AudioSpec have;
    SDL_zero(have);
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(
        NULL, 0, &want, &have,
        SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE);

    if (dev == 0)
    {
        fprintf(stderr, "lh_audio: SDL_OpenAudioDevice failed: %s\n",
                SDL_GetError());
        return false;
    }

    g_audio.device       = dev;
    g_audio.spec         = have;
    g_audio.sample_rate  = have.freq;
    g_audio.channels     = have.channels;
    if (g_audio.master_vol <= 0) g_audio.master_vol = 100;
    g_audio.running      = true;
    g_audio.source_count = 0;
    memset(g_audio.sources, 0, sizeof(g_audio.sources));

    SDL_PauseAudioDevice(dev, 0);
    return true;
}

void lh_audio_shutdown(void)
{
    if (!g_audio.running) return;
    SDL_PauseAudioDevice(g_audio.device, 1);
    SDL_CloseAudioDevice(g_audio.device);
    g_audio.device  = 0;
    g_audio.running = false;
    g_audio.source_count = 0;
    memset(g_audio.sources, 0, sizeof(g_audio.sources));
    free(g_audio.mix_buf);
    g_audio.mix_buf = NULL;
    g_audio.mix_buf_frames = 0;
}

bool lh_audio_is_running(void)        { return g_audio.running; }
int  lh_audio_get_sample_rate(void)   { return g_audio.sample_rate; }
int  lh_audio_get_channels(void)      { return g_audio.channels; }
int  lh_audio_get_master_volume(void) { return g_audio.master_vol; }

void lh_audio_set_master_volume(int v)
{
    if (v < 0)   v = 0;
    if (v > 100) v = 100;
    g_audio.master_vol = v;
}

bool lh_audio_register_source(lh_audio_pull_fn fn)
{
    if (fn == NULL) return false;
    bool ok = false;
    if (g_audio.running) SDL_LockAudioDevice(g_audio.device);
    for (int i = 0; i < LH_AUDIO_MAX_SOURCES; ++i)
    {
        if (g_audio.sources[i] == fn) { ok = true; goto done; }
    }
    for (int i = 0; i < LH_AUDIO_MAX_SOURCES; ++i)
    {
        if (g_audio.sources[i] == NULL)
        {
            g_audio.sources[i] = fn;
            if (i + 1 > g_audio.source_count) g_audio.source_count = i + 1;
            ok = true;
            goto done;
        }
    }
done:
    if (g_audio.running) SDL_UnlockAudioDevice(g_audio.device);
    return ok;
}

bool lh_audio_unregister_source(lh_audio_pull_fn fn)
{
    if (fn == NULL) return false;
    bool ok = false;
    if (g_audio.running) SDL_LockAudioDevice(g_audio.device);
    for (int i = 0; i < LH_AUDIO_MAX_SOURCES; ++i)
    {
        if (g_audio.sources[i] == fn)
        {
            g_audio.sources[i] = NULL;
            ok = true;
        }
    }
    /* Recompute high-water source_count. */
    int n = 0;
    for (int i = 0; i < LH_AUDIO_MAX_SOURCES; ++i)
    {
        if (g_audio.sources[i] != NULL) n = i + 1;
    }
    g_audio.source_count = n;
    if (g_audio.running) SDL_UnlockAudioDevice(g_audio.device);
    return ok;
}

void lh_audio_lock(void)
{
    if (g_audio.running) SDL_LockAudioDevice(g_audio.device);
}

void lh_audio_unlock(void)
{
    if (g_audio.running) SDL_UnlockAudioDevice(g_audio.device);
}
