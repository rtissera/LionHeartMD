/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Minimal hero entity. Phase 7+ first playable: arrow keys move,
 * SPACE/UP jumps. Crude tile collision against the active map.
 *
 * Renders as a placeholder rectangle until phase 8 wires the full
 * Valdyn featurable graph (Animatable + Rasterable + Hurtable + Stats
 * + StateHandler with 18 states).
 */
#ifndef LH_HERO_H
#define LH_HERO_H

#include <stdbool.h>

#include "lh_le_camera.h"
#include "lh_le_graphic.h"
#include "lh_le_image_buffer.h"
#include "lh_le_map_tile.h"
#include "lh_le_transformable.h"
#include "lh_tile_collision.h"

typedef enum {
    LH_HERO_IDLE = 0,
    LH_HERO_WALK,
    LH_HERO_JUMP,
    LH_HERO_FALL
} lh_hero_state;

typedef struct {
    lh_transformable transformable;
    double           vx;
    double           vy;
    bool             on_ground;
    bool             facing_left;
    int              tick;
    lh_hero_state    state;
    int              anim_frame;   /* current 1-based sheet frame */
    int              walk_idx;     /* walk cycle position 0..N */

    /* Sprite atlas (cached, NOT owned). */
    lh_image_buffer* sprite;
    int              sprite_cols;
    int              sprite_rows;
    int              frame_w;
    int              frame_h;
} lh_hero;

void lh_hero_init       (lh_hero* h, double x, double y);
void lh_hero_load_sprite(lh_hero* h, lh_image_buffer* atlas, int cols, int rows);
void lh_hero_update     (lh_hero* h, const lh_tile_collision_ctx* coll, double extrp);
void lh_hero_render     (const lh_hero* h, lh_graphic* g, const lh_camera* cam);

double lh_hero_get_x(const lh_hero* h);
double lh_hero_get_y(const lh_hero* h);

#endif
