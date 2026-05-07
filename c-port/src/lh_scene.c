/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_scene.h"
#include "lh_le_graphic.h"
#include "lh_constant.h"

#include <stdlib.h>
#include <string.h>

static void scene_load(lh_sequence* s)
{
    lh_scene* sc = (lh_scene*)s;
    if (sc->started) return;

    /* Bring up the world container. View dimensions = source resolution. */
    lh_world_init(&sc->world, &sc->game, &sc->init,
                  sc->seq.resolution.width, sc->seq.resolution.height);

    /* If the init config carries a stage media, load it. */
    if (sc->init.stage)
    {
        lh_world_load_stage(&sc->world, sc->init.stage);
    }
    sc->started = true;
}

static void scene_update(lh_sequence* s, double extrp)
{
    lh_scene* sc = (lh_scene*)s;
    lh_world_update(&sc->world, extrp);

    /* Stage transition request? Consume + chain. */
    const char* next = lh_load_next_stage_consume(&sc->world.next_stage);
    if (next && next[0])
    {
        /* Update init.stage in-place + reload. Phase 7 keeps it simple —
         * tear the world down, init again, load the new stage. Phase 8+
         * may want a smoother fade transition. */
        snprintf(sc->stage_buf, sizeof(sc->stage_buf), "%s", next);
        sc->init.stage = sc->stage_buf;
        lh_world_destroy(&sc->world);
        sc->started = false;
        scene_load(s);
    }
}

static void scene_render(lh_sequence* s, SDL_Renderer* g)
{
    lh_scene* sc = (lh_scene*)s;

    /* Wrap the SDL renderer in lh_graphic for world's draw API. */
    lh_graphic gx;
    lh_graphic_init(&gx, g);

    /* Clear to black before world draws bg layers. */
    SDL_SetRenderDrawColor(g, 0, 0, 0, 255);
    SDL_RenderClear(g);

    lh_world_render(&sc->world, &gx);
}

static void scene_destroy(lh_sequence* s)
{
    lh_scene* sc = (lh_scene*)s;
    if (sc)
    {
        lh_world_destroy(&sc->world);
        free(sc);
    }
}

static const lh_sequence_vt SCENE_VT = {
    .load                  = scene_load,
    .update                = scene_update,
    .render                = scene_render,
    .on_resolution_changed = NULL,
    .on_terminated         = NULL,
    .destroy               = scene_destroy,
};

lh_sequence* lh_scene_new(const lh_game_config* game, const lh_init_config* init)
{
    lh_scene* s = (lh_scene*)calloc(1, sizeof(*s));
    if (!s) return NULL;

    lh_resolution res = { LH_RES_W, LH_RES_H, LH_RES_RATE };
    lh_sequence_init(&s->seq, &SCENE_VT, &res);

    if (game) s->game = *game;     else lh_game_config_default(&s->game);
    if (init) s->init = *init;     else lh_init_config_default(&s->init);

    return &s->seq;
}
