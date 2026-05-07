/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Tile-level formula-driven collision query. Combines:
 *   lh_tile_groups_config       — maps (sheet, tile_num) -> group name
 *   lh_collision_group_config   — maps group name        -> [formula refs]
 *   lh_collision_formula_config — maps formula name      -> range + LINEAR
 *
 * Public API: lh_tile_collision_solid_at(world_x, world_y) returns true
 * iff the tile at that pixel is solid (or partially solid via a slope/
 * incline formula whose line covers the query point).
 *
 * Phase 7+: Y-axis ground/slope formulas only. X-axis (vertical wall)
 * formulas treated as full-tile solid for now — phase 8 will read
 * formula constraints to disambiguate orientation.
 */
#ifndef LH_TILE_COLLISION_H
#define LH_TILE_COLLISION_H

#include <stdbool.h>

#include "lh_collision_formula_config.h"
#include "lh_collision_group_config.h"
#include "lh_le_map_tile.h"
#include "lh_tile_groups_config.h"

typedef struct {
    const lh_map_tile*                  map;
    const lh_tile_groups_config*        tile_groups;
    const lh_collision_group_config*    coll_groups;
    const lh_collision_formula_config*  formulas;
    int                                 tile_w;
    int                                 tile_h;
} lh_tile_collision_ctx;

bool lh_tile_collision_solid_at(const lh_tile_collision_ctx* c,
                                double world_x, double world_y);

/* Probe along an edge: returns true if any of three points is solid. */
bool lh_tile_collision_solid_3 (const lh_tile_collision_ctx* c,
                                double x_a, double x_b, double y);

#endif
