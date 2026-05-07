/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.tile.map.collision
 *      .CollisionFormulaConfig (and inlined CollisionRangeConfig +
 *      CollisionFunctionConfig — only LINEAR is used by Lionheart).
 *
 * Parses formulas.xml of the form:
 *
 *   <lionengine:formulas>
 *     <lionengine:formula name="ground">
 *       <lionengine:range  output="Y" minX="0" maxX="16" minY="0" maxY="0"/>
 *       <lionengine:function type="LINEAR" a="0.0" b="0.0"/>
 *       <lionengine:constraint orientation="..." group="..."/>
 *       ...
 *     </lionengine:formula>
 *   </lionengine:formulas>
 *
 * No reflection: function type and axis are stored as enums; all
 * constraint orientations are kept verbatim as strings.
 */
#ifndef LH_COLLISION_FORMULA_CONFIG_H
#define LH_COLLISION_FORMULA_CONFIG_H

#include "lh_xml.h"

#define LH_COLL_FORMULAS_FILENAME      "formulas.xml"

#define LH_COLL_FORMULA_NODE_ROOT      "lionengine:formulas"
#define LH_COLL_FORMULA_NODE_FORMULA   "lionengine:formula"
#define LH_COLL_FORMULA_NODE_RANGE     "lionengine:range"
#define LH_COLL_FORMULA_NODE_FUNCTION  "lionengine:function"

#define LH_COLL_MAX_FORMULAS           64
#define LH_COLL_FORMULA_NAME_MAX       48

typedef enum {
    LH_COLL_AXIS_X = 0,
    LH_COLL_AXIS_Y = 1
} lh_coll_axis;

typedef enum {
    LH_COLL_FN_NONE   = 0,
    LH_COLL_FN_LINEAR = 1
} lh_coll_function_type;

typedef struct {
    lh_coll_axis axis;     /* output axis */
    int          min_x;
    int          max_x;
    int          min_y;
    int          max_y;
} lh_coll_range;

typedef struct {
    lh_coll_function_type type;
    double a;              /* slope (LINEAR) */
    double b;              /* offset (LINEAR) */
} lh_coll_function;

typedef struct {
    char             name [LH_COLL_FORMULA_NAME_MAX];
    lh_coll_range    range;
    lh_coll_function function;
} lh_coll_formula;

typedef struct {
    lh_coll_formula entries [LH_COLL_MAX_FORMULAS];
    int             count;
} lh_collision_formula_config;

void                    lh_collision_formula_config_init(lh_collision_formula_config* cfg);
bool                    lh_collision_formula_config_load(lh_collision_formula_config* cfg,
                                                         const char* media);
const lh_coll_formula*  lh_collision_formula_config_get (const lh_collision_formula_config* cfg,
                                                         const char* name);

#endif
