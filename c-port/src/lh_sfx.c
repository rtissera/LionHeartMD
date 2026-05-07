/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com) Pierre-Alexandre
 *
 * Sfx (WAV) cache + voice bank implementation.
 *
 * Loaded WAVs are converted to the audio device's format (S16
 * interleaved, native channel count) once at load time and kept in
 * memory for the lifetime of the process. The cache is bounded
 * (LH_SFX_MAX_CACHED entries) with LRU eviction. Playback allocates a
 * voice from a fixed pool (LH_SFX_MAX_VOICES) and overwrites the
 * least-recently-used voice when all are busy.
 */
#include "lh_sfx.h"
#include "lh_audio.h"
#include "lh_medias.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char      media[LH_MEDIA_PATH_MAX];
    int16_t*  pcm;          /* device-format S16 interleaved */
    int       frames;       /* total frames in pcm */
    int       channels;     /* always == device channels after convert */
    uint64_t  last_used;    /* LRU tick */
    bool      in_use;
} lh_sfx_entry;

typedef struct {
    const lh_sfx_entry* entry; /* NULL = free */
    int                 cursor; /* frame index, advances each pull */
    uint64_t            started;
} lh_sfx_voice;

static lh_sfx_entry g_cache[LH_SFX_MAX_CACHED];
static lh_sfx_voice g_voices[LH_SFX_MAX_VOICES];
static int          g_volume      = 100;
static uint64_t     g_clock       = 0;
static int          g_round_robin = 0;
static bool         g_initialised = false;

static lh_sfx_entry* lh_sfx_find_cached(const char* media)
{
    for (int i = 0; i < LH_SFX_MAX_CACHED; ++i)
    {
        if (g_cache[i].in_use &&
            strncmp(g_cache[i].media, media, sizeof(g_cache[i].media)) == 0)
        {
            return &g_cache[i];
        }
    }
    return NULL;
}

static lh_sfx_entry* lh_sfx_pick_slot(void)
{
    /* First free slot. */
    for (int i = 0; i < LH_SFX_MAX_CACHED; ++i)
    {
        if (!g_cache[i].in_use) return &g_cache[i];
    }
    /* Else evict LRU. */
    int      victim     = 0;
    uint64_t victim_age = g_cache[0].last_used;
    for (int i = 1; i < LH_SFX_MAX_CACHED; ++i)
    {
        if (g_cache[i].last_used < victim_age)
        {
            victim     = i;
            victim_age = g_cache[i].last_used;
        }
    }
    return &g_cache[victim];
}

static void lh_sfx_release_entry(lh_sfx_entry* e)
{
    if (e == NULL || !e->in_use) return;

    /* Stop any voice still pointing at this entry. The audio thread is
     * locked by the caller. */
    for (int i = 0; i < LH_SFX_MAX_VOICES; ++i)
    {
        if (g_voices[i].entry == e)
        {
            g_voices[i].entry  = NULL;
            g_voices[i].cursor = 0;
        }
    }
    free(e->pcm);
    memset(e, 0, sizeof(*e));
}

bool lh_sfx_init(void)
{
    if (g_initialised) return true;
    memset(g_cache,  0, sizeof(g_cache));
    memset(g_voices, 0, sizeof(g_voices));
    g_clock       = 0;
    g_round_robin = 0;
    g_initialised = lh_audio_register_source(lh_sfx_pull);
    return g_initialised;
}

void lh_sfx_shutdown(void)
{
    if (!g_initialised) return;
    lh_audio_unregister_source(lh_sfx_pull);
    lh_audio_lock();
    for (int i = 0; i < LH_SFX_MAX_CACHED; ++i)
    {
        if (g_cache[i].in_use)
        {
            free(g_cache[i].pcm);
            memset(&g_cache[i], 0, sizeof(g_cache[i]));
        }
    }
    memset(g_voices, 0, sizeof(g_voices));
    lh_audio_unlock();
    g_initialised = false;
}

bool lh_sfx_load(const char* media)
{
    if (!g_initialised || media == NULL) return false;

    if (lh_sfx_find_cached(media) != NULL) return true;

    char fs_path[LH_MEDIA_PATH_MAX];
    if (!lh_medias_resolve(media, fs_path, sizeof(fs_path)))
    {
        fprintf(stderr, "lh_sfx: cannot resolve '%s'\n", media);
        return false;
    }

    SDL_AudioSpec  wav_spec;
    Uint8*         wav_buf = NULL;
    Uint32         wav_len = 0;
    if (SDL_LoadWAV(fs_path, &wav_spec, &wav_buf, &wav_len) == NULL)
    {
        fprintf(stderr, "lh_sfx: SDL_LoadWAV('%s') failed: %s\n",
                fs_path, SDL_GetError());
        return false;
    }

    const int dev_rate = lh_audio_get_sample_rate();
    const int dev_ch   = lh_audio_get_channels();

    SDL_AudioCVT cvt;
    SDL_zero(cvt);
    int rc = SDL_BuildAudioCVT(&cvt,
                               wav_spec.format, wav_spec.channels, wav_spec.freq,
                               AUDIO_S16SYS,    (Uint8)dev_ch,     dev_rate);
    if (rc < 0)
    {
        fprintf(stderr, "lh_sfx: SDL_BuildAudioCVT failed: %s\n", SDL_GetError());
        SDL_FreeWAV(wav_buf);
        return false;
    }

    int16_t* pcm    = NULL;
    int      frames = 0;
    if (rc == 0)
    {
        /* No conversion needed. */
        pcm = (int16_t*)malloc(wav_len);
        if (pcm == NULL) { SDL_FreeWAV(wav_buf); return false; }
        memcpy(pcm, wav_buf, wav_len);
        frames = (int)wav_len / ((int)sizeof(int16_t) * dev_ch);
    }
    else
    {
        cvt.len = (int)wav_len;
        cvt.buf = (Uint8*)malloc((size_t)cvt.len * cvt.len_mult);
        if (cvt.buf == NULL) { SDL_FreeWAV(wav_buf); return false; }
        memcpy(cvt.buf, wav_buf, wav_len);
        if (SDL_ConvertAudio(&cvt) != 0)
        {
            fprintf(stderr, "lh_sfx: SDL_ConvertAudio failed: %s\n",
                    SDL_GetError());
            free(cvt.buf);
            SDL_FreeWAV(wav_buf);
            return false;
        }
        pcm    = (int16_t*)cvt.buf;
        frames = cvt.len_cvt / ((int)sizeof(int16_t) * dev_ch);
    }
    SDL_FreeWAV(wav_buf);

    lh_audio_lock();
    lh_sfx_entry* slot = lh_sfx_pick_slot();
    lh_sfx_release_entry(slot);
    snprintf(slot->media, sizeof(slot->media), "%s", media);
    slot->pcm       = pcm;
    slot->frames    = frames;
    slot->channels  = dev_ch;
    slot->last_used = ++g_clock;
    slot->in_use    = true;
    lh_audio_unlock();
    return true;
}

void lh_sfx_play(const char* media)
{
    if (!g_initialised || media == NULL || g_volume <= 0) return;

    lh_sfx_entry* e = lh_sfx_find_cached(media);
    if (e == NULL)
    {
        if (!lh_sfx_load(media)) return;
        e = lh_sfx_find_cached(media);
        if (e == NULL) return;
    }

    lh_audio_lock();
    e->last_used = ++g_clock;

    /* Pick a free voice; fall back to round-robin overwrite. */
    int slot = -1;
    for (int i = 0; i < LH_SFX_MAX_VOICES; ++i)
    {
        if (g_voices[i].entry == NULL) { slot = i; break; }
    }
    if (slot < 0)
    {
        slot          = g_round_robin;
        g_round_robin = (g_round_robin + 1) % LH_SFX_MAX_VOICES;
    }
    g_voices[slot].entry   = e;
    g_voices[slot].cursor  = 0;
    g_voices[slot].started = g_clock;
    lh_audio_unlock();
}

void lh_sfx_stop_all(void)
{
    if (!g_initialised) return;
    lh_audio_lock();
    for (int i = 0; i < LH_SFX_MAX_VOICES; ++i)
    {
        g_voices[i].entry  = NULL;
        g_voices[i].cursor = 0;
    }
    lh_audio_unlock();
}

void lh_sfx_set_volume(int v)
{
    if (v < 0)   v = 0;
    if (v > 100) v = 100;
    g_volume = v;
}

int lh_sfx_get_volume(void) { return g_volume; }

void lh_sfx_pull(int32_t* mix, int frames, int channels)
{
    /* Audio thread; assumes audio device lock is held by SDL around
     * the master callback. */
    const int volume = g_volume;
    if (volume <= 0) return;

    for (int v = 0; v < LH_SFX_MAX_VOICES; ++v)
    {
        const lh_sfx_entry* e = g_voices[v].entry;
        if (e == NULL || e->pcm == NULL) continue;

        int      cursor    = g_voices[v].cursor;
        const int remaining = e->frames - cursor;
        if (remaining <= 0)
        {
            g_voices[v].entry  = NULL;
            g_voices[v].cursor = 0;
            continue;
        }
        const int take = (remaining < frames) ? remaining : frames;
        const int ch   = (e->channels < channels) ? e->channels : channels;
        const int16_t* src = e->pcm + (size_t)cursor * e->channels;

        for (int f = 0; f < take; ++f)
        {
            for (int c = 0; c < ch; ++c)
            {
                int32_t s = ((int32_t)src[f * e->channels + c] * volume) / 100;
                mix[f * channels + c] += s;
            }
            /* If sfx is mono and device stereo, dup to remaining channels. */
            if (e->channels == 1 && channels > 1)
            {
                int32_t s = ((int32_t)src[f] * volume) / 100;
                for (int c = 1; c < channels; ++c)
                {
                    mix[f * channels + c] += s;
                }
            }
        }
        cursor += take;
        if (cursor >= e->frames)
        {
            g_voices[v].entry  = NULL;
            g_voices[v].cursor = 0;
        }
        else
        {
            g_voices[v].cursor = cursor;
        }
    }
}
