/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Phase 2b minimal: fixed-pitch path, XML widths file deferred to phase 4
 * (asset pipeline) — function exposed but data_path currently unused.
 */
#include "lh_le_sprite_font.h"
#include <stdlib.h>
#include <string.h>

lh_sprite_font* lh_sprite_font_load(SDL_Renderer* g,
                                    const char*   img_path,
                                    const char*   data_path,
                                    int           tile_w,
                                    int           tile_h)
{
    (void)data_path; /* TODO phase 4: parse XML char-widths */
    lh_image_buffer* b = lh_image_buffer_load(g, img_path);
    if (!b) return NULL;
    lh_sprite_font* f = (lh_sprite_font*)calloc(1, sizeof(*f));
    f->surface       = b;
    f->owns_surface  = true;
    f->tile_w        = tile_w;
    f->tile_h        = tile_h;
    f->line_height   = tile_h;
    f->align         = LH_ALIGN_LEFT;
    for (int i = 0; i < LH_FONT_CHARS; i++)
        f->widths[i] = tile_w;
    return f;
}

void lh_sprite_font_set_location(lh_sprite_font* f, int x, int y)
{
    f->text_x = x;
    f->text_y = y;
}

void lh_sprite_font_set_align(lh_sprite_font* f, lh_align a)
{
    f->align = a;
}

int lh_sprite_font_text_width(const lh_sprite_font* f, const char* text)
{
    if (!text) return 0;
    int w = 0;
    for (const unsigned char* p = (const unsigned char*)text; *p; p++)
        w += f->widths[*p];
    return w;
}

int lh_sprite_font_get_height(const lh_sprite_font* f)
{
    return f->line_height;
}

void lh_sprite_font_draw(lh_sprite_font* f, lh_graphic* g, int x, int y,
                         lh_align align, const char* text)
{
    if (!f || !text || !f->surface) return;

    int draw_x = x;
    if (align == LH_ALIGN_CENTER)
        draw_x = x - lh_sprite_font_text_width(f, text) / 2;
    else if (align == LH_ALIGN_RIGHT)
        draw_x = x - lh_sprite_font_text_width(f, text);

    /* Atlas assumed: 16x16 grid of glyphs (font ASCII). */
    const int cols = f->surface->width  / f->tile_w;
    if (cols <= 0) return;

    int cx = draw_x;
    for (const unsigned char* p = (const unsigned char*)text; *p; p++)
    {
        const int idx = (int)*p;
        const int col = idx % cols;
        const int row = idx / cols;
        lh_graphic_draw_image_region(g, f->surface,
                                     col * f->tile_w, row * f->tile_h,
                                     f->tile_w,        f->tile_h,
                                     cx, y);
        cx += f->widths[idx];
    }
}

void lh_sprite_font_render(const lh_sprite_font* f, lh_graphic* g)
{
    /* No queued text in this minimal port; Lionheart calls draw() directly. */
    (void)f; (void)g;
}

void lh_sprite_font_destroy(lh_sprite_font* f)
{
    if (!f) return;
    if (f->owns_surface) lh_image_buffer_destroy(f->surface);
    free(f);
}
