/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.rasterable.Rasterable
 *   com.b3dgs.lionengine.game.feature.rasterable.RasterableModel
 *
 * Sprite-rasterization feature. Draws an animated sprite at a Transformable's
 * position, optionally remapping palette via a raster bar table indexed by
 * world Y. Phase 2c skeleton — full raster bar palette swap lands phase 7
 * (Lionheart World) when MapTileWater + raster pipeline is ready.
 */
#ifndef LH_LE_RASTERABLE_H
#define LH_LE_RASTERABLE_H

#include <stdbool.h>
#include "lh_le_feature.h"
#include "lh_le_sprite.h"
#include "lh_le_transformable.h"
#include "lh_le_mirrorable.h"

typedef struct {
    lh_feature             base;
    lh_sprite*             sprite;          /* not owned */
    const lh_transformable* transformable;  /* not owned, looked up at prepare */
    const lh_mirrorable*    mirrorable;     /* not owned */
    int                    anim_offset;
    int                    anim_offset2;
    int                    frame_offset_x;
    int                    frame_offset_y;
    bool                   visible;
} lh_rasterable;

void lh_rasterable_init           (lh_rasterable* r, lh_sprite* sprite);
void lh_rasterable_bind           (lh_rasterable* r,
                                   const lh_transformable* tf,
                                   const lh_mirrorable* mr);
void lh_rasterable_set_anim_offset(lh_rasterable* r, int offset);
void lh_rasterable_set_anim_offset2(lh_rasterable* r, int offset);
void lh_rasterable_set_frame_offsets(lh_rasterable* r, int ox, int oy);
void lh_rasterable_set_visible    (lh_rasterable* r, bool v);
int  lh_rasterable_get_anim_offset(const lh_rasterable* r);
int  lh_rasterable_get_anim_offset2(const lh_rasterable* r);

#endif
