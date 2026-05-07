/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Asset cache. Wraps lh_image_buffer_load with a string→pointer hash so
 * repeated loads of the same logical media path return the same buffer.
 * Mirrors LionEngine's Drawable factory caching.
 */
#ifndef LH_ASSET_CACHE_H
#define LH_ASSET_CACHE_H

#include "lh_le_image_buffer.h"

void              lh_asset_cache_init   (SDL_Renderer* renderer);
void              lh_asset_cache_destroy(void);

/* Load (or return cached) image buffer for a logical media path. */
lh_image_buffer*  lh_asset_get_image    (const char* logical);

/* Stats. */
int               lh_asset_cache_count  (void);

#endif
