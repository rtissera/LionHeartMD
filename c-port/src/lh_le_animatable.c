/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_animatable.h"

static void anim_update(lh_feature* f, double extrp)
{
    lh_animatable* a = (lh_animatable*)f;
    lh_animator_update(&a->animator, extrp);
}

static const lh_feature_update_vt s_anim_update_vt = {
    .priority      = 0,
    .update_before = NULL,
    .update        = anim_update,
    .update_after  = NULL,
};

void lh_animatable_init(lh_animatable* a)
{
    lh_feature_init(&a->base, LH_FEAT_ANIMATABLE, NULL);
    a->base.update_vt = &s_anim_update_vt;
    lh_animator_init(&a->animator);
}

void lh_animatable_play(lh_animatable* a, const lh_animation* anim)
{
    if (a) lh_animator_play(&a->animator, anim);
}

void lh_animatable_pause(lh_animatable* a)
{
    if (a) lh_animator_stop(&a->animator);
}

void lh_animatable_set_frame(lh_animatable* a, int frame)
{
    if (a) lh_animator_set_frame(&a->animator, frame);
}

int lh_animatable_get_frame(const lh_animatable* a)
{
    return a ? lh_animator_get_frame(&a->animator) : 1;
}

lh_anim_state lh_animatable_get_state(const lh_animatable* a)
{
    return a ? a->animator.state : LH_ANIM_STOPPED;
}
