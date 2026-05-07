/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "sequence_intro.h"
#include "sequence_menu.h"
#include "lh_constant.h"

#include <stdlib.h>
#include <string.h>

/* Tick budget per cinematic part (in 50Hz ticks). Java uses absolute ms
 * synced with sc68 music. Skeleton uses fixed-tick approximation. */
#define INTRO_PART1_TICKS 600          /* ~12s — stories scroll  */
#define INTRO_PART2_TICKS 600          /* ~12s — palace pan      */
#define INTRO_PART3_TICKS 600          /* ~12s — dragon flight   */
#define INTRO_PART4_TICKS 400          /* ~8s  — hero arrival    */
#define INTRO_FADE_TICKS  50           /* per-part fade duration */

typedef enum {
    INTRO_STATE_PART1 = 0,
    INTRO_STATE_PART2,
    INTRO_STATE_PART3,
    INTRO_STATE_PART4,
    INTRO_STATE_DONE,
    INTRO_STATE_QUIT
} intro_state;

typedef struct {
    lh_sequence    base;
    lh_game_config game;
    intro_state    state;
    int            part_tick;
    int            total_tick;
    bool           skipped;
} intro_seq;

typedef struct {
    lh_resolution  res;
    lh_game_config game;
} intro_to_menu_args;

static lh_sequence* menu_factory(void* args)
{
    intro_to_menu_args* a = (intro_to_menu_args*)args;
    lh_sequence* s = menu_sequence_new(&a->res, &a->game);
    free(a);
    return s;
}

static void intro_load(lh_sequence* s)
{
    (void)s;
    /* TODO phase 9: lh_music_play("com/b3dgs/lionheart/music/intro.sc68");
     * preload story banner sprites. */
}

static int part_budget(intro_state st)
{
    switch (st)
    {
        case INTRO_STATE_PART1: return INTRO_PART1_TICKS;
        case INTRO_STATE_PART2: return INTRO_PART2_TICKS;
        case INTRO_STATE_PART3: return INTRO_PART3_TICKS;
        case INTRO_STATE_PART4: return INTRO_PART4_TICKS;
        default:                return 0;
    }
}

static void intro_handle_input(intro_seq* self)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT) { self->state = INTRO_STATE_QUIT; return; }
        if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_ESCAPE: self->state = INTRO_STATE_QUIT; return;
                case SDLK_SPACE:
                case SDLK_RETURN: self->skipped = true; self->state = INTRO_STATE_DONE; return;
                default: break;
            }
        }
    }
}

static void intro_update(lh_sequence* s, double extrp)
{
    (void)extrp;
    intro_seq* self = (intro_seq*)s;
    self->part_tick++;
    self->total_tick++;
    intro_handle_input(self);

    if (self->state == INTRO_STATE_QUIT)
    {
        lh_sequence_end(s);
        return;
    }

    if (self->state == INTRO_STATE_DONE)
    {
        intro_to_menu_args* a = (intro_to_menu_args*)calloc(1, sizeof(*a));
        a->res  = self->base.resolution;
        a->game = self->game;
        lh_sequence_end_with(s, menu_factory, a);
        return;
    }

    /* Advance through parts on tick budget exhaustion. */
    if (self->part_tick >= part_budget(self->state))
    {
        self->part_tick = 0;
        self->state = (intro_state)(self->state + 1);
    }
}

static int fade_alpha(int part_tick, int budget)
{
    /* Triangle: 0 -> 255 over INTRO_FADE_TICKS, hold at 255, then 255 ->
     * 0 over the last INTRO_FADE_TICKS ticks of the part. */
    if (part_tick < INTRO_FADE_TICKS)
        return (part_tick * 255) / INTRO_FADE_TICKS;
    const int tail = budget - INTRO_FADE_TICKS;
    if (part_tick > tail)
        return ((budget - part_tick) * 255) / INTRO_FADE_TICKS;
    return 255;
}

static void intro_render(lh_sequence* s, SDL_Renderer* g)
{
    intro_seq* self = (intro_seq*)s;

    /* Part-tinted backdrop placeholder. Full art lands phase 9. */
    Uint8 r = 0, gg = 0, b = 0;
    switch (self->state)
    {
        case INTRO_STATE_PART1: r = 30;  gg = 0;   b = 60;  break;  /* dusk     */
        case INTRO_STATE_PART2: r = 60;  gg = 30;  b = 90;  break;  /* palace   */
        case INTRO_STATE_PART3: r = 90;  gg = 30;  b = 30;  break;  /* dragon   */
        case INTRO_STATE_PART4: r = 60;  gg = 60;  b = 30;  break;  /* arrival  */
        default:                                              break;
    }
    SDL_SetRenderDrawColor(g, r, gg, b, 255);
    SDL_RenderClear(g);

    /* Per-part fade overlay. */
    const int budget = part_budget(self->state);
    if (budget > 0)
    {
        const int alpha = 255 - fade_alpha(self->part_tick, budget);
        if (alpha > 0)
        {
            SDL_SetRenderDrawBlendMode(g, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(g, 0, 0, 0, (Uint8)alpha);
            const SDL_Rect full = { 0, 0, LH_RES_W, LH_RES_H };
            SDL_RenderFillRect(g, &full);
        }
    }
}

static void intro_destroy(lh_sequence* s)
{
    free(s);
}

static const lh_sequence_vt INTRO_VT = {
    .load                  = intro_load,
    .update                = intro_update,
    .render                = intro_render,
    .on_resolution_changed = NULL,
    .on_terminated         = NULL,
    .destroy               = intro_destroy,
};

lh_sequence* intro_sequence_new(const lh_resolution* res, const lh_game_config* game)
{
    intro_seq* s = (intro_seq*)calloc(1, sizeof(*s));
    if (!s) return NULL;
    lh_sequence_init(&s->base, &INTRO_VT, res);
    if (game) s->game = *game; else lh_game_config_default(&s->game);
    s->state = INTRO_STATE_PART1;
    return &s->base;
}
