/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_layerable.h"

void lh_layerable_init(lh_layerable* l, int refresh, int display)
{
    lh_feature_init(&l->base, LH_FEAT_LAYERABLE, NULL);
    l->layer_refresh = refresh;
    l->layer_display = display;
}

int lh_layerable_get_refresh(const lh_layerable* l) { return l ? l->layer_refresh : 0; }
int lh_layerable_get_display(const lh_layerable* l) { return l ? l->layer_display : 0; }

void lh_layerable_set_layer(lh_layerable* l, int refresh, int display)
{
    if (!l) return;
    l->layer_refresh = refresh;
    l->layer_display = display;
}
