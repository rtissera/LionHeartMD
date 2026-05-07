/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_graphic.h"
#include <string.h>

void lh_graphic_init(lh_graphic* g, SDL_Renderer* sdl)
{
    memset(g, 0, sizeof(*g));
    g->sdl   = sdl;
    g->color = LH_COLOR_WHITE;
    g->alpha = 255;
}

static void apply_color(const lh_graphic* g)
{
    SDL_SetRenderDrawColor(g->sdl, g->color.r, g->color.g, g->color.b, g->alpha);
}

void lh_graphic_set_color(lh_graphic* g, lh_color_rgba c)
{
    g->color = c;
    g->alpha = c.a == 0 ? 255 : c.a;
}

void lh_graphic_set_alpha(lh_graphic* g, uint8_t a)
{
    g->alpha   = a;
    g->color.a = a;
}

void lh_graphic_set_color_gradient(lh_graphic* g,
                                   int x1, int y1, lh_color_rgba c1,
                                   int x2, int y2, lh_color_rgba c2)
{
    /* TODO: gradient fill currently flat. Lionheart uses gradient for HUD bars. */
    (void)x1; (void)y1; (void)x2; (void)y2; (void)c2;
    g->color = c1;
}

void lh_graphic_draw_rect(lh_graphic* g, int x, int y, int w, int h, bool fill)
{
    apply_color(g);
    SDL_Rect r = { x, y, w, h };
    if (fill) SDL_RenderFillRect(g->sdl, &r);
    else      SDL_RenderDrawRect(g->sdl, &r);
}

void lh_graphic_fill_rect(lh_graphic* g, int x, int y, int w, int h)
{
    lh_graphic_draw_rect(g, x, y, w, h, true);
}

void lh_graphic_draw_line(lh_graphic* g, int x1, int y1, int x2, int y2)
{
    apply_color(g);
    SDL_RenderDrawLine(g->sdl, x1, y1, x2, y2);
}

void lh_graphic_draw_image(lh_graphic* g, const lh_image_buffer* img, int x, int y)
{
    if (!img || !img->tex) return;
    SDL_SetTextureAlphaMod(img->tex, g->alpha);
    SDL_Rect dst = { x, y, img->width, img->height };
    SDL_RenderCopy(g->sdl, img->tex, NULL, &dst);
}

void lh_graphic_draw_image_region(lh_graphic* g, const lh_image_buffer* img,
                                  int sx, int sy, int sw, int sh,
                                  int dx, int dy)
{
    if (!img || !img->tex) return;
    SDL_SetTextureAlphaMod(img->tex, g->alpha);
    SDL_Rect src = { sx, sy, sw, sh };
    SDL_Rect dst = { dx, dy, sw, sh };
    SDL_RenderCopy(g->sdl, img->tex, &src, &dst);
}

void lh_graphic_draw_image_region_flip(lh_graphic* g, const lh_image_buffer* img,
                                       int sx, int sy, int sw, int sh,
                                       int dx, int dy,
                                       bool flip_h, bool flip_v)
{
    if (!img || !img->tex) return;
    SDL_SetTextureAlphaMod(img->tex, g->alpha);
    SDL_Rect src = { sx, sy, sw, sh };
    SDL_Rect dst = { dx, dy, sw, sh };
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (flip_h) flip |= SDL_FLIP_HORIZONTAL;
    if (flip_v) flip |= SDL_FLIP_VERTICAL;
    SDL_RenderCopyEx(g->sdl, img->tex, &src, &dst, 0.0, NULL, flip);
}

void lh_graphic_push_target(lh_graphic* g, lh_image_buffer* target)
{
    g->prev_target = SDL_GetRenderTarget(g->sdl);
    SDL_SetRenderTarget(g->sdl, target ? target->tex : NULL);
}

void lh_graphic_pop_target(lh_graphic* g)
{
    SDL_SetRenderTarget(g->sdl, g->prev_target);
    g->prev_target = NULL;
}
