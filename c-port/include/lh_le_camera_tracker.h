/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.CameraTracker.
 *
 * Follows a Trackable target each frame, keeping the Camera centered on
 * (target.x + offset_x, target.y + offset_y). Honors the camera's interval
 * dead-zone — Camera only moves when target leaves the zone.
 */
#ifndef LH_LE_CAMERA_TRACKER_H
#define LH_LE_CAMERA_TRACKER_H

#include "lh_le_camera.h"
#include "lh_le_trackable.h"

typedef struct {
    lh_camera*          camera;
    const lh_trackable* target;        /* nullable */
    int                 offset_x;
    int                 offset_y;
} lh_camera_tracker;

void lh_camera_tracker_init  (lh_camera_tracker* t, lh_camera* camera);
void lh_camera_tracker_track (lh_camera_tracker* t, const lh_trackable* target);
void lh_camera_tracker_set_offset(lh_camera_tracker* t, int ox, int oy);
void lh_camera_tracker_update(lh_camera_tracker* t, double extrp);

#endif
