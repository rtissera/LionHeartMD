/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.DisplayableModel
 *   com.b3dgs.lionengine.game.feature.RefreshableModel
 *
 * Lightweight feature carrier for an inline render/update callback. Used
 * in entity ctor blocks where Java has `addFeature(new DisplayableModel(g -> ...))`.
 * In C, point cb to a function and stash any state in user_data.
 */
#ifndef LH_LE_DISPLAYABLE_H
#define LH_LE_DISPLAYABLE_H

#include "lh_le_feature.h"
#include "lh_le_graphic.h"

typedef void (*lh_render_cb)(void* user_data, lh_graphic* g);
typedef void (*lh_update_cb)(void* user_data, double extrp);

typedef struct {
    lh_feature   base;
    lh_render_cb cb;
    void*        user_data;
} lh_displayable;

typedef struct {
    lh_feature   base;
    lh_update_cb cb;
    void*        user_data;
} lh_refreshable;

void lh_displayable_init(lh_displayable* d, lh_render_cb cb, void* user_data);
void lh_refreshable_init(lh_refreshable* r, lh_update_cb cb, void* user_data);

#endif
