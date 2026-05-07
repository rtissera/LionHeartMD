/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#define _POSIX_C_SOURCE 200809L
#include "lh_asset_cache.h"
#include "lh_medias.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cache_entry {
    char*               key;
    lh_image_buffer*    img;
    struct cache_entry* next;
} cache_entry;

static cache_entry*  s_head     = NULL;
static SDL_Renderer* s_renderer = NULL;
static int           s_count    = 0;

void lh_asset_cache_init(SDL_Renderer* r)
{
    s_renderer = r;
}

void lh_asset_cache_destroy(void)
{
    cache_entry* e = s_head;
    while (e)
    {
        cache_entry* n = e->next;
        if (e->img) lh_image_buffer_destroy(e->img);
        free(e->key);
        free(e);
        e = n;
    }
    s_head     = NULL;
    s_renderer = NULL;
    s_count    = 0;
}

lh_image_buffer* lh_asset_get_image(const char* logical)
{
    if (!logical || !s_renderer) return NULL;

    /* Lookup. */
    for (cache_entry* e = s_head; e; e = e->next)
    {
        if (strcmp(e->key, logical) == 0) return e->img;
    }

    /* Miss — load. */
    char path[LH_MEDIA_PATH_MAX];
    if (!lh_medias_resolve(logical, path, sizeof(path))) return NULL;

    lh_image_buffer* img = lh_image_buffer_load(s_renderer, path);
    if (!img) return NULL;

    cache_entry* e = (cache_entry*)calloc(1, sizeof(*e));
    if (!e) { lh_image_buffer_destroy(img); return NULL; }
    e->key  = strdup(logical);
    e->img  = img;
    e->next = s_head;
    s_head  = e;
    s_count++;
    return img;
}

int lh_asset_cache_count(void) { return s_count; }
