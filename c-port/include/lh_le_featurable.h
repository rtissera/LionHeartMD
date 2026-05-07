/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.Featurable
 *   com.b3dgs.lionengine.game.feature.FeaturableAbstract
 *   com.b3dgs.lionengine.game.feature.FeaturableModel
 *
 * Container for feature components. lh_featurable owns an array of
 * lh_feature pointers indexed by lh_feature_id. add_feature stores
 * under primary id; get_feature is O(1) array lookup.
 */
#ifndef LH_LE_FEATURABLE_H
#define LH_LE_FEATURABLE_H

#include <stdbool.h>
#include "lh_le_feature.h"

struct lh_featurable {
    /* Array of features, indexed by lh_feature_id. NULL slot = absent. */
    lh_feature* features[LH_FEAT_MAX];
    /* Owner-controlled user pointer; concrete entity classes hang their
     * own state here (e.g. setup pointer, type tag, debug name). */
    void* user;
};

void lh_featurable_init   (lh_featurable* f);
void lh_featurable_destroy(lh_featurable* f);

/* Register feature under its primary id. The featurable does not own the
 * feature memory — caller manages allocation. */
lh_feature* lh_featurable_add_feature(lh_featurable* f, lh_feature* feat);

/* Lookup. Returns NULL if absent. */
lh_feature* lh_featurable_get_feature(const lh_featurable* f, lh_feature_id id);
bool        lh_featurable_has_feature(const lh_featurable* f, lh_feature_id id);

/* Lifecycle: call prepare on all features after all added. */
void lh_featurable_prepare(lh_featurable* f);
/* Call recycle on all features (used when respawning pooled entities). */
void lh_featurable_recycle(lh_featurable* f);

#endif
