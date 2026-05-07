/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_setup.h"
#include "lh_asset_cache.h"
#include "lh_medias.h"
#include "lh_xml.h"

#include <stdio.h>
#include <string.h>

void lh_setup_init(lh_setup* s, const lh_setup_def* def)
{
    if (!s) return;
    memset(s, 0, sizeof(*s));
    s->def = def;
    if (def && def->media)
    {
        snprintf(s->media_path, sizeof(s->media_path), "%s", def->media);
    }
    s->frames.horizontal = def ? def->frame_h : 1;
    s->frames.vertical   = def ? def->frame_v : 1;
}

void lh_setup_destroy(lh_setup* s)
{
    if (!s) return;
    if (s->doc)
    {
        lh_xml_free(s->doc);
        s->doc = NULL;
    }
    /* surface owned by asset cache — do not free here */
    s->surface = NULL;
}

bool lh_setup_load(lh_setup* s, const char* media)
{
    if (!s || !media) return false;
    snprintf(s->media_path, sizeof(s->media_path), "%s", media);

    s->doc = lh_xml_load(media);
    if (!s->doc) return false;

    const lh_xml_node* root = lh_xml_root(s->doc);
    lh_frames_config_import   (&s->frames, root);
    lh_animation_config_import(&s->anims,  root);

    /* Surface path: <lionengine:surface image="foo.png"/> within node tree.
     * Lionheart convention: image sits next to the entity XML. */
    const lh_xml_node* surf = lh_xml_node_first_child(root, "lionengine:surface");
    if (surf)
    {
        const char* img = lh_xml_attr_string(surf, "image", NULL);
        if (img)
        {
            /* Build image path = directory of media + img basename. */
            char img_path[LH_MEDIA_PATH_MAX];
            const char* slash = strrchr(media, '/');
            if (slash)
            {
                const size_t dir_len = (size_t)(slash - media + 1);
                if (dir_len < sizeof(img_path))
                {
                    memcpy(img_path, media, dir_len);
                    snprintf(img_path + dir_len, sizeof(img_path) - dir_len, "%s", img);
                    s->surface = lh_asset_get_image(img_path);
                }
            }
            else
            {
                s->surface = lh_asset_get_image(img);
            }
        }
    }
    return true;
}

const char* lh_setup_get_media(const lh_setup* s)
{
    return s ? s->media_path : NULL;
}

lh_image_buffer* lh_setup_get_surface(const lh_setup* s)
{
    return s ? s->surface : NULL;
}

int lh_setup_get_frame_h(const lh_setup* s) { return s ? s->frames.horizontal : 0; }
int lh_setup_get_frame_v(const lh_setup* s) { return s ? s->frames.vertical   : 0; }

const lh_xml_node* lh_setup_get_root(const lh_setup* s)
{
    return s ? lh_xml_root(s->doc) : NULL;
}

const lh_animation* lh_setup_get_anim(const lh_setup* s, const char* name)
{
    return s ? lh_animation_config_get(&s->anims, name) : NULL;
}

bool lh_setup_has_node(const lh_setup* s, const char* node)
{
    if (!s || !node) return false;
    return lh_xml_node_has_node(lh_xml_root(s->doc), node);
}
