/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_camera.h"
#include <math.h>

static double cam_get_x(const lh_viewer* v)
{
    const lh_camera* c = (const lh_camera*)v;
    return c->x;
}

static double cam_get_y(const lh_viewer* v)
{
    const lh_camera* c = (const lh_camera*)v;
    return c->y;
}

static int cam_get_view_x(const lh_viewer* v)
{
    return ((const lh_camera*)v)->view_x;
}

static int cam_get_view_y(const lh_viewer* v)
{
    return ((const lh_camera*)v)->view_y;
}

static int cam_get_width(const lh_viewer* v)
{
    return ((const lh_camera*)v)->width;
}

static int cam_get_height(const lh_viewer* v)
{
    return ((const lh_camera*)v)->height;
}

static double cam_get_view_offset_x(const lh_viewer* v)
{
    return ((const lh_camera*)v)->view_offset_x;
}

static double cam_get_view_offset_y(const lh_viewer* v)
{
    return ((const lh_camera*)v)->view_offset_y;
}

static int cam_get_screen_height(const lh_viewer* v)
{
    return ((const lh_camera*)v)->screen_height;
}

static bool cam_is_viewable(const lh_viewer* v, double x, double y, int w, int h)
{
    const lh_camera* c = (const lh_camera*)v;
    return x + w > c->x
        && x     < c->x + c->width
        && y + h > c->y
        && y     < c->y + c->height;
}

static const lh_viewer_vt s_camera_vt = {
    .get_x            = cam_get_x,
    .get_y            = cam_get_y,
    .get_view_x       = cam_get_view_x,
    .get_view_y       = cam_get_view_y,
    .get_width        = cam_get_width,
    .get_height       = cam_get_height,
    .get_view_offset_x= cam_get_view_offset_x,
    .get_view_offset_y= cam_get_view_offset_y,
    .get_screen_height= cam_get_screen_height,
    .is_viewable      = cam_is_viewable,
};

static void clamp_to_limits(lh_camera* c)
{
    if (c->no_limit) return;
    if (c->x < c->min_x) c->x = c->min_x;
    if (c->y < c->min_y) c->y = c->min_y;
    if (c->x > c->max_x) c->x = c->max_x;
    if (c->y > c->max_y) c->y = c->max_y;
}

void lh_camera_init(lh_camera* c)
{
    c->viewer.vt = &s_camera_vt;
    c->x = c->y = 0.0;
    c->view_x = c->view_y = 0;
    c->width = c->height = 0;
    c->view_offset_x = c->view_offset_y = 0.0;
    c->screen_height = 0;
    c->no_limit = true;
    c->min_x = c->max_x = c->min_y = c->max_y = 0.0;
    c->interval_h = c->interval_v = 0;
}

void lh_camera_set_view(lh_camera* c, int vx, int vy, int w, int h, int screen_h)
{
    c->view_x = vx;
    c->view_y = vy;
    c->width  = w;
    c->height = h;
    c->screen_height = screen_h;
}

void lh_camera_set_location(lh_camera* c, double x, double y)
{
    c->x = x;
    c->y = y;
    clamp_to_limits(c);
}

void lh_camera_teleport(lh_camera* c, double x, double y)
{
    lh_camera_set_location(c, x, y);
}

void lh_camera_move(lh_camera* c, double extrp, double dx, double dy)
{
    c->x += dx * extrp;
    c->y += dy * extrp;
    clamp_to_limits(c);
}

void lh_camera_set_intervals(lh_camera* c, int ih, int iv)
{
    c->interval_h = ih;
    c->interval_v = iv;
}

void lh_camera_set_limits(lh_camera* c, double min_x, double max_x, double min_y, double max_y)
{
    c->no_limit = false;
    c->min_x = min_x; c->max_x = max_x;
    c->min_y = min_y; c->max_y = max_y;
    clamp_to_limits(c);
}

void lh_camera_clear_limits(lh_camera* c)
{
    c->no_limit = true;
}

double lh_camera_get_x(const lh_camera* c) { return c ? c->x : 0.0; }
double lh_camera_get_y(const lh_camera* c) { return c ? c->y : 0.0; }
