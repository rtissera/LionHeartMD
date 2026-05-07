/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_tile_collidable.h"
#include <string.h>

void lh_tile_collidable_init(lh_tile_collidable* tc)
{
    lh_feature_init(&tc->base, LH_FEAT_TILE_COLLIDABLE, NULL);
    tc->category_count = 0;
    tc->listener_count = 0;
    tc->enabled        = true;
    memset(tc->categories, 0, sizeof(tc->categories));
    memset(tc->listeners,  0, sizeof(tc->listeners));
}

void lh_tile_collidable_set_enabled(lh_tile_collidable* tc, bool e)
{
    if (tc) tc->enabled = e;
}

void lh_tile_collidable_add_category(lh_tile_collidable* tc, const lh_collision_category* cat)
{
    if (!tc || !cat || tc->category_count >= LH_TILE_COLL_MAX_CATEGORIES) return;
    tc->categories[tc->category_count++] = *cat;
}

void lh_tile_collidable_add_listener(lh_tile_collidable* tc, lh_tile_collidable_listener l)
{
    if (!tc || !l || tc->listener_count >= LH_TILE_COLL_MAX_LISTENERS) return;
    tc->listeners[tc->listener_count++] = l;
}

void lh_tile_collidable_notify(lh_tile_collidable* tc,
                               const lh_collision_result* res,
                               const lh_collision_category* cat)
{
    if (!tc) return;
    for (int i = 0; i < tc->listener_count; i++)
    {
        tc->listeners[i](&tc->base, res, cat);
    }
}
