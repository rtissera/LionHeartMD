/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.tile.TileGroupsConfig
 *      (and inlined TileConfig).
 *
 * Maps a logical group name to the set of tile numbers that belong to
 * the group. The on-disk file is named groups.xml — under each level
 * theme folder (level/<theme>/groups.xml) — and shares its grammar
 * with neither formulas.xml nor collisions.xml.
 *
 * Grammar:
 *   <lionengine:groups>
 *     <lionengine:group name="ground" type="NONE">
 *       <lionengine:tile number="4"/>
 *       <lionengine:tile number="5"/>
 *       ...
 *     </lionengine:group>
 *   </lionengine:groups>
 *
 * The Java engine stores tile numbers as a HashSet<Integer>; here we
 * store contiguous tile-number arrays per group plus, for the range
 * accessor described in the task brief, a flat (group, first, last)
 * tuple list collapsed from runs of consecutive numbers.
 *
 * No (sheet_id) is encoded in this XML: it predates multi-sheet maps
 * — Lionheart uses a single sheet, so sheet_id is fixed to 0 in the
 * tuple view.
 */
#ifndef LH_TILE_GROUPS_CONFIG_H
#define LH_TILE_GROUPS_CONFIG_H

#include "lh_xml.h"

#define LH_TILE_GROUPS_FILENAME            "groups.xml"

#define LH_TILE_GROUPS_NODE_ROOT           "lionengine:groups"
#define LH_TILE_GROUPS_NODE_GROUP          "lionengine:group"
#define LH_TILE_GROUPS_NODE_TILE           "lionengine:tile"

#define LH_TILE_GROUPS_MAX                 64
#define LH_TILE_GROUP_NAME_MAX             48
#define LH_TILE_GROUP_TYPE_MAX             16
#define LH_TILES_PER_GROUP_MAX             256
#define LH_TILE_GROUP_RANGES_MAX           1024

typedef struct {
    char name [LH_TILE_GROUP_NAME_MAX];
    char type [LH_TILE_GROUP_TYPE_MAX]; /* "NONE", "TRANSITION", ... verbatim */
    int  tiles [LH_TILES_PER_GROUP_MAX];
    int  tile_count;
} lh_tile_group;

/* Compact "(group_name, sheet_id, first_tile, last_tile)" tuple, the
 * shape requested in the task brief. Built from collapsing each
 * group's sorted tile list into runs of consecutive numbers. */
typedef struct {
    char group_name [LH_TILE_GROUP_NAME_MAX];
    int  sheet_id;     /* always 0 — Lionheart uses a single sheet */
    int  first_tile;
    int  last_tile;
} lh_tile_group_range;

typedef struct {
    lh_tile_group       groups [LH_TILE_GROUPS_MAX];
    int                 group_count;
    lh_tile_group_range ranges [LH_TILE_GROUP_RANGES_MAX];
    int                 range_count;
} lh_tile_groups_config;

void                 lh_tile_groups_config_init    (lh_tile_groups_config* cfg);
bool                 lh_tile_groups_config_load    (lh_tile_groups_config* cfg,
                                                    const char* media);
const lh_tile_group* lh_tile_groups_config_get     (const lh_tile_groups_config* cfg,
                                                    const char* group_name);
const char*          lh_tile_groups_config_find_for(const lh_tile_groups_config* cfg,
                                                    int tile_number);

#endif
