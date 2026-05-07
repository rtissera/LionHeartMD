/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.graphic.drawable.SpriteFont.
 * Lionheart fonts: bitmap atlas + XML data file (char widths).
 */
#ifndef LH_LE_SPRITE_FONT_H
#define LH_LE_SPRITE_FONT_H

#include "lh_le_align.h"
#include "lh_le_graphic.h"
#include "lh_le_image_buffer.h"

#define LH_FONT_CHARS 256

typedef struct {
    lh_image_buffer* surface;
    bool             owns_surface;
    int              tile_w;
    int              tile_h;
    int              widths[LH_FONT_CHARS]; /* glyph widths (variable-pitch) */
    int              line_height;
    /* Text state */
    int              text_x;
    int              text_y;
    lh_align         align;
} lh_sprite_font;

/* Load font: image at img_path, widths from data_path (XML char-set).
 * Phase 2b minimal: data_path may be NULL (fixed pitch tile_w). */
lh_sprite_font* lh_sprite_font_load(SDL_Renderer* g,
                                    const char*   img_path,
                                    const char*   data_path,
                                    int           tile_w,
                                    int           tile_h);

void lh_sprite_font_set_location(lh_sprite_font* f, int x, int y);
void lh_sprite_font_set_align   (lh_sprite_font* f, lh_align a);

int  lh_sprite_font_text_width  (const lh_sprite_font* f, const char* text);
int  lh_sprite_font_get_height  (const lh_sprite_font* f);

void lh_sprite_font_draw(lh_sprite_font* f, lh_graphic* g, int x, int y,
                         lh_align align, const char* text);
void lh_sprite_font_render(const lh_sprite_font* f, lh_graphic* g);

void lh_sprite_font_destroy(lh_sprite_font* f);

#endif
