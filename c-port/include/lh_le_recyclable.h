/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.Recyclable.
 *
 * Marker interface in Java; here the contract lives in lh_feature_vt.recycle.
 * Pooled featurables (e.g. projectiles, enemies) are recycled into a clean
 * state instead of freshly allocated. lh_featurable_recycle() walks the
 * feature list and invokes vt->recycle for each.
 */
#ifndef LH_LE_RECYCLABLE_H
#define LH_LE_RECYCLABLE_H

#include "lh_le_feature.h"
#include "lh_le_featurable.h"

/* Convenience: walk all features and invoke recycle. Equivalent to
 * lh_featurable_recycle but keeps the call site self-documenting. */
static inline void lh_recyclable_recycle_all(lh_featurable* f)
{
    lh_featurable_recycle(f);
}

#endif
