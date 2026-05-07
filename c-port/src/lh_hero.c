/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_hero.h"

#include "lh_input.h"
#include "lh_le_color.h"

#define HERO_W           20    /* matches Valdyn idle hitbox */
#define HERO_H           48    /* matches Valdyn idle hitbox (close) */
#define HERO_WALK_SPEED  2.0
#define HERO_JUMP_VEL    -5.5
#define HERO_GRAVITY     0.35
#define HERO_MAX_FALL    8.0

#define HERO_WALK_TICKS_PER_FRAME 4   /* sprite cycle pace */

/* Valdyn sprite-sheet frame numbers (1-based, see Valdyn.xml). */
#define HERO_FRAME_IDLE       1
#define HERO_FRAME_WALK_FIRST 26
#define HERO_FRAME_WALK_LAST  35
#define HERO_FRAME_JUMP_FIRST 17
#define HERO_FRAME_JUMP_LAST  19
#define HERO_FRAME_FALL       20

static bool tile_solid(const lh_map_tile* m, int tile_w, int tile_h, double px, double py)
{
    if (!m || tile_w <= 0 || tile_h <= 0) return false;
    return lh_map_tile_get_tile_at_pixel(m, (int)px, (int)py) >= 0;
}

/* Probe at three x positions along an edge (left/center/right). */
static bool tile_solid_3(const lh_map_tile* m, int tile_w, int tile_h,
                         double px_left, double px_right, double py)
{
    return tile_solid(m, tile_w, tile_h, px_left,  py)
        || tile_solid(m, tile_w, tile_h, (px_left + px_right) * 0.5, py)
        || tile_solid(m, tile_w, tile_h, px_right, py);
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
    h->anim_frame  = HERO_FRAME_IDLE;
    h->walk_idx    = 0;
    h->sprite      = NULL;
    h->sprite_cols = 0;
    h->sprite_rows = 0;
    h->frame_w     = 0;
    h->frame_h     = 0;
}

void lh_hero_load_sprite(lh_hero* h, lh_image_buffer* atlas, int cols, int rows)
{
    if (!h || !atlas || cols <= 0 || rows <= 0) return;
    h->sprite      = atlas;
    h->sprite_cols = cols;
    h->sprite_rows = rows;
    h->frame_w     = atlas->width  / cols;
    h->frame_h     = atlas->height / rows;
}

static int hero_pick_frame(const lh_hero* h)
{
    switch (h->state)
    {
        case LH_HERO_WALK:
        {
            const int span = HERO_FRAME_WALK_LAST - HERO_FRAME_WALK_FIRST + 1;
            return HERO_FRAME_WALK_FIRST + (h->walk_idx % span);
        }
        case LH_HERO_JUMP:
        {
            const int span = HERO_FRAME_JUMP_LAST - HERO_FRAME_JUMP_FIRST + 1;
            const int idx  = (h->walk_idx / 2) % span;
            return HERO_FRAME_JUMP_FIRST + idx;
        }
        case LH_HERO_FALL:
            return HERO_FRAME_FALL;
        case LH_HERO_IDLE:
        default:
            return HERO_FRAME_IDLE;
    }
}

void lh_hero_update(lh_hero* h, const lh_map_tile* map, int tile_w, int tile_h, double extrp)
{
    if (!h) return;
    h->tick++;

    const double ax = lh_input_axis_h();
    h->vx = ax * HERO_WALK_SPEED;
    if (ax < 0) h->facing_left = true;
    else if (ax > 0) h->facing_left = false;

    if (h->on_ground && lh_input_jump())
    {
        h->vy = HERO_JUMP_VEL;
        h->on_ground = false;
    }

    h->vy += HERO_GRAVITY * extrp;
    if (h->vy > HERO_MAX_FALL) h->vy = HERO_MAX_FALL;

    const double old_x = lh_transformable_get_x(&h->transformable);
    const double old_y = lh_transformable_get_y(&h->transformable);
    double new_x = old_x + h->vx * extrp;
    double new_y = old_y + h->vy * extrp;

    if (map && tile_w > 0 && tile_h > 0)
    {
        /* Vertical resolve — multi-probe along foot line. */
        const double foot_l = new_x + 2.0;
        const double foot_r = new_x + HERO_W - 2.0;
        const double foot_y = new_y + HERO_H;
        if (h->vy >= 0.0 && tile_solid_3(map, tile_w, tile_h, foot_l, foot_r, foot_y))
        {
            const int tile_top = ((int)foot_y / tile_h) * tile_h;
            new_y = (double)(tile_top - HERO_H);
            h->vy = 0.0;
            h->on_ground = true;
        }
        else if (h->vy < 0.0 && tile_solid_3(map, tile_w, tile_h, foot_l, foot_r, new_y))
        {
            const int tile_bot = ((int)new_y / tile_h + 1) * tile_h;
            new_y = (double)tile_bot;
            h->vy = 0.0;
        }
        else
        {
            h->on_ground = false;
        }

        /* Horizontal resolve — probe at three vertical points (foot/mid/head). */
        const double mid_y_top = new_y + 4.0;
        const double mid_y_mid = new_y + HERO_H * 0.5;
        const double mid_y_bot = new_y + HERO_H - 4.0;
        if (h->vx > 0.0
            && (tile_solid(map, tile_w, tile_h, new_x + HERO_W, mid_y_top)
             || tile_solid(map, tile_w, tile_h, new_x + HERO_W, mid_y_mid)
             || tile_solid(map, tile_w, tile_h, new_x + HERO_W, mid_y_bot)))
        {
            const int tile_left = (((int)(new_x + HERO_W) / tile_w)) * tile_w;
            new_x = (double)(tile_left - HERO_W);
            h->vx = 0.0;
        }
        else if (h->vx < 0.0
            && (tile_solid(map, tile_w, tile_h, new_x, mid_y_top)
             || tile_solid(map, tile_w, tile_h, new_x, mid_y_mid)
             || tile_solid(map, tile_w, tile_h, new_x, mid_y_bot)))
        {
            const int tile_right = ((int)new_x / tile_w + 1) * tile_w;
            new_x = (double)tile_right;
            h->vx = 0.0;
        }
    }

    lh_transformable_set_location(&h->transformable, new_x, new_y);

    if (!h->on_ground)
        h->state = (h->vy < 0) ? LH_HERO_JUMP : LH_HERO_FALL;
    else
        h->state = (ax != 0.0) ? LH_HERO_WALK : LH_HERO_IDLE;

    /* Walk anim cycle. */
    if ((h->state == LH_HERO_WALK || h->state == LH_HERO_JUMP)
        && (h->tick % HERO_WALK_TICKS_PER_FRAME) == 0)
    {
        h->walk_idx++;
    }
    else if (h->state == LH_HERO_IDLE || h->state == LH_HERO_FALL)
    {
        h->walk_idx = 0;
    }

    h->anim_frame = hero_pick_frame(h);
}

void lh_hero_render(const lh_hero* h, lh_graphic* g, const lh_camera* cam)
{
    if (!h || !g) return;

    const double cx = cam ? cam->x : 0.0;
    const double cy = cam ? cam->y : 0.0;

    if (h->sprite && h->frame_w > 0 && h->frame_h > 0 && h->sprite_cols > 0)
    {
        /* Frame index is 1-based; 0..count-1 in-grid. */
        const int idx = h->anim_frame > 0 ? h->anim_frame - 1 : 0;
        const int sx_grid = (idx % h->sprite_cols) * h->frame_w;
        const int sy_grid = (idx / h->sprite_cols) * h->frame_h;

        /* Origin: CENTER_BOTTOM — sprite bottom = hero foot. */
        const int dst_x = (int)(lh_transformable_get_x(&h->transformable) - cx)
                        + HERO_W / 2 - h->frame_w / 2;
        const int dst_y = (int)(lh_transformable_get_y(&h->transformable) - cy)
                        + HERO_H - h->frame_h;

        /* facing_left maps to horizontal flip. */
        lh_graphic_draw_image_region_flip(g, h->sprite,
                                            sx_grid, sy_grid,
                                            h->frame_w, h->frame_h,
                                            dst_x, dst_y, h->facing_left, false);
        return;
    }

    /* Placeholder rect fallback if sprite not loaded. */
    const int sx = (int)(lh_transformable_get_x(&h->transformable) - cx);
    const int sy = (int)(lh_transformable_get_y(&h->transformable) - cy);
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
    lh_graphic_set_color(g, LH_RGB(0, 0, 0));
    const int eye_x = sx + (h->facing_left ? 3 : HERO_W - 5);
    lh_graphic_fill_rect(g, eye_x, sy + 5, 2, 2);
}

double lh_hero_get_x(const lh_hero* h) { return h ? lh_transformable_get_x(&h->transformable) : 0.0; }
double lh_hero_get_y(const lh_hero* h) { return h ? lh_transformable_get_y(&h->transformable) : 0.0; }
