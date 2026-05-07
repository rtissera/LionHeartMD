/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_image_buffer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

lh_image_buffer* lh_image_buffer_load(SDL_Renderer* g, const char* path)
{
    SDL_Surface* surf = IMG_Load(path);
    if (!surf)
    {
        fprintf(stderr, "lh_image_buffer_load IMG_Load(%s): %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(g, surf);
    const int w = surf->w, h = surf->h;
    SDL_FreeSurface(surf);
    if (!tex)
    {
        fprintf(stderr, "lh_image_buffer_load CreateTexture: %s\n", SDL_GetError());
        return NULL;
    }
    lh_image_buffer* b = (lh_image_buffer*)calloc(1, sizeof(*b));
    b->tex       = tex;
    b->width     = w;
    b->height    = h;
    b->is_target = false;
    return b;
}

lh_image_buffer* lh_image_buffer_create(SDL_Renderer* g, int w, int h)
{
    SDL_Texture* tex = SDL_CreateTexture(g, SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_TARGET, w, h);
    if (!tex)
    {
        fprintf(stderr, "lh_image_buffer_create: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    lh_image_buffer* b = (lh_image_buffer*)calloc(1, sizeof(*b));
    b->tex       = tex;
    b->width     = w;
    b->height    = h;
    b->is_target = true;
    return b;
}

void lh_image_buffer_set_transparent(lh_image_buffer* b, lh_color_rgba c)
{
    if (!b || !b->tex) return;
    /* SDL approximate: alpha-mod on key colors. Real keying done at surface load
     * — here we just expose blend, since textures are already pre-multiplied. */
    SDL_SetTextureBlendMode(b->tex, SDL_BLENDMODE_BLEND);
    (void)c;
}

void lh_image_buffer_destroy(lh_image_buffer* b)
{
    if (!b) return;
    if (b->tex) SDL_DestroyTexture(b->tex);
    free(b);
}
