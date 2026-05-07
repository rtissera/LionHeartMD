/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.Animatable
 *   com.b3dgs.lionengine.game.feature.AnimatableModel
 *
 * Wraps the existing lh_animator (phase 2b) as a feature so it participates
 * in Handler.update via update_vt. play()/setFrame()/pause() forward to the
 * embedded animator.
 */
#ifndef LH_LE_ANIMATABLE_H
#define LH_LE_ANIMATABLE_H

#include "lh_le_anim.h"
#include "lh_le_feature.h"

typedef struct {
    lh_feature  base;
    lh_animator animator;
} lh_animatable;

void          lh_animatable_init    (lh_animatable* a);
void          lh_animatable_play    (lh_animatable* a, const lh_animation* anim);
void          lh_animatable_pause   (lh_animatable* a);
void          lh_animatable_set_frame(lh_animatable* a, int frame);
int           lh_animatable_get_frame(const lh_animatable* a);
lh_anim_state lh_animatable_get_state(const lh_animatable* a);

#endif
