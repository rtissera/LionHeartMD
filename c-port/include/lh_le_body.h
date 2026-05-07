/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.body.Body
 *   com.b3dgs.lionengine.game.feature.body.BodyModel
 *
 * Gravity-affected vertical direction accumulator. update(extrp) increments
 * direction_vertical by gravity until clamped at gravity_max.
 */
#ifndef LH_LE_BODY_H
#define LH_LE_BODY_H

#include "lh_le_feature.h"

typedef struct {
    lh_feature base;
    double     gravity;
    double     gravity_max;
    double     direction_h;
    double     direction_v;
} lh_body;

void   lh_body_init             (lh_body* b);
void   lh_body_set_gravity      (lh_body* b, double g);
void   lh_body_set_gravity_max  (lh_body* b, double gmax);
void   lh_body_reset_gravity    (lh_body* b);

double lh_body_get_direction_h  (const lh_body* b);
double lh_body_get_direction_v  (const lh_body* b);

#endif
