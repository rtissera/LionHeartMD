/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.tile.map.TileSheetsConfig.
 *
 * Reads sheets.xml describing one or more tile sheet PNGs sharing the
 * same tile dimensions:
 *   <lionengine:sheets tileWidth="N" tileHeight="M">
 *     <lionengine:sheet>tile_set_a.png</lionengine:sheet>
 *     ...
 *   </lionengine:sheets>
 */
#ifndef LH_TILE_SHEETS_CONFIG_H
#define LH_TILE_SHEETS_CONFIG_H

#include "lh_xml.h"

#define LH_TILE_SHEETS_FILENAME    "sheets.xml"
#define LH_TILE_SHEETS_NODE_ROOT   "lionengine:sheets"
#define LH_TILE_SHEETS_NODE_SHEET  "lionengine:sheet"

#define LH_TILE_SHEETS_MAX 32
#define LH_TILE_SHEET_PATH_MAX 128

typedef struct {
    int  tile_width;
    int  tile_height;
    char paths[LH_TILE_SHEETS_MAX][LH_TILE_SHEET_PATH_MAX];
    int  count;
} lh_tile_sheets_config;

bool lh_tile_sheets_load(lh_tile_sheets_config* cfg, const char* media);

#endif
