/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_collision_group_config.h"

#include <stdio.h>
#include <string.h>

void lh_collision_group_config_init(lh_collision_group_config* cfg)
{
    if (!cfg) return;
    memset(cfg, 0, sizeof(*cfg));
}

bool lh_collision_group_config_load(lh_collision_group_config* cfg,
                                    const char* media)
{
    if (!cfg || !media) return false;
    lh_collision_group_config_init(cfg);

    lh_xml_doc* doc = lh_xml_load(media);
    if (!doc) return false;

    const lh_xml_node* root = lh_xml_root(doc);
    if (!root || strcmp(lh_xml_node_name(root), LH_COLL_GROUPS_NODE_ROOT) != 0)
    {
        lh_xml_free(doc);
        return false;
    }

    for (lh_xml_node* g = lh_xml_node_first_child((lh_xml_node*)root,
                                                  LH_COLL_GROUPS_NODE_COLLISION);
         g;
         g = lh_xml_node_next_sibling(g, LH_COLL_GROUPS_NODE_COLLISION))
    {
        if (cfg->count >= LH_COLL_MAX_GROUPS) break;
        lh_coll_group* e = &cfg->entries[cfg->count];
        memset(e, 0, sizeof(*e));

        const char* name = lh_xml_attr_string(g, "group", "");
        snprintf(e->name, sizeof(e->name), "%s", name);

        for (lh_xml_node* fn = lh_xml_node_first_child(g,
                                                       LH_COLL_GROUPS_NODE_FORMULA_REF);
             fn;
             fn = lh_xml_node_next_sibling(fn, LH_COLL_GROUPS_NODE_FORMULA_REF))
        {
            if (e->formula_count >= LH_COLL_MAX_FORMULAS_PER_GROUP) break;
            const char* txt = lh_xml_node_text(fn);
            if (!txt) continue;
            snprintf(e->formulas[e->formula_count],
                     LH_COLL_FORMULA_REF_NAME_MAX, "%s", txt);
            e->formula_count++;
        }
        cfg->count++;
    }
    lh_xml_free(doc);
    return cfg->count > 0;
}

const lh_coll_group* lh_collision_group_config_get(
        const lh_collision_group_config* cfg, const char* name)
{
    if (!cfg || !name) return NULL;
    for (int i = 0; i < cfg->count; i++)
    {
        if (strcmp(cfg->entries[i].name, name) == 0) return &cfg->entries[i];
    }
    return NULL;
}
