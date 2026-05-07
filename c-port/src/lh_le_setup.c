/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_setup.h"
#include <string.h>

void lh_setup_init(lh_setup* s, const lh_setup_def* def)
{
    if (!s) return;
    memset(s, 0, sizeof(*s));
    s->def     = def;
    s->surface = NULL;   /* phase 4 asset pipeline loads PNG via def->surface_path */
}

void lh_setup_destroy(lh_setup* s)
{
    if (!s) return;
    if (s->surface)
    {
        lh_image_buffer_destroy(s->surface);
        s->surface = NULL;
    }
}

const char* lh_setup_get_media(const lh_setup* s)
{
    return (s && s->def) ? s->def->media : NULL;
}

lh_image_buffer* lh_setup_get_surface(const lh_setup* s)
{
    return s ? s->surface : NULL;
}

int lh_setup_get_frame_h(const lh_setup* s) { return (s && s->def) ? s->def->frame_h : 0; }
int lh_setup_get_frame_v(const lh_setup* s) { return (s && s->def) ? s->def->frame_v : 0; }
