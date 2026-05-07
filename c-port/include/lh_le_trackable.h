/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.Trackable.
 *
 * Marker for a Featurable whose Transformable should be followed by a
 * CameraTracker. The C port keeps it as a typedef alias to Transformable
 * since Lionheart's only consumer (Camera follow) reads x/y.
 */
#ifndef LH_LE_TRACKABLE_H
#define LH_LE_TRACKABLE_H

#include "lh_le_transformable.h"

typedef lh_transformable lh_trackable;

#endif
