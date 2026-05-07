/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.Setup
 *   com.b3dgs.lionengine.game.feature.SetupSurfaceRastered
 *
 * Carries data parsed from an entity XML resource: animation frames,
 * collision shapes, body config, sprite sheet path. The C port replaces
 * runtime XML parsing with a build-time codegen — at phase 4, scripts
 * scan asset XML files and emit a const lh_setup_def[] table indexed by
 * entity id.
 *
 * Phase 2c: skeleton struct + lookup. Concrete data wiring lands phase 4.
 */
#ifndef LH_LE_SETUP_H
#define LH_LE_SETUP_H

#include "lh_le_image_buffer.h"

typedef struct {
    const char* media;          /* XML path identifier */
    const char* surface_path;   /* sprite sheet PNG path */
    int         frame_h;
    int         frame_v;
    /* TODO phase 4: animation table, collision boxes, body params */
} lh_setup_def;

typedef struct {
    const lh_setup_def* def;       /* nullable */
    lh_image_buffer*    surface;   /* loaded sprite sheet, owned */
} lh_setup;

void lh_setup_init   (lh_setup* s, const lh_setup_def* def);
void lh_setup_destroy(lh_setup* s);

const char*       lh_setup_get_media   (const lh_setup* s);
lh_image_buffer*  lh_setup_get_surface (const lh_setup* s);
int               lh_setup_get_frame_h (const lh_setup* s);
int               lh_setup_get_frame_v (const lh_setup* s);

#endif
