/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "sequence_menu.h"
#include "lh_constant.h"
#include "lh_init_config.h"
#include "lh_scene.h"

#include <stdlib.h>
#include <string.h>

typedef enum {
    MENU_STATE_MAIN = 0,
    MENU_STATE_PLAY,
    MENU_STATE_QUIT
} menu_state;

typedef struct {
    lh_sequence    base;
    lh_game_config game;
    int            tick;
    menu_state     state;
} menu_seq;

/* Container holding the GameConfig payload for the next-sequence factory. */
typedef struct {
    lh_game_config game;
    lh_init_config init;
} menu_to_scene_args;

static lh_sequence* scene_factory(void* args)
{
    menu_to_scene_args* a = (menu_to_scene_args*)args;
    lh_sequence* s = lh_scene_new(&a->game, &a->init);
    free(a);
    return s;
}

static void menu_load(lh_sequence* s)
{
    /* TODO phase 9: preload menu fonts/sprites; play menu music. */
    (void)s;
}

static void menu_handle_input(menu_seq* self)
{
    /* Pump SDL events: ESC quits, SPACE/ENTER plays. */
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT) { self->state = MENU_STATE_QUIT; break; }
        if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_ESCAPE: self->state = MENU_STATE_QUIT; break;
                case SDLK_SPACE:
                case SDLK_RETURN: self->state = MENU_STATE_PLAY; break;
                default: break;
            }
        }
    }
}

static void menu_update(lh_sequence* s, double extrp)
{
    (void)extrp;
    menu_seq* self = (menu_seq*)s;
    self->tick++;
    menu_handle_input(self);

    if (self->state == MENU_STATE_QUIT)
    {
        lh_sequence_end(s);
        return;
    }
    if (self->state == MENU_STATE_PLAY)
    {
        menu_to_scene_args* a = (menu_to_scene_args*)calloc(1, sizeof(*a));
        a->game = self->game;
        lh_init_config_default(&a->init);
        a->init.stage = "com/b3dgs/lionheart/stage/story/beginner/stage1.xml";
        lh_sequence_end_with(s, scene_factory, a);
    }
}

static void menu_render(lh_sequence* s, SDL_Renderer* g)
{
    menu_seq* self = (menu_seq*)s;

    /* Solid dark blue background so the boot path is visible. */
    SDL_SetRenderDrawColor(g, 0, 0, 32, 255);
    SDL_RenderClear(g);

    /* Pulsing call-to-action rect — placeholder for "PRESS SPACE TO START".
     * Phase 9 swaps this for a real text-rendered menu. */
    const Uint8 v = (Uint8)(128 + (self->tick & 0x7F));
    SDL_SetRenderDrawColor(g, v, v, 0, 255);
    const SDL_Rect r = {
        LH_RES_W / 2 - 64,
        LH_RES_H / 2 - 8,
        128, 16
    };
    SDL_RenderFillRect(g, &r);
}

static void menu_destroy(lh_sequence* s)
{
    free(s);
}

static const lh_sequence_vt MENU_VT = {
    .load                  = menu_load,
    .update                = menu_update,
    .render                = menu_render,
    .on_resolution_changed = NULL,
    .on_terminated         = NULL,
    .destroy               = menu_destroy,
};

lh_sequence* menu_sequence_new(const lh_resolution* res, const lh_game_config* game)
{
    menu_seq* s = (menu_seq*)calloc(1, sizeof(*s));
    if (!s) return NULL;
    lh_sequence_init(&s->base, &MENU_VT, res);
    if (game) s->game = *game;     else lh_game_config_default(&s->game);
    s->state = MENU_STATE_MAIN;
    return &s->base;
}
