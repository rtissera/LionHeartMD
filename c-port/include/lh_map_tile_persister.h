/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com)
 *                          Pierre-Alexandre (contact@b3dgs.com)
 * C/SDL2 port: faithful derivative of MapTilePersisterModel +
 *              MapTilePersisterOptimized (the only flavour Lionheart
 *              actually writes on disk).
 *
 * Reads / writes Lionheart .lvl binary level files. Format (big-endian):
 *
 *   char  tile_width
 *   char  tile_height
 *   int   in_tile_width
 *   int   in_tile_height
 *   char  bloc_count_x   (= ceil(in_tile_width  / 255))
 *   char  bloc_count_y   (= ceil(in_tile_height / 255))
 *   for each bloc (sx in [0..bloc_count_x), sy in [0..bloc_count_y)):
 *       char tiles_in_bloc
 *       repeat tiles_in_bloc times:
 *           char  tile_number   (16-bit unsigned)
 *           byte  tx mod 255    (unsigned 0..254)
 *           byte  ty mod 255    (unsigned 0..254)
 *
 * Note: Lionheart's MapTilePersisterOptimized stores tile_number as a
 * 16-bit value; sheet ids are looked up from TileSheetsConfig at runtime.
 * On read we therefore set sheet=0 in the packed lh_map_tile cell. On
 * write we mask to the low 16 bits and warn (return false) if any cell
 * exceeds that range or has a non-zero sheet, since the on-disk format
 * cannot represent it.
 */
#ifndef LH_MAP_TILE_PERSISTER_H
#define LH_MAP_TILE_PERSISTER_H

#include <stdbool.h>
#include "lh_le_map_tile.h"

/* Block size used by MapTilePersisterModel (Constant.UNSIGNED_BYTE - 1). */
#define LH_MAP_PERSIST_BLOC_SIZE 255

/* Save/load. `path` is a filesystem path (resolve via lh_medias if needed
 * before calling). Returns true on success. */
bool lh_map_tile_persister_save(const lh_map_tile* m, const char* path);
bool lh_map_tile_persister_load(lh_map_tile* m,       const char* path);

/* Self-test: load a sample .lvl, print dimensions + first 10 tiles, save
 * to a temp path, reload, and verify byte-for-byte cell equality. Returns
 * true if the round-trip matches. Prints diagnostics to stdout/stderr. */
bool lh_map_tile_persister_self_test(const char* sample_lvl_path);

#endif
