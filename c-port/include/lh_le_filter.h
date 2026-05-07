/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Port of com.b3dgs.lionengine.graphic.Filter (subset).
 *
 * Only NONE + BILINEAR matter for Lionheart; HQ2X/HQ3X TODO/DEFER.
 */
#ifndef LH_LE_FILTER_H
#define LH_LE_FILTER_H

typedef enum {
    LH_FILTER_NONE,
    LH_FILTER_BILINEAR,
    /* TODO: HQ2X, HQ3X, SCANLINE — not yet ported */
} lh_filter;

#endif
