/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_displayable.h"

static void disp_render(lh_feature* f, lh_graphic* g)
{
    lh_displayable* d = (lh_displayable*)f;
    if (d->cb) d->cb(d->user_data, g);
}

static const lh_feature_render_vt s_disp_render_vt = {
    .priority = 0,
    .render   = disp_render,
};

static void refr_update(lh_feature* f, double extrp)
{
    lh_refreshable* r = (lh_refreshable*)f;
    if (r->cb) r->cb(r->user_data, extrp);
}

static const lh_feature_update_vt s_refr_update_vt = {
    .priority      = 0,
    .update_before = NULL,
    .update        = refr_update,
    .update_after  = NULL,
};

void lh_displayable_init(lh_displayable* d, lh_render_cb cb, void* user_data)
{
    lh_feature_init(&d->base, LH_FEAT_DISPLAYABLE, NULL);
    d->base.render_vt = &s_disp_render_vt;
    d->cb        = cb;
    d->user_data = user_data;
}

void lh_refreshable_init(lh_refreshable* r, lh_update_cb cb, void* user_data)
{
    lh_feature_init(&r->base, LH_FEAT_REFRESHABLE, NULL);
    r->base.update_vt = &s_refr_update_vt;
    r->cb        = cb;
    r->user_data = user_data;
}
