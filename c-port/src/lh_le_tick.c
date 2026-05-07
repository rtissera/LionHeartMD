/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_tick.h"
#include <string.h>

void lh_tick_init(lh_tick* t)
{
    memset(t, 0, sizeof(*t));
}

void lh_tick_start(lh_tick* t)
{
    if (t->started) return;
    t->started = true;
}

void lh_tick_stop(lh_tick* t)
{
    t->started      = false;
    t->frame        = 0;
    t->extrp_acc    = 0.0;
    t->action_count = 0;
}

void lh_tick_restart(lh_tick* t)
{
    t->started      = true;
    t->frame        = 0;
    t->extrp_acc    = 0.0;
    t->action_count = 0;
}

void lh_tick_update(lh_tick* t, double extrp)
{
    if (!t->started) return;
    t->extrp_acc += extrp;
    while (t->extrp_acc >= 1.0)
    {
        t->extrp_acc -= 1.0;
        t->frame++;
        for (int i = 0; i < t->action_count; i++)
        {
            lh_tick_action* a = &t->actions[i];
            if (!a->fired && t->frame >= a->frame)
            {
                a->fired = true;
                if (a->fn) a->fn(a->user);
            }
        }
    }
}

bool lh_tick_started(const lh_tick* t)
{
    return t->started;
}

bool lh_tick_elapsed(const lh_tick* t, int frames)
{
    return t->started && t->frame >= frames;
}

int lh_tick_elapsed_count(const lh_tick* t)
{
    return t->frame;
}

void lh_tick_add_action(lh_tick* t, lh_tick_action_fn fn, void* user, int frame)
{
    if (t->action_count >= LH_TICK_MAX_ACTIONS) return;
    t->actions[t->action_count++] = (lh_tick_action){ fn, user, frame, false };
}
