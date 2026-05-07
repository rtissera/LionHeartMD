/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Component system port of:
 *   com.b3dgs.lionengine.game.feature.Feature
 *   com.b3dgs.lionengine.game.feature.FeatureModel
 *   com.b3dgs.lionengine.game.feature.FeatureProvider
 *
 * Java's Class<? extends Feature> dispatch becomes a closed-world enum
 * (lh_feature_id). Each feature implementation registers under one or
 * more feature IDs in a Featurable container.
 */
#ifndef LH_LE_FEATURE_H
#define LH_LE_FEATURE_H

#include <stdbool.h>

/* Closed-world feature type IDs.
 * Add new IDs here as port progresses; one entry per Java @FeatureInterface.
 * Codegen target: gen_dispatch.py reads java feature sources and emits
 * an authoritative list. For now the subset Lionheart Loading/Menu need. */
typedef enum {
    LH_FEAT_NONE = 0,
    LH_FEAT_IDENTIFIABLE,
    LH_FEAT_LAYERABLE,
    LH_FEAT_RECYCLABLE,
    LH_FEAT_TRANSFORMABLE,
    LH_FEAT_MIRRORABLE,
    LH_FEAT_ANIMATABLE,
    LH_FEAT_BODY,
    LH_FEAT_RASTERABLE,
    LH_FEAT_COLLIDABLE,
    LH_FEAT_TILE_COLLIDABLE,
    LH_FEAT_STATE_HANDLER,
    LH_FEAT_REFRESHABLE,
    LH_FEAT_DISPLAYABLE,
    LH_FEAT_ROUTINE_UPDATE,
    LH_FEAT_ROUTINE_RENDER,
    LH_FEAT_ENTITY_CHECKER,
    /* Reserve up to 80; phase 4+ asset pipeline will populate the full list. */
    LH_FEAT_MAX = 80
} lh_feature_id;

#include "lh_le_graphic.h"

typedef struct lh_featurable lh_featurable;
typedef struct lh_feature    lh_feature;

/* Feature lifecycle vtable. All optional — set to NULL when not used.
 * Maps to Java's prepare()/checkListener()/recycle() base methods. */
typedef struct {
    void (*prepare)        (lh_feature* f, lh_featurable* owner);
    void (*recycle)        (lh_feature* f);
    void (*destroy)        (lh_feature* f);
} lh_feature_vt;

/* Optional update participation. Concrete feature sets this field at init
 * iff it implements RoutineUpdate (Java parlance). NULL otherwise. */
typedef struct {
    int  priority;
    void (*update_before)(lh_feature* f);            /* nullable */
    void (*update)       (lh_feature* f, double extrp);
    void (*update_after) (lh_feature* f);            /* nullable */
} lh_feature_update_vt;

/* Optional render participation. NULL otherwise. */
typedef struct {
    int  priority;
    void (*render)(lh_feature* f, lh_graphic* g);
} lh_feature_render_vt;

/* Feature base. Concrete features embed this as first field. */
struct lh_feature {
    const lh_feature_vt*        vt;            /* nullable lifecycle vt */
    const lh_feature_update_vt* update_vt;     /* nullable RoutineUpdate */
    const lh_feature_render_vt* render_vt;     /* nullable RoutineRender */
    lh_featurable*              owner;         /* set by add_feature */
    lh_feature_id               id;            /* primary type key */
    /* Concrete feature fields follow in derived structs. */
};

/* Helper: initialize feature base. */
static inline void lh_feature_init(lh_feature* f, lh_feature_id id, const lh_feature_vt* vt)
{
    f->vt        = vt;
    f->update_vt = (lh_feature_update_vt*)0;
    f->render_vt = (lh_feature_render_vt*)0;
    f->owner     = (lh_featurable*)0;
    f->id        = id;
}

#endif
