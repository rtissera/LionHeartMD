/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com)
 *                          Pierre-Alexandre (contact@b3dgs.com)
 * C/SDL2 port: faithful derivative of MapTileSurface (subset).
 */
#include "lh_le_map_tile.h"

#include <stdlib.h>
#include <string.h>

bool lh_map_tile_create(lh_map_tile* m,
                        int tile_width, int tile_height,
                        int in_tile_width, int in_tile_height)
{
    if (!m) return false;
    if (tile_width <= 0 || tile_height <= 0
        || in_tile_width <= 0 || in_tile_height <= 0) return false;

    /* Free any prior allocation. */
    lh_map_tile_destroy(m);

    const size_t cells = (size_t)in_tile_width * (size_t)in_tile_height;
    int32_t* buf = (int32_t*)malloc(cells * sizeof(int32_t));
    if (!buf) return false;
    for (size_t i = 0; i < cells; i++) buf[i] = LH_TILE_EMPTY;

    m->tile_width     = tile_width;
    m->tile_height    = tile_height;
    m->in_tile_width  = in_tile_width;
    m->in_tile_height = in_tile_height;
    m->tiles          = buf;
    return true;
}

void lh_map_tile_destroy(lh_map_tile* m)
{
    if (!m) return;
    free(m->tiles);
    m->tiles = NULL;
    m->tile_width = 0;
    m->tile_height = 0;
    m->in_tile_width = 0;
    m->in_tile_height = 0;
}

static inline bool in_bounds(const lh_map_tile* m, int tx, int ty)
{
    return tx >= 0 && ty >= 0
        && tx < m->in_tile_width && ty < m->in_tile_height;
}

void lh_map_tile_set_tile(lh_map_tile* m, int tx, int ty, int32_t packed)
{
    if (!m || !m->tiles) return;
    if (!in_bounds(m, tx, ty)) return;
    m->tiles[(size_t)ty * (size_t)m->in_tile_width + (size_t)tx] = packed;
}

int32_t lh_map_tile_get_tile(const lh_map_tile* m, int tx, int ty)
{
    if (!m || !m->tiles) return LH_TILE_EMPTY;
    if (!in_bounds(m, tx, ty)) return LH_TILE_EMPTY;
    return m->tiles[(size_t)ty * (size_t)m->in_tile_width + (size_t)tx];
}

int32_t lh_map_tile_get_tile_at_pixel(const lh_map_tile* m, int px, int py)
{
    if (!m || !m->tiles || m->tile_width <= 0 || m->tile_height <= 0)
        return LH_TILE_EMPTY;
    return lh_map_tile_get_tile(m, px / m->tile_width, py / m->tile_height);
}

int lh_map_tile_get_width(const lh_map_tile* m)
{
    return m ? m->tile_width * m->in_tile_width : 0;
}

int lh_map_tile_get_height(const lh_map_tile* m)
{
    return m ? m->tile_height * m->in_tile_height : 0;
}

int lh_map_tile_get_in_tile_width(const lh_map_tile* m)
{
    return m ? m->in_tile_width : 0;
}

int lh_map_tile_get_in_tile_height(const lh_map_tile* m)
{
    return m ? m->in_tile_height : 0;
}
