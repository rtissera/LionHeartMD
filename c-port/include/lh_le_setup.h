/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.Setup
 *   com.b3dgs.lionengine.game.feature.SetupSurfaceRastered
 *
 * Carries data parsed from an entity XML resource: animation frames,
 * collision shapes, body config, sprite sheet path. Phase 4 wires actual
 * XML parsing + asset cache; the lh_setup_def table is for build-time
 * codegen identification (string keys for Factory dispatch).
 */
#ifndef LH_LE_SETUP_H
#define LH_LE_SETUP_H

#include <stdbool.h>
#include "lh_animation_config.h"
#include "lh_frames_config.h"
#include "lh_le_image_buffer.h"
#include "lh_xml.h"

typedef struct {
    const char* media;          /* XML media path identifier */
    const char* surface_path;   /* sprite sheet PNG path */
    int         frame_h;        /* default if XML <frames> missing */
    int         frame_v;
} lh_setup_def;

typedef struct {
    const lh_setup_def*  def;       /* nullable */
    lh_xml_doc*          doc;       /* parsed entity XML, owned */
    lh_image_buffer*     surface;   /* loaded sprite sheet (cached, NOT owned) */
    lh_frames_config     frames;
    lh_animation_config  anims;
    char                 media_path[256];
} lh_setup;

void lh_setup_init   (lh_setup* s, const lh_setup_def* def);
void lh_setup_destroy(lh_setup* s);

/* Phase-4 load: parse the entity XML at logical media path, populate frames
 * + animations, and load the sprite sheet via the asset cache. Returns
 * true on success. The Setup keeps the parsed XML so feature ctors can
 * read their own subnodes (collision boxes, body params, etc.). */
bool lh_setup_load(lh_setup* s, const char* media);

const char*         lh_setup_get_media   (const lh_setup* s);
lh_image_buffer*    lh_setup_get_surface (const lh_setup* s);
int                 lh_setup_get_frame_h (const lh_setup* s);
int                 lh_setup_get_frame_v (const lh_setup* s);
const lh_xml_node*  lh_setup_get_root    (const lh_setup* s);
const lh_animation* lh_setup_get_anim    (const lh_setup* s, const char* name);

bool                lh_setup_has_node    (const lh_setup* s, const char* node);

#endif
