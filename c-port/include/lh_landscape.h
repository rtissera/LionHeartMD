/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionheart.landscape.Landscape
 *   com.b3dgs.lionheart.landscape.BackgroundAbstract
 *   com.b3dgs.lionheart.landscape.Parallax
 *
 * Background + foreground multi-layer parallax. Each layer has its own
 * image surface and parallax factor; render functions take the active
 * camera and compute scroll offsets per layer.
 *
 * Phase 7 skeleton: API + struct in place, layer loading is hardcoded
 * empty until landscape XML parser lands (phase 8 / pre-stage hook).
 */
#ifndef LH_LANDSCAPE_H
#define LH_LANDSCAPE_H

#include <stdbool.h>
#include "lh_le_camera.h"
#include "lh_le_graphic.h"
#include "lh_le_image_buffer.h"

#define LH_LANDSCAPE_BG_LAYERS 8
#define LH_LANDSCAPE_FG_LAYERS 4

typedef struct {
    lh_image_buffer* surface;       /* not owned (asset cache) */
    double           parallax_x;    /* fraction of camera.x */
    double           parallax_y;
    int              base_y;        /* draw y baseline */
    int              tile_w;        /* horizontal repeat width (0 = no repeat) */
    bool             enabled;
} lh_landscape_layer;

typedef struct {
    lh_landscape_layer bg[LH_LANDSCAPE_BG_LAYERS];
    int                bg_count;
    lh_landscape_layer fg[LH_LANDSCAPE_FG_LAYERS];
    int                fg_count;
    int                view_width;     /* destination clip width  */
    int                view_height;    /* destination clip height */
} lh_landscape;

void lh_landscape_init   (lh_landscape* l, int view_w, int view_h);
void lh_landscape_destroy(lh_landscape* l);

/* Phase 7 skeleton — accepts a media path but does not parse yet. Returns
 * true even for unknown media. Real loading lands when landscape XML
 * parser is added. */
bool lh_landscape_load   (lh_landscape* l, const char* media);

/* Add a layer manually (used until XML loader is in). */
void lh_landscape_add_bg (lh_landscape* l, lh_image_buffer* surface,
                          double px, double py, int base_y, int tile_w);
void lh_landscape_add_fg (lh_landscape* l, lh_image_buffer* surface,
                          double px, double py, int base_y, int tile_w);

void lh_landscape_render_background(const lh_landscape* l, lh_graphic* g, const lh_camera* cam);
void lh_landscape_render_foreground(const lh_landscape* l, lh_graphic* g, const lh_camera* cam);
void lh_landscape_update            (lh_landscape* l, double extrp, const lh_camera* cam);

#endif
