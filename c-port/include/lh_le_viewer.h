/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Port of com.b3dgs.lionengine.game.feature.Viewer (interface).
 *
 * Concrete impl: Camera (lh_le_camera.h) — phase 2c.
 */
#ifndef LH_LE_VIEWER_H
#define LH_LE_VIEWER_H

#include <stdbool.h>

typedef struct lh_viewer lh_viewer;

typedef struct {
    double (*get_x)         (const lh_viewer*);
    double (*get_y)         (const lh_viewer*);
    int    (*get_view_x)    (const lh_viewer*);
    int    (*get_view_y)    (const lh_viewer*);
    int    (*get_width)     (const lh_viewer*);
    int    (*get_height)    (const lh_viewer*);
    double (*get_view_offset_x)(const lh_viewer*);
    double (*get_view_offset_y)(const lh_viewer*);
    int    (*get_screen_height)(const lh_viewer*);
    bool   (*is_viewable)   (const lh_viewer*, double x, double y, int w, int h);
} lh_viewer_vt;

struct lh_viewer {
    const lh_viewer_vt* vt;
};

#endif
