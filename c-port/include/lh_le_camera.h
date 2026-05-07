/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.Camera.
 *
 * Top-level scene viewer. Implements lh_viewer interface so renderable
 * features can compute screen coordinates from world coordinates. Held in
 * the Services registry under LH_SVC_CAMERA.
 */
#ifndef LH_LE_CAMERA_H
#define LH_LE_CAMERA_H

#include "lh_le_viewer.h"

typedef struct {
    lh_viewer viewer;            /* embed first so cast to lh_viewer* works */
    double    x, y;
    int       view_x, view_y;
    int       width, height;
    double    view_offset_x, view_offset_y;
    int       screen_height;
    /* Optional clamping bounds. Disabled when no_limit==true. */
    bool      no_limit;
    double    min_x, max_x, min_y, max_y;
    /* Interval for camera dead-zone (move only when target leaves zone). */
    int       interval_h, interval_v;
} lh_camera;

void   lh_camera_init        (lh_camera* c);
void   lh_camera_set_view    (lh_camera* c, int vx, int vy, int w, int h, int screen_h);
void   lh_camera_set_location(lh_camera* c, double x, double y);
void   lh_camera_teleport    (lh_camera* c, double x, double y);
void   lh_camera_move        (lh_camera* c, double extrp, double dx, double dy);

void   lh_camera_set_intervals(lh_camera* c, int ih, int iv);
void   lh_camera_set_limits   (lh_camera* c, double min_x, double max_x, double min_y, double max_y);
void   lh_camera_clear_limits (lh_camera* c);

double lh_camera_get_x(const lh_camera* c);
double lh_camera_get_y(const lh_camera* c);

#endif
