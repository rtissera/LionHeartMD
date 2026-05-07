/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_rasterable.h"

static void rast_render(lh_feature* f, lh_graphic* g)
{
    lh_rasterable* r = (lh_rasterable*)f;
    if (!r->visible || !r->sprite || !r->transformable) return;

    const double x = lh_transformable_get_x(r->transformable) + r->frame_offset_x;
    const double y = lh_transformable_get_y(r->transformable) + r->frame_offset_y;
    lh_sprite_set_location(r->sprite, x, y);
    if (r->mirrorable)
    {
        const lh_mirror m = lh_mirrorable_get(r->mirrorable);
        lh_sprite_set_mirror(r->sprite,
                             m == LH_MIRROR_HORIZONTAL,
                             m == LH_MIRROR_VERTICAL);
    }
    lh_sprite_render(r->sprite, g);
}

static const lh_feature_render_vt s_rast_render_vt = {
    .priority = 0,
    .render   = rast_render,
};

void lh_rasterable_init(lh_rasterable* r, lh_sprite* sprite)
{
    lh_feature_init(&r->base, LH_FEAT_RASTERABLE, NULL);
    r->base.render_vt    = &s_rast_render_vt;
    r->sprite            = sprite;
    r->transformable     = NULL;
    r->mirrorable        = NULL;
    r->anim_offset       = 0;
    r->anim_offset2      = 0;
    r->frame_offset_x    = 0;
    r->frame_offset_y    = 0;
    r->visible           = true;
}

void lh_rasterable_bind(lh_rasterable* r,
                        const lh_transformable* tf,
                        const lh_mirrorable* mr)
{
    if (!r) return;
    r->transformable = tf;
    r->mirrorable    = mr;
}

void lh_rasterable_set_anim_offset (lh_rasterable* r, int o) { if (r) r->anim_offset  = o; }
void lh_rasterable_set_anim_offset2(lh_rasterable* r, int o) { if (r) r->anim_offset2 = o; }
void lh_rasterable_set_frame_offsets(lh_rasterable* r, int ox, int oy)
{
    if (!r) return;
    r->frame_offset_x = ox;
    r->frame_offset_y = oy;
}
void lh_rasterable_set_visible(lh_rasterable* r, bool v) { if (r) r->visible = v; }
int  lh_rasterable_get_anim_offset (const lh_rasterable* r) { return r ? r->anim_offset  : 0; }
int  lh_rasterable_get_anim_offset2(const lh_rasterable* r) { return r ? r->anim_offset2 : 0; }
