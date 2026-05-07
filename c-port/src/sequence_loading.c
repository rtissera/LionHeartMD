/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Phase 5: Loading sequence. Holds for a short fade-in then transitions
 * to the Menu sequence. Asset preloading hooks left as TODO — phase 8/9
 * will preload fonts/raster banks here once the asset table grows.
 */
#include "lh_constant.h"
#include "lh_engine.h"
#include "sequence_menu.h"
#include "lh_game_config.h"

#include <stdlib.h>
#include <string.h>

#define LOADING_FADE_TICKS 60       /* ~1.2 sec at 50Hz */

typedef struct {
    lh_sequence    base;
    int            tick;
    lh_game_config game;
    bool           transitioned;
} loading_seq;

typedef struct {
    lh_resolution  res;
    lh_game_config game;
} loading_to_menu_args;

static lh_sequence* menu_factory(void* args)
{
    loading_to_menu_args* a = (loading_to_menu_args*)args;
    lh_sequence* s = menu_sequence_new(&a->res, &a->game);
    free(a);
    return s;
}

static void loading_load(lh_sequence* s)
{
    (void)s;
    /* TODO phase 9: lh_asset_get_image(LH_FOLDER_SPRITE "/logo.png");
     * preload + warm caches. */
}

static void loading_update(lh_sequence* s, double extrp)
{
    (void)extrp;
    loading_seq* self = (loading_seq*)s;
    self->tick++;

    /* Quit on ESC or window close. */
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT
            || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
        {
            lh_sequence_end(s);
            return;
        }
    }

    if (!self->transitioned && self->tick >= LOADING_FADE_TICKS)
    {
        loading_to_menu_args* a = (loading_to_menu_args*)calloc(1, sizeof(*a));
        a->res  = self->base.resolution;
        a->game = self->game;
        self->transitioned = true;
        lh_sequence_end_with(s, menu_factory, a);
    }
}

static void loading_render(lh_sequence* s, SDL_Renderer* g)
{
    loading_seq* self = (loading_seq*)s;

    /* Black backdrop. */
    SDL_SetRenderDrawColor(g, 0, 0, 0, 255);
    SDL_RenderClear(g);

    /* Pulsing rect placeholder — visual confirmation the loop ticks. */
    const Uint8 v = (Uint8)((self->tick * 4) & 0xFF);
    SDL_SetRenderDrawColor(g, v, v / 2, 0, 255);
    const SDL_Rect r = { LH_RES_W / 2 - 32, LH_RES_H / 2 - 16, 64, 32 };
    SDL_RenderFillRect(g, &r);
}

static void loading_destroy(lh_sequence* s)
{
    free(s);
}

static const lh_sequence_vt LOADING_VT = {
    .load                  = loading_load,
    .update                = loading_update,
    .render                = loading_render,
    .on_resolution_changed = NULL,
    .on_terminated         = NULL,
    .destroy               = loading_destroy,
};

lh_sequence* loading_sequence_new(const lh_resolution* res)
{
    loading_seq* s = (loading_seq*)calloc(1, sizeof(*s));
    if (!s) return NULL;
    lh_sequence_init(&s->base, &LOADING_VT, res);
    lh_game_config_default(&s->game);
    return &s->base;
}
