/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_camera_tracker.h"
#include <math.h>

void lh_camera_tracker_init(lh_camera_tracker* t, lh_camera* camera)
{
    t->camera   = camera;
    t->target   = NULL;
    t->offset_x = 0;
    t->offset_y = 0;
}

void lh_camera_tracker_track(lh_camera_tracker* t, const lh_trackable* target)
{
    t->target = target;
}

void lh_camera_tracker_set_offset(lh_camera_tracker* t, int ox, int oy)
{
    t->offset_x = ox;
    t->offset_y = oy;
}

void lh_camera_tracker_update(lh_camera_tracker* t, double extrp)
{
    (void)extrp;
    if (!t || !t->camera || !t->target) return;

    const double tx = lh_transformable_get_x(t->target) + t->offset_x;
    const double ty = lh_transformable_get_y(t->target) + t->offset_y;
    /* Center camera on target. */
    const double cx = tx - t->camera->width  * 0.5;
    const double cy = ty - t->camera->height * 0.5;

    /* Apply dead-zone interval — camera only moves when target leaves zone. */
    const double cur_x = t->camera->x;
    const double cur_y = t->camera->y;
    double new_x = cur_x;
    double new_y = cur_y;

    if (fabs(cx - cur_x) > t->camera->interval_h) new_x = cx;
    if (fabs(cy - cur_y) > t->camera->interval_v) new_y = cy;

    lh_camera_set_location(t->camera, new_x, new_y);
}
