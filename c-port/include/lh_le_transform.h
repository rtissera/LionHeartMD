/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Port of com.b3dgs.lionengine.graphic.Transform (scale + interpolation hint).
 */
#ifndef LH_LE_TRANSFORM_H
#define LH_LE_TRANSFORM_H

#include "lh_le_filter.h"

typedef struct {
    double    scale_x;
    double    scale_y;
    lh_filter interpolation;
} lh_transform;

#define LH_TRANSFORM_IDENTITY ((lh_transform){1.0, 1.0, LH_FILTER_NONE})

#endif
