/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_landscape.h"
#include <string.h>

void lh_landscape_init(lh_landscape* l, int view_w, int view_h)
{
    if (!l) return;
    memset(l, 0, sizeof(*l));
    l->view_width  = view_w;
    l->view_height = view_h;
}

void lh_landscape_destroy(lh_landscape* l)
{
    if (!l) return;
    /* Surfaces owned by asset cache — caller frees that. */
    memset(l, 0, sizeof(*l));
}

bool lh_landscape_load(lh_landscape* l, const char* media)
{
    if (!l) return false;
    (void)media;
    /* TODO phase 8: parse landscape XML (name, parallax layers, water tile,
     * raster file). For now the caller manually populates layers via
     * lh_landscape_add_bg/fg. */
    return true;
}

static void add_layer(lh_landscape_layer* slot, lh_image_buffer* surface,
                      double px, double py, int base_y, int tile_w)
{
    slot->surface    = surface;
    slot->parallax_x = px;
    slot->parallax_y = py;
    slot->base_y     = base_y;
    slot->tile_w     = tile_w;
    slot->enabled    = surface != NULL;
}

void lh_landscape_add_bg(lh_landscape* l, lh_image_buffer* surface,
                         double px, double py, int base_y, int tile_w)
{
    if (!l || l->bg_count >= LH_LANDSCAPE_BG_LAYERS) return;
    add_layer(&l->bg[l->bg_count++], surface, px, py, base_y, tile_w);
}

void lh_landscape_add_fg(lh_landscape* l, lh_image_buffer* surface,
                         double px, double py, int base_y, int tile_w)
{
    if (!l || l->fg_count >= LH_LANDSCAPE_FG_LAYERS) return;
    add_layer(&l->fg[l->fg_count++], surface, px, py, base_y, tile_w);
}

static void render_layers(const lh_landscape_layer* layers, int count,
                          lh_graphic* g, const lh_camera* cam,
                          int view_w, int view_h)
{
    if (!layers || !g || !cam || count <= 0) return;
    const double cx = cam ? cam->x : 0.0;
    const double cy = cam ? cam->y : 0.0;
    for (int i = 0; i < count; i++)
    {
        const lh_landscape_layer* L = &layers[i];
        if (!L->enabled || !L->surface) continue;

        const int sw = L->surface->width;
        const int sh = L->surface->height;
        if (sw <= 0 || sh <= 0) continue;

        const int draw_y = L->base_y - (int)(cy * L->parallax_y);
        if (L->tile_w > 0)
        {
            /* Repeat horizontally to cover view. */
            int sx = -(int)(cx * L->parallax_x) % L->tile_w;
            if (sx > 0) sx -= L->tile_w;
            for (int x = sx; x < view_w; x += L->tile_w)
            {
                lh_graphic_draw_image(g, L->surface, x, draw_y);
            }
        }
        else
        {
            const int draw_x = -(int)(cx * L->parallax_x);
            lh_graphic_draw_image(g, L->surface, draw_x, draw_y);
        }
        (void)sh; (void)view_h;
    }
}

void lh_landscape_render_background(const lh_landscape* l, lh_graphic* g, const lh_camera* cam)
{
    if (!l) return;
    render_layers(l->bg, l->bg_count, g, cam, l->view_width, l->view_height);
}

void lh_landscape_render_foreground(const lh_landscape* l, lh_graphic* g, const lh_camera* cam)
{
    if (!l) return;
    render_layers(l->fg, l->fg_count, g, cam, l->view_width, l->view_height);
}

void lh_landscape_update(lh_landscape* l, double extrp, const lh_camera* cam)
{
    (void)l; (void)extrp; (void)cam;
    /* No animated landscape state in skeleton. Phase 7+ may add water
     * surface ripple, animated foreground decals here. */
}
