/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_hero.h"

#include "lh_input.h"
#include "lh_le_color.h"

#define HERO_W           16
#define HERO_H           24
#define HERO_WALK_SPEED  2.0
#define HERO_JUMP_VEL    -5.0      /* px/tick at frame 0 */
#define HERO_GRAVITY     0.3
#define HERO_MAX_FALL    8.0

static bool tile_solid(const lh_map_tile* m, int tile_w, int tile_h, double px, double py)
{
    if (!m || tile_w <= 0 || tile_h <= 0) return false;
    return lh_map_tile_get_tile_at_pixel(m, (int)px, (int)py) >= 0;
}

void lh_hero_init(lh_hero* h, double x, double y)
{
    if (!h) return;
    lh_transformable_init(&h->transformable);
    lh_transformable_set_size(&h->transformable, HERO_W, HERO_H);
    lh_transformable_teleport(&h->transformable, x, y);
    h->vx = h->vy = 0.0;
    h->on_ground   = false;
    h->facing_left = false;
    h->tick        = 0;
    h->state       = LH_HERO_IDLE;
    h->anim_frame  = 0;
}

void lh_hero_update(lh_hero* h, const lh_map_tile* map, int tile_w, int tile_h, double extrp)
{
    if (!h) return;
    h->tick++;

    /* Horizontal control. */
    const double ax = lh_input_axis_h();
    h->vx = ax * HERO_WALK_SPEED;
    if (ax < 0) h->facing_left = true;
    else if (ax > 0) h->facing_left = false;

    /* Jump only when grounded. */
    if (h->on_ground && lh_input_jump())
    {
        h->vy = HERO_JUMP_VEL;
        h->on_ground = false;
    }

    /* Apply gravity (positive Y = down). */
    h->vy += HERO_GRAVITY * extrp;
    if (h->vy > HERO_MAX_FALL) h->vy = HERO_MAX_FALL;

    /* Integrate horizontal then resolve. */
    const double old_x = lh_transformable_get_x(&h->transformable);
    const double old_y = lh_transformable_get_y(&h->transformable);
    double new_x = old_x + h->vx * extrp;
    double new_y = old_y + h->vy * extrp;

    /* Crude tile collision: probe foot center for ground; probe head for
     * ceiling; probe sides at mid for walls. Phase 8+ swaps for the
     * formula-driven LionEngine TileCollidable pipeline. */
    if (map && tile_w > 0 && tile_h > 0)
    {
        /* Vertical resolve (foot probe). */
        const double foot_px = new_x + HERO_W * 0.5;
        const double foot_py = new_y + HERO_H;
        if (h->vy >= 0.0 && tile_solid(map, tile_w, tile_h, foot_px, foot_py))
        {
            const int tile_top = ((int)foot_py / tile_h) * tile_h;
            new_y = (double)(tile_top - HERO_H);
            h->vy = 0.0;
            h->on_ground = true;
        }
        else if (h->vy < 0.0 && tile_solid(map, tile_w, tile_h, foot_px, new_y))
        {
            const int tile_bot = ((int)new_y / tile_h + 1) * tile_h;
            new_y = (double)tile_bot;
            h->vy = 0.0;
        }
        else
        {
            h->on_ground = false;
        }

        /* Horizontal resolve (mid-side probes). */
        const double mid_y = new_y + HERO_H * 0.5;
        if (h->vx > 0.0 && tile_solid(map, tile_w, tile_h, new_x + HERO_W, mid_y))
        {
            const int tile_left = (((int)(new_x + HERO_W) / tile_w)) * tile_w;
            new_x = (double)(tile_left - HERO_W);
            h->vx = 0.0;
        }
        else if (h->vx < 0.0 && tile_solid(map, tile_w, tile_h, new_x, mid_y))
        {
            const int tile_right = ((int)new_x / tile_w + 1) * tile_w;
            new_x = (double)tile_right;
            h->vx = 0.0;
        }
    }

    lh_transformable_set_location(&h->transformable, new_x, new_y);

    /* State machine. */
    if (!h->on_ground)
        h->state = (h->vy < 0) ? LH_HERO_JUMP : LH_HERO_FALL;
    else
        h->state = (ax != 0.0) ? LH_HERO_WALK : LH_HERO_IDLE;

    /* Cheap 4-frame walk anim cycle. */
    if (h->state == LH_HERO_WALK)
    {
        if ((h->tick & 7) == 0) h->anim_frame = (h->anim_frame + 1) & 3;
    }
    else
    {
        h->anim_frame = 0;
    }
}

void lh_hero_render(const lh_hero* h, lh_graphic* g, const lh_camera* cam)
{
    if (!h || !g) return;

    const double cx = cam ? cam->x : 0.0;
    const double cy = cam ? cam->y : 0.0;
    const int    sx = (int)(lh_transformable_get_x(&h->transformable) - cx);
    const int    sy = (int)(lh_transformable_get_y(&h->transformable) - cy);

    /* Body rectangle, color tinted by state. */
    lh_color_rgba body;
    switch (h->state)
    {
        case LH_HERO_WALK: body = LH_RGB(255, 200, 0);   break;
        case LH_HERO_JUMP: body = LH_RGB(255, 100, 100); break;
        case LH_HERO_FALL: body = LH_RGB(200, 50,  50);  break;
        default:           body = LH_RGB(255, 255, 0);   break;
    }
    lh_graphic_set_color(g, body);
    lh_graphic_fill_rect(g, sx, sy, HERO_W, HERO_H);

    /* Eye dot — flips with facing. */
    lh_graphic_set_color(g, LH_RGB(0, 0, 0));
    const int eye_x = sx + (h->facing_left ? 3 : HERO_W - 5);
    lh_graphic_fill_rect(g, eye_x, sy + 5, 2, 2);

    /* Walk anim "feet" toggle. */
    if (h->state == LH_HERO_WALK && (h->anim_frame & 1))
    {
        lh_graphic_set_color(g, LH_RGB(0, 0, 0));
        lh_graphic_fill_rect(g, sx + 2,         sy + HERO_H - 2, 4, 2);
        lh_graphic_fill_rect(g, sx + HERO_W - 6, sy + HERO_H - 2, 4, 2);
    }
}

double lh_hero_get_x(const lh_hero* h) { return h ? lh_transformable_get_x(&h->transformable) : 0.0; }
double lh_hero_get_y(const lh_hero* h) { return h ? lh_transformable_get_y(&h->transformable) : 0.0; }
