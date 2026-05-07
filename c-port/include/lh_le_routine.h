/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.RoutineUpdate
 *   com.b3dgs.lionengine.game.feature.RoutineRender
 *
 * Features that participate in the per-frame update or render pass implement
 * one of these vtables. Handler discovers them by walking each Featurable's
 * features list and calling their lh_feature.vt fields.
 */
#ifndef LH_LE_ROUTINE_H
#define LH_LE_ROUTINE_H

#include "lh_le_feature.h"
#include "lh_le_graphic.h"

/* Update routine vtable. priority is consulted at sort time; lower runs
 * first within a single Featurable's feature list. update_before/after
 * mirror Java's bookkeeping hooks (defaults to no-op). */
typedef struct {
    int  priority;                                  /* sort key */
    void (*update_before)(lh_feature* f);           /* nullable */
    void (*update)       (lh_feature* f, double extrp);
    void (*update_after) (lh_feature* f);           /* nullable */
} lh_routine_update_vt;

typedef struct {
    lh_feature                 base;
    const lh_routine_update_vt* vt;
} lh_routine_update;

/* Render routine vtable. priority is sort key (lower = drawn first/back). */
typedef struct {
    int  priority;
    void (*render)(lh_feature* f, lh_graphic* g);
} lh_routine_render_vt;

typedef struct {
    lh_feature                 base;
    const lh_routine_render_vt* vt;
} lh_routine_render;

/* Helpers to fetch the routine vtable from a feature, or NULL if the feature
 * does not participate. Concrete features set their own routine_update/render
 * fields and provide the vtables; Handler invokes through these accessors.
 *
 * Convention: a feature can be a routine carrier if its lh_feature.id is one
 * of LH_FEAT_ROUTINE_UPDATE / LH_FEAT_ROUTINE_RENDER, OR if the concrete
 * feature embeds a lh_routine_update/render at a known offset. Phase 2c+
 * uses the explicit-id path; phase 8 (object features) may switch to embed.
 */

#endif
