/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.Layerable.
 *
 * Layer assignment for ordered update + render. Lower layer_refresh runs
 * first in update; lower layer_display draws first (back-to-front).
 */
#ifndef LH_LE_LAYERABLE_H
#define LH_LE_LAYERABLE_H

#include "lh_le_feature.h"

typedef struct {
    lh_feature base;
    int        layer_refresh;
    int        layer_display;
} lh_layerable;

void lh_layerable_init        (lh_layerable* l, int refresh, int display);
int  lh_layerable_get_refresh (const lh_layerable* l);
int  lh_layerable_get_display (const lh_layerable* l);
void lh_layerable_set_layer   (lh_layerable* l, int refresh, int display);

#endif
