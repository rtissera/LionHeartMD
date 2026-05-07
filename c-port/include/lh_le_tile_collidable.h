/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.tile.map.collision.TileCollidable
 *   com.b3dgs.lionengine.game.feature.tile.map.collision.TileCollidableModel
 *   com.b3dgs.lionengine.game.feature.tile.map.collision.TileCollidableListener
 *
 * Tile-grid collision feature. Probes the active MapTileCollision (held in
 * Services under LH_SVC_MAP_TILE_COLLISION) using categories that describe
 * which directions/axes to test, then notifies registered listeners on
 * collision. Phase 2c skeleton — formula evaluation lands phase 7 World.
 */
#ifndef LH_LE_TILE_COLLIDABLE_H
#define LH_LE_TILE_COLLIDABLE_H

#include <stdbool.h>
#include "lh_le_feature.h"

#define LH_TILE_COLL_MAX_CATEGORIES 8
#define LH_TILE_COLL_MAX_LISTENERS  4

typedef enum {
    LH_AXIS_NONE = 0,
    LH_AXIS_X,
    LH_AXIS_Y,
} lh_axis;

typedef struct {
    char    name[32];
    lh_axis axis;
    int     offset_x;
    int     offset_y;
    int     glue;
    bool    forced;
} lh_collision_category;

typedef struct {
    bool    hit;
    double  x, y;
    int     formula_index;
    char    formula_start[32];
    char    formula_end[32];
} lh_collision_result;

typedef void (*lh_tile_collidable_listener)(lh_feature* self,
                                            const lh_collision_result* res,
                                            const lh_collision_category* cat);

typedef struct {
    lh_feature                  base;
    lh_collision_category       categories[LH_TILE_COLL_MAX_CATEGORIES];
    int                         category_count;
    lh_tile_collidable_listener listeners[LH_TILE_COLL_MAX_LISTENERS];
    int                         listener_count;
    bool                        enabled;
} lh_tile_collidable;

void lh_tile_collidable_init       (lh_tile_collidable* tc);
void lh_tile_collidable_set_enabled(lh_tile_collidable* tc, bool e);
void lh_tile_collidable_add_category(lh_tile_collidable* tc, const lh_collision_category* cat);
void lh_tile_collidable_add_listener(lh_tile_collidable* tc, lh_tile_collidable_listener l);
void lh_tile_collidable_notify     (lh_tile_collidable* tc,
                                    const lh_collision_result* res,
                                    const lh_collision_category* cat);

#endif
