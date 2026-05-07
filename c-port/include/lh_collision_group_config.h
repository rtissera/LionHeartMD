/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.tile.map.collision
 *      .CollisionGroupConfig.
 *
 * Parses collisions.xml of the form:
 *
 *   <lionengine:collisions>
 *     <lionengine:collision group="ground">
 *       <lionengine:formula>ground</lionengine:formula>
 *       <lionengine:formula>ground15</lionengine:formula>
 *     </lionengine:collision>
 *   </lionengine:collisions>
 *
 * Each <lionengine:collision> binds a logical group name to one or more
 * formula names defined in formulas.xml.
 */
#ifndef LH_COLLISION_GROUP_CONFIG_H
#define LH_COLLISION_GROUP_CONFIG_H

#include "lh_xml.h"

#define LH_COLL_GROUPS_FILENAME           "collisions.xml"

#define LH_COLL_GROUPS_NODE_ROOT          "lionengine:collisions"
#define LH_COLL_GROUPS_NODE_COLLISION     "lionengine:collision"
#define LH_COLL_GROUPS_NODE_FORMULA_REF   "lionengine:formula"

#define LH_COLL_MAX_GROUPS                32
#define LH_COLL_MAX_FORMULAS_PER_GROUP    16
#define LH_COLL_GROUP_NAME_MAX            48
#define LH_COLL_FORMULA_REF_NAME_MAX      48

typedef struct {
    char name [LH_COLL_GROUP_NAME_MAX];
    char formulas [LH_COLL_MAX_FORMULAS_PER_GROUP][LH_COLL_FORMULA_REF_NAME_MAX];
    int  formula_count;
} lh_coll_group;

typedef struct {
    lh_coll_group entries [LH_COLL_MAX_GROUPS];
    int           count;
} lh_collision_group_config;

void                  lh_collision_group_config_init(lh_collision_group_config* cfg);
bool                  lh_collision_group_config_load(lh_collision_group_config* cfg,
                                                     const char* media);
const lh_coll_group*  lh_collision_group_config_get (const lh_collision_group_config* cfg,
                                                     const char* name);

#endif
