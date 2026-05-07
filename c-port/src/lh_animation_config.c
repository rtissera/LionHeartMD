/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_animation_config.h"

#include <string.h>

#define NODE_ANIMATIONS "lionengine:animations"
#define NODE_ANIMATION  "lionengine:animation"
#define ATT_NAME        "name"
#define ATT_FIRST       "first"
#define ATT_LAST        "last"
#define ATT_SPEED       "speed"
#define ATT_REVERSE     "reverse"
#define ATT_REPEAT      "repeat"

void lh_animation_config_init(lh_animation_config* cfg)
{
    if (!cfg) return;
    memset(cfg, 0, sizeof(*cfg));
}

int lh_animation_config_import(lh_animation_config* cfg, const lh_xml_node* root)
{
    if (!cfg || !root) return 0;
    lh_animation_config_init(cfg);

    lh_xml_node* anims = lh_xml_node_first_child(root, NODE_ANIMATIONS);
    if (!anims) return 0;

    for (lh_xml_node* a = lh_xml_node_first_child(anims, NODE_ANIMATION);
         a;
         a = lh_xml_node_next_sibling(a, NODE_ANIMATION))
    {
        if (cfg->count >= LH_ANIM_CONFIG_MAX) break;
        lh_anim_entry* e = &cfg->entries[cfg->count];

        const char* name = lh_xml_attr_string(a, ATT_NAME, "");
        snprintf(e->name, sizeof(e->name), "%s", name);

        e->anim.name    = e->name;
        e->anim.first   = lh_xml_attr_int   (a, ATT_FIRST, 1);
        e->anim.last    = lh_xml_attr_int   (a, ATT_LAST,  e->anim.first);
        e->anim.speed   = lh_xml_attr_double(a, ATT_SPEED, 1.0);
        e->anim.reverse = lh_xml_attr_bool  (a, ATT_REVERSE, false);
        e->anim.repeat  = lh_xml_attr_bool  (a, ATT_REPEAT,  false);

        cfg->count++;
    }
    return cfg->count;
}

const lh_animation* lh_animation_config_get(const lh_animation_config* cfg, const char* name)
{
    if (!cfg || !name) return NULL;
    for (int i = 0; i < cfg->count; i++)
    {
        if (strcmp(cfg->entries[i].name, name) == 0) return &cfg->entries[i].anim;
    }
    return NULL;
}
