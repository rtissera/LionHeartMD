/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_checkpoint_handler.h"
#include <stdio.h>
#include <string.h>

void lh_checkpoint_handler_init(lh_checkpoint_handler* h)
{
    if (!h) return;
    memset(h, 0, sizeof(*h));
    h->current = -1;
}

void lh_checkpoint_handler_load(lh_checkpoint_handler* h, const lh_stage_config* stage)
{
    if (!h || !stage) return;
    lh_checkpoint_handler_init(h);

    const int n = stage->checkpoint_count < LH_CHECKPOINT_MAX
                  ? stage->checkpoint_count
                  : LH_CHECKPOINT_MAX;
    for (int i = 0; i < n; i++)
    {
        const lh_stage_checkpoint* sc = &stage->checkpoints[i];
        h->checkpoints[i].id = i;
        /* tx/ty are tile coords in stage XML; pixel conversion deferred to
         * lh_world_load_stage which knows tile dimensions. Store raw for now. */
        h->checkpoints[i].x  = sc->tx;
        h->checkpoints[i].y  = sc->ty;
        if (sc->has_next)
            snprintf(h->checkpoints[i].next_stage, LH_CHECKPOINT_MEDIA_MAX, "%s", sc->next);
        else
            h->checkpoints[i].next_stage[0] = '\0';
    }
    h->count   = n;
    h->current = n > 0 ? 0 : -1;
}

void lh_checkpoint_handler_register(lh_checkpoint_handler* h, const lh_transformable* tf)
{
    if (h) h->tracked = tf;
}

void lh_checkpoint_handler_update(lh_checkpoint_handler* h, double extrp)
{
    (void)extrp;
    if (!h || !h->tracked || h->count == 0) return;

    const double tx = lh_transformable_get_x(h->tracked);
    /* Promote current to the highest-id checkpoint whose x is <= tracked.x. */
    for (int i = h->count - 1; i >= 0; i--)
    {
        if (tx >= h->checkpoints[i].x)
        {
            if (h->current != i) h->current = i;
            return;
        }
    }
}

lh_coord lh_checkpoint_handler_get_current(const lh_checkpoint_handler* h)
{
    lh_coord c = { 0.0, 0.0 };
    if (h && h->current >= 0 && h->current < h->count)
    {
        c.x = h->checkpoints[h->current].x;
        c.y = h->checkpoints[h->current].y;
    }
    return c;
}

const char* lh_checkpoint_handler_get_next_stage(const lh_checkpoint_handler* h)
{
    if (!h || h->current < 0 || h->current >= h->count) return NULL;
    const char* s = h->checkpoints[h->current].next_stage;
    return (s && s[0]) ? s : NULL;
}

int lh_checkpoint_handler_count(const lh_checkpoint_handler* h)
{
    return h ? h->count : 0;
}
