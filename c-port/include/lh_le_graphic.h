/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.graphic.Graphic (subset).
 * Lionheart uses: setColor, setAlpha, drawRect, drawLine, fillRect, drawImage,
 *                 copyArea, setColorGradient.
 */
#ifndef LH_LE_GRAPHIC_H
#define LH_LE_GRAPHIC_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "lh_le_color.h"
#include "lh_le_image_buffer.h"
#include "lh_le_transform.h"

typedef struct {
    SDL_Renderer*    sdl;
    lh_color_rgba    color;
    uint8_t          alpha;
    SDL_Texture*     prev_target;   /* for push/pop target */
} lh_graphic;

void lh_graphic_init(lh_graphic* g, SDL_Renderer* sdl);

void lh_graphic_set_color(lh_graphic* g, lh_color_rgba c);
void lh_graphic_set_alpha(lh_graphic* g, uint8_t a);
void lh_graphic_set_color_gradient(lh_graphic* g,
                                   int x1, int y1, lh_color_rgba c1,
                                   int x2, int y2, lh_color_rgba c2);

void lh_graphic_draw_rect (lh_graphic* g, int x, int y, int w, int h, bool fill);
void lh_graphic_fill_rect (lh_graphic* g, int x, int y, int w, int h);
void lh_graphic_draw_line (lh_graphic* g, int x1, int y1, int x2, int y2);
void lh_graphic_draw_image(lh_graphic* g, const lh_image_buffer* img, int x, int y);
void lh_graphic_draw_image_region(lh_graphic* g, const lh_image_buffer* img,
                                  int sx, int sy, int sw, int sh,
                                  int dx, int dy);
void lh_graphic_draw_image_region_flip(lh_graphic* g, const lh_image_buffer* img,
                                       int sx, int sy, int sw, int sh,
                                       int dx, int dy,
                                       bool flip_h, bool flip_v);

/* Render-target push/pop — used by ImageBuffer::createGraphic surfaces. */
void lh_graphic_push_target(lh_graphic* g, lh_image_buffer* target);
void lh_graphic_pop_target (lh_graphic* g);

#endif
