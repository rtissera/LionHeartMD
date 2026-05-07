/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.Mirrorable
 *   com.b3dgs.lionengine.game.feature.MirrorableModel
 *   com.b3dgs.lionengine.Mirror
 *
 * Tracks current + next mirror state. update() applies the pending mirror
 * change so renderers always see a stable orientation per frame.
 */
#ifndef LH_LE_MIRRORABLE_H
#define LH_LE_MIRRORABLE_H

#include <stdbool.h>
#include "lh_le_feature.h"

typedef enum {
    LH_MIRROR_NONE = 0,
    LH_MIRROR_HORIZONTAL,
    LH_MIRROR_VERTICAL
} lh_mirror;

typedef struct {
    lh_feature base;
    lh_mirror  current;
    lh_mirror  next;
} lh_mirrorable;

void      lh_mirrorable_init   (lh_mirrorable* m);
void      lh_mirrorable_set    (lh_mirrorable* m, lh_mirror next);
lh_mirror lh_mirrorable_get    (const lh_mirrorable* m);
bool      lh_mirrorable_is     (const lh_mirrorable* m, lh_mirror q);
void      lh_mirrorable_update (lh_mirrorable* m, double extrp);

#endif
