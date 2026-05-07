/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Timing implementation backed by SDL_GetTicks64.
 */
#include "lh_le_timing.h"
#include <SDL2/SDL.h>
#include <string.h>

static uint64_t now_ms(void)
{
    return (uint64_t)SDL_GetTicks64();
}

void lh_timing_init(lh_timing* t)
{
    memset(t, 0, sizeof(*t));
}

void lh_timing_start(lh_timing* t)
{
    if (t->started) return;
    t->started  = true;
    t->paused   = false;
    t->start_ms = now_ms();
    t->accum_ms = 0;
}

void lh_timing_stop(lh_timing* t)
{
    t->started = false;
    t->paused  = false;
    t->accum_ms = 0;
}

void lh_timing_pause(lh_timing* t)
{
    if (!t->started || t->paused) return;
    t->accum_ms += now_ms() - t->start_ms;
    t->paused = true;
}

void lh_timing_unpause(lh_timing* t)
{
    if (!t->paused) return;
    t->paused   = false;
    t->start_ms = now_ms();
}

void lh_timing_restart(lh_timing* t)
{
    t->started  = true;
    t->paused   = false;
    t->start_ms = now_ms();
    t->accum_ms = 0;
}

bool lh_timing_started(const lh_timing* t)
{
    return t->started;
}

uint64_t lh_timing_elapsed_ms(const lh_timing* t)
{
    if (!t->started) return 0;
    if (t->paused)   return t->accum_ms;
    return t->accum_ms + (now_ms() - t->start_ms);
}

bool lh_timing_elapsed(const lh_timing* t, uint64_t ms)
{
    return lh_timing_elapsed_ms(t) >= ms;
}
