/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.graphic.ImageBuffer (subset used by Lionheart).
 * Backed by SDL_Texture in render-target mode for createImageBuffer paths,
 * or static texture for getImageBuffer (file load) paths.
 */
#ifndef LH_LE_IMAGE_BUFFER_H
#define LH_LE_IMAGE_BUFFER_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include "lh_le_color.h"

typedef struct lh_image_buffer {
    SDL_Texture* tex;
    int          width;
    int          height;
    bool         is_target;   /* SDL_TEXTUREACCESS_TARGET */
} lh_image_buffer;

/* Load PNG/etc. from file path. Returns NULL on failure. */
lh_image_buffer* lh_image_buffer_load(SDL_Renderer* g, const char* path);

/* Allocate a render-target buffer of given size. */
lh_image_buffer* lh_image_buffer_create(SDL_Renderer* g, int w, int h);

/* Set per-texture transparent/colorkey, mirroring ImageBuffer.setTransparency. */
void lh_image_buffer_set_transparent(lh_image_buffer* b, lh_color_rgba c);

void lh_image_buffer_destroy(lh_image_buffer* b);

#endif
