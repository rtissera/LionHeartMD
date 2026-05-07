/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_sprite.h"
#include <stdlib.h>
#include <string.h>

static lh_sprite* alloc_sprite(lh_image_buffer* buf, lh_sprite_kind kind, bool owns)
{
    lh_sprite* s = (lh_sprite*)calloc(1, sizeof(*s));
    s->kind         = kind;
    s->surface      = buf;
    s->owns_surface = owns;
    s->frame        = 1;
    s->alpha        = 1.0;
    s->transform    = LH_TRANSFORM_IDENTITY;
    lh_animator_init(&s->animator);
    return s;
}

lh_sprite* lh_sprite_load_plain(SDL_Renderer* g, const char* path)
{
    lh_image_buffer* b = lh_image_buffer_load(g, path);
    if (!b) return NULL;
    lh_sprite* s = alloc_sprite(b, LH_SPRITE_PLAIN, true);
    s->tile_w = b->width;
    s->tile_h = b->height;
    s->cols = s->rows = 1;
    return s;
}

lh_sprite* lh_sprite_load_animated(SDL_Renderer* g, const char* path, int cols, int rows)
{
    lh_image_buffer* b = lh_image_buffer_load(g, path);
    if (!b) return NULL;
    lh_sprite* s = alloc_sprite(b, LH_SPRITE_ANIMATED, true);
    s->cols   = cols > 0 ? cols : 1;
    s->rows   = rows > 0 ? rows : 1;
    s->tile_w = b->width  / s->cols;
    s->tile_h = b->height / s->rows;
    return s;
}

lh_sprite* lh_sprite_load_tiled(SDL_Renderer* g, const char* path, int tw, int th)
{
    lh_image_buffer* b = lh_image_buffer_load(g, path);
    if (!b) return NULL;
    lh_sprite* s = alloc_sprite(b, LH_SPRITE_TILED, true);
    s->tile_w = tw > 0 ? tw : b->width;
    s->tile_h = th > 0 ? th : b->height;
    s->cols   = b->width  / s->tile_w;
    s->rows   = b->height / s->tile_h;
    return s;
}

lh_sprite* lh_sprite_from_buffer(lh_image_buffer* buf, lh_sprite_kind kind,
                                 int cols_or_tw, int rows_or_th)
{
    lh_sprite* s = alloc_sprite(buf, kind, false);
    if (kind == LH_SPRITE_PLAIN)
    {
        s->tile_w = buf->width;
        s->tile_h = buf->height;
        s->cols   = 1;
        s->rows   = 1;
    }
    else if (kind == LH_SPRITE_ANIMATED)
    {
        s->cols   = cols_or_tw > 0 ? cols_or_tw : 1;
        s->rows   = rows_or_th > 0 ? rows_or_th : 1;
        s->tile_w = buf->width  / s->cols;
        s->tile_h = buf->height / s->rows;
    }
    else /* TILED */
    {
        s->tile_w = cols_or_tw > 0 ? cols_or_tw : buf->width;
        s->tile_h = rows_or_th > 0 ? rows_or_th : buf->height;
        s->cols   = buf->width  / s->tile_w;
        s->rows   = buf->height / s->tile_h;
    }
    return s;
}

void lh_sprite_set_location(lh_sprite* s, double x, double y)
{
    s->x = x;
    s->y = y;
}

void lh_sprite_set_frame(lh_sprite* s, int frame)
{
    s->frame = frame > 0 ? frame : 1;
}

void lh_sprite_set_frame_offsets(lh_sprite* s, int ox, int oy)
{
    s->offset_x = ox;
    s->offset_y = oy;
}

void lh_sprite_set_mirror(lh_sprite* s, bool h, bool v)
{
    s->mirror_h = h;
    s->mirror_v = v;
}

void lh_sprite_set_alpha(lh_sprite* s, double a)
{
    if (a < 0.0) a = 0.0;
    if (a > 1.0) a = 1.0;
    s->alpha = a;
}

void lh_sprite_set_transform(lh_sprite* s, lh_transform t)
{
    s->transform = t;
}

int lh_sprite_get_width  (const lh_sprite* s) { return s->surface ? s->surface->width  : 0; }
int lh_sprite_get_height (const lh_sprite* s) { return s->surface ? s->surface->height : 0; }
int lh_sprite_get_tile_width (const lh_sprite* s) { return s->tile_w; }
int lh_sprite_get_tile_height(const lh_sprite* s) { return s->tile_h; }

void lh_sprite_play_anim(lh_sprite* s, const lh_animation* anim)
{
    lh_animator_play(&s->animator, anim);
    s->frame = anim->first;
}

void lh_sprite_update_anim(lh_sprite* s, double extrp)
{
    if (s->kind != LH_SPRITE_ANIMATED) return;
    lh_animator_update(&s->animator, extrp);
    s->frame = lh_animator_get_frame(&s->animator);
}

lh_anim_state lh_sprite_anim_state(const lh_sprite* s)
{
    return s->animator.state;
}

void lh_sprite_render(const lh_sprite* s, lh_graphic* g)
{
    if (!s || !s->surface || !s->surface->tex) return;

    SDL_Rect src;
    if (s->kind == LH_SPRITE_PLAIN)
    {
        src = (SDL_Rect){ 0, 0, s->surface->width, s->surface->height };
    }
    else
    {
        const int idx = s->frame - 1;
        const int col = (s->cols > 0) ? (idx % s->cols) : 0;
        const int row = (s->cols > 0) ? (idx / s->cols) : 0;
        src = (SDL_Rect){ col * s->tile_w, row * s->tile_h, s->tile_w, s->tile_h };
    }
    const int dw = (int)(src.w * s->transform.scale_x);
    const int dh = (int)(src.h * s->transform.scale_y);
    SDL_Rect dst = { (int)s->x + s->offset_x, (int)s->y + s->offset_y, dw, dh };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (s->mirror_h) flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
    if (s->mirror_v) flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);

    const uint8_t a = (uint8_t)(s->alpha * (double)g->alpha);
    SDL_SetTextureAlphaMod(s->surface->tex, a);
    SDL_RenderCopyEx(g->sdl, s->surface->tex, &src, &dst, 0.0, NULL, flip);
}

void lh_sprite_destroy(lh_sprite* s)
{
    if (!s) return;
    if (s->owns_surface) lh_image_buffer_destroy(s->surface);
    free(s);
}
