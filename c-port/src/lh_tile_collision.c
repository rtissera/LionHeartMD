/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_tile_collision.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

/* Find the group name for a (sheet_id, tile_num) pair. Returns NULL when
 * the tile has no group entry. */
static const char* group_for_tile(const lh_tile_groups_config* tg,
                                  int sheet_id, int tile_num)
{
    if (!tg) return NULL;
    for (int i = 0; i < tg->range_count; i++)
    {
        const lh_tile_group_range* r = &tg->ranges[i];
        if (r->sheet_id == sheet_id
            && tile_num >= r->first_tile
            && tile_num <= r->last_tile)
        {
            return r->group_name;
        }
    }
    return NULL;
}

static const lh_coll_group* coll_group_lookup(const lh_collision_group_config* gc,
                                              const char* name)
{
    if (!gc || !name) return NULL;
    for (int i = 0; i < gc->count; i++)
    {
        if (strcmp(gc->entries[i].name, name) == 0) return &gc->entries[i];
    }
    return NULL;
}

/* Evaluate a single formula at tile-local (lx, ly). Returns true if the
 * point is on the solid side of the formula's line. */
static bool eval_formula(const lh_coll_formula* f, int lx, int ly, int tile_w, int tile_h)
{
    if (!f) return false;
    const lh_coll_range* r = &f->range;

    if (f->range.axis == LH_COLL_AXIS_Y)
    {
        if (lx < r->min_x || lx > r->max_x) return false;
        const double y_bound = (f->function.type == LH_COLL_FN_LINEAR)
                               ? f->function.a * (double)lx + f->function.b
                               : 0.0;
        /* Lionheart convention: the LINEAR Y bound describes the top
         * surface of the solid; pixels at or below the line are inside
         * the tile material. tile_local_y is measured from the tile top. */
        return (double)ly >= y_bound;
    }
    if (f->range.axis == LH_COLL_AXIS_X)
    {
        if (ly < r->min_y || ly > r->max_y) return false;
        const double x_bound = (f->function.type == LH_COLL_FN_LINEAR)
                               ? f->function.a * (double)ly + f->function.b
                               : 0.0;
        /* X-bound semantics depend on orientation we don't have yet —
         * approximate as "either side past the line is solid", which
         * over-approximates walls. Phase 8 wires the constraint axis. */
        (void)x_bound; (void)tile_w; (void)tile_h;
        return true;
    }
    return false;
}

bool lh_tile_collision_solid_at(const lh_tile_collision_ctx* c,
                                double world_x, double world_y)
{
    if (!c || !c->map || c->tile_w <= 0 || c->tile_h <= 0) return false;
    if (world_x < 0 || world_y < 0) return false;

    const int32_t packed = lh_map_tile_get_tile_at_pixel(c->map,
                                                         (int)world_x,
                                                         (int)world_y);
    if (packed < 0) return false;
    const int sheet = (int)((uint32_t)packed >> 16);
    const int tile  = (int)((uint32_t)packed & 0xFFFFu);

    /* If we don't have group/formula data loaded, fall back to "any
     * non-empty tile is solid" — matches the pre-formula behavior. */
    if (!c->tile_groups || !c->coll_groups || !c->formulas
        || c->tile_groups->range_count == 0
        || c->coll_groups->count       == 0
        || c->formulas->count          == 0)
    {
        return true;
    }

    const char* group_name = group_for_tile(c->tile_groups, sheet, tile);
    if (!group_name) return false;       /* tile has no collision group */

    const lh_coll_group* g = coll_group_lookup(c->coll_groups, group_name);
    if (!g || g->formula_count == 0) return false;

    const int lx = ((int)world_x) % c->tile_w;
    const int ly = ((int)world_y) % c->tile_h;

    for (int i = 0; i < g->formula_count; i++)
    {
        const lh_coll_formula* f = lh_collision_formula_config_get(c->formulas,
                                                                   g->formulas[i]);
        if (eval_formula(f, lx, ly, c->tile_w, c->tile_h)) return true;
    }
    return false;
}

bool lh_tile_collision_solid_3(const lh_tile_collision_ctx* c,
                               double x_a, double x_b, double y)
{
    return lh_tile_collision_solid_at(c, x_a, y)
        || lh_tile_collision_solid_at(c, (x_a + x_b) * 0.5, y)
        || lh_tile_collision_solid_at(c, x_b, y);
}
