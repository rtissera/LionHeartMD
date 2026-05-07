/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Stub of Loading.java (com.b3dgs.lionheart.Loading).
 * Phase 1: just renders a black screen + title text-quad placeholder.
 * Phase 5 will fill this in (asset preload + transition to Menu).
 */
#include "lh_engine.h"
#include "lh_constant.h"
#include <stdlib.h>

typedef struct {
    lh_sequence base;
    int         tick;
} loading_seq;

static void loading_load(lh_sequence* s)
{
    (void)s;
    /* TODO phase 5: preload tile sets, sprites, music banks */
}

static void loading_update(lh_sequence* s, double extrp)
{
    (void)extrp;
    loading_seq* self = (loading_seq*)s;
    self->tick++;
    /* Phase 1: idle until ESC. Phase 5: end_with(menu_factory, ...). */
}

static void loading_render(lh_sequence* s, SDL_Renderer* g)
{
    loading_seq* self = (loading_seq*)s;
    /* Pulsing rect placeholder so we can see the loop ticks. */
    Uint8 v = (Uint8)((self->tick * 4) & 0xFF);
    SDL_SetRenderDrawColor(g, v, v / 2, 0, 255);
    SDL_Rect r = { LH_RES_W / 2 - 32, LH_RES_H / 2 - 16, 64, 32 };
    SDL_RenderFillRect(g, &r);
}

static void loading_destroy(lh_sequence* s)
{
    free(s);
}

static const lh_sequence_vt LOADING_VT = {
    .load          = loading_load,
    .update        = loading_update,
    .render        = loading_render,
    .on_terminated = NULL,
    .destroy       = loading_destroy,
};

lh_sequence* loading_sequence_new(const lh_resolution* res)
{
    loading_seq* s = (loading_seq*)calloc(1, sizeof(*s));
    lh_sequence_init(&s->base, &LOADING_VT, res);
    return &s->base;
}
