/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com)
 *                          Pierre-Alexandre (contact@b3dgs.com)
 * C/SDL2 port: faithful derivative of MapTileSurface (subset).
 *
 * Minimal map tile grid backing store for Phase 4. Holds the tile sheet
 * layout: dimensions in pixels and tiles, plus a flat array of packed
 * (sheet, tile) entries. The packing keeps space for future sheet ids on
 * the wire even though Lionheart's optimized .lvl format only stores a
 * 16-bit tile number per cell — sheet id is derived from TileSheetsConfig
 * after load, so we keep it = 0 when reading raw .lvl data.
 */
#ifndef LH_LE_MAP_TILE_H
#define LH_LE_MAP_TILE_H

#include <stdbool.h>
#include <stdint.h>

/* Sentinel value indicating an empty cell (no tile drawn). */
#define LH_TILE_EMPTY        ((int32_t)-1)

/* Pack/unpack helpers: high 16 bits = sheet id, low 16 bits = tile number. */
#define LH_TILE_PACK(sheet, num) \
    ((int32_t)((((uint32_t)(uint16_t)(sheet)) << 16) | \
                ((uint32_t)(uint16_t)(num))))
#define LH_TILE_SHEET(packed)    ((int)(((uint32_t)(packed) >> 16) & 0xFFFFu))
#define LH_TILE_NUMBER(packed)   ((int)((uint32_t)(packed) & 0xFFFFu))

typedef struct {
    int      tile_width;       /* pixel width  of a tile */
    int      tile_height;      /* pixel height of a tile */
    int      in_tile_width;    /* grid width  in tiles  */
    int      in_tile_height;   /* grid height in tiles  */
    int32_t* tiles;            /* in_tile_width * in_tile_height entries  */
} lh_map_tile;

/* Allocate the tile grid with the given dimensions. All cells are
 * initialised to LH_TILE_EMPTY. Returns false on alloc failure. Calling
 * create on an already-initialised map_tile destroys it first. */
bool lh_map_tile_create (lh_map_tile* m,
                         int tile_width, int tile_height,
                         int in_tile_width, int in_tile_height);
void lh_map_tile_destroy(lh_map_tile* m);

/* Set/get cells. Out-of-bounds set is silently dropped; out-of-bounds
 * get returns LH_TILE_EMPTY. */
void    lh_map_tile_set_tile (lh_map_tile* m, int tx, int ty, int32_t packed);
int32_t lh_map_tile_get_tile (const lh_map_tile* m, int tx, int ty);
int32_t lh_map_tile_get_tile_at_pixel(const lh_map_tile* m, int px, int py);

int lh_map_tile_get_width          (const lh_map_tile* m); /* pixels */
int lh_map_tile_get_height         (const lh_map_tile* m); /* pixels */
int lh_map_tile_get_in_tile_width  (const lh_map_tile* m); /* tiles  */
int lh_map_tile_get_in_tile_height (const lh_map_tile* m); /* tiles  */

#endif
