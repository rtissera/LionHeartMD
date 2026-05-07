/*
 * Lionheart Remake - C/SDL2 port
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com) Pierre-Alexandre
 * Port: derivative work under GNU GPL v3 (see LICENSE).
 *
 * This file is the C port of java/lionheart-game/.../Constant.java
 */
#ifndef LH_CONSTANT_H
#define LH_CONSTANT_H

#include <stdbool.h>
#include <stdint.h>

#define LH_PROGRAM_NAME    "Lionheart Remake"
#define LH_PROGRAM_VERSION "1.4.0"
#define LH_PROGRAM_WEBSITE "https://lionheart.b3dgs.com"

/* Resolutions (w, h, refresh rate Hz) */
#define LH_RES_W           320
#define LH_RES_H           240
#define LH_RES_RATE        50
#define LH_RES_GAME_W      278
#define LH_RES_GAME_H      208
#define LH_RES_OUT_W       640
#define LH_RES_OUT_H       480

#define LH_DEBUG_COLLISIONS 0

/* Collision groups */
#define LH_COLL_GROUP_PLAYER       1
#define LH_COLL_GROUP_ENEMIES      2
#define LH_COLL_GROUP_BACKGROUND   3
#define LH_COLL_GROUP_PROJECTILES  4

/* Stats caps */
#define LH_STATS_MAX_HEART     8
#define LH_STATS_MAX_HEALTH    99
#define LH_STATS_MAX_TALISMENT 99
#define LH_STATS_MAX_LIFE      99
#define LH_STATS_MAX_SWORD     4

#define LH_CREDITS                  7
#define LH_CAMERA_HORIZONTAL_MARGIN 16

/* Physics (matches Java doubles) */
#define LH_GRAVITY                       7.8
#define LH_WALK_SPEED                    2.0
#define LH_WALK_MIN_SPEED                0.75
#define LH_WALK_VELOCITY_SLOPE_DECREASE  0.0001
#define LH_WALK_VELOCITY_MAX             0.15
#define LH_JUMP_MIN                      2.5
#define LH_JUMP_HIT                      5.5
#define LH_JUMP_MAX_X                    0.0
#define LH_JUMP_MAX_Y                    5.75
#define LH_JUMP_SPIDER_X                 0.0
#define LH_JUMP_SPIDER_Y                 4.2

#define LH_STAGE_PREFIX        "stage"
#define LH_STAGE_HARD_SUFFIX   "_hard"

#define LH_RASTER_FILE_TILE   "tiles.png"
#define LH_RASTER_FILE_WATER  "water.png"
#define LH_RASTER_FILE_LAVA   "lava.png"

#define LH_INPUT_FILE_DEFAULT "input.xml"
#define LH_INPUT_FILE_CURSOR  "input_cursor.xml"

#define LH_FILE_PROGRESS  "progress.lhr"
#define LH_FILE_SNAPSHOT  "snapshot.lhr"

#define LH_ZOOM_MIN  0.8
#define LH_ZOOM_MAX  1.3

#define LH_UNSIGNED_BYTE 256

typedef struct { uint8_t r, g, b, a; } lh_color_rgba;

extern lh_color_rgba LH_ALPHAS_BLACK[LH_UNSIGNED_BYTE];
extern lh_color_rgba LH_ALPHAS_WHITE[LH_UNSIGNED_BYTE];

void lh_constant_init(void);

#endif /* LH_CONSTANT_H */
