/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#define _POSIX_C_SOURCE 200809L
#include "lh_tile_groups_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int int_cmp(const void* a, const void* b)
{
    int x = *(const int*)a;
    int y = *(const int*)b;
    return (x > y) - (x < y);
}

/* Collapse the (now-sorted) tile list of one group into ranges and
 * append them to cfg->ranges. */
static void emit_ranges(lh_tile_groups_config* cfg, const lh_tile_group* g)
{
    int i = 0;
    while (i < g->tile_count)
    {
        int first = g->tiles[i];
        int last  = first;
        int j     = i + 1;
        while (j < g->tile_count && g->tiles[j] == last + 1)
        {
            last = g->tiles[j];
            j++;
        }
        if (cfg->range_count < LH_TILE_GROUP_RANGES_MAX)
        {
            lh_tile_group_range* r = &cfg->ranges[cfg->range_count++];
            /* g->name and r->group_name are distinct fields; use memcpy to
             * sidestep a -Wrestrict false positive caused by inlining. */
            size_t n = strnlen(g->name, sizeof(r->group_name) - 1);
            memcpy(r->group_name, g->name, n);
            r->group_name[n] = '\0';
            r->sheet_id   = 0;
            r->first_tile = first;
            r->last_tile  = last;
        }
        i = j;
    }
}

void lh_tile_groups_config_init(lh_tile_groups_config* cfg)
{
    if (!cfg) return;
    memset(cfg, 0, sizeof(*cfg));
}

bool lh_tile_groups_config_load(lh_tile_groups_config* cfg, const char* media)
{
    if (!cfg || !media) return false;
    lh_tile_groups_config_init(cfg);

    lh_xml_doc* doc = lh_xml_load(media);
    if (!doc) return false;

    const lh_xml_node* root = lh_xml_root(doc);
    if (!root || strcmp(lh_xml_node_name(root), LH_TILE_GROUPS_NODE_ROOT) != 0)
    {
        lh_xml_free(doc);
        return false;
    }

    for (lh_xml_node* g = lh_xml_node_first_child((lh_xml_node*)root,
                                                  LH_TILE_GROUPS_NODE_GROUP);
         g;
         g = lh_xml_node_next_sibling(g, LH_TILE_GROUPS_NODE_GROUP))
    {
        if (cfg->group_count >= LH_TILE_GROUPS_MAX) break;
        lh_tile_group* e = &cfg->groups[cfg->group_count];
        memset(e, 0, sizeof(*e));

        snprintf(e->name, sizeof(e->name), "%s",
                 lh_xml_attr_string(g, "name", ""));
        snprintf(e->type, sizeof(e->type), "%s",
                 lh_xml_attr_string(g, "type", "NONE"));

        for (lh_xml_node* t = lh_xml_node_first_child(g,
                                                      LH_TILE_GROUPS_NODE_TILE);
             t;
             t = lh_xml_node_next_sibling(t, LH_TILE_GROUPS_NODE_TILE))
        {
            if (e->tile_count >= LH_TILES_PER_GROUP_MAX) break;
            e->tiles[e->tile_count++] = lh_xml_attr_int(t, "number", 0);
        }

        if (e->tile_count > 1)
        {
            qsort(e->tiles, (size_t)e->tile_count, sizeof(int), int_cmp);
        }
        emit_ranges(cfg, e);
        cfg->group_count++;
    }
    lh_xml_free(doc);
    return cfg->group_count > 0;
}

const lh_tile_group* lh_tile_groups_config_get(
        const lh_tile_groups_config* cfg, const char* group_name)
{
    if (!cfg || !group_name) return NULL;
    for (int i = 0; i < cfg->group_count; i++)
    {
        if (strcmp(cfg->groups[i].name, group_name) == 0) return &cfg->groups[i];
    }
    return NULL;
}

const char* lh_tile_groups_config_find_for(
        const lh_tile_groups_config* cfg, int tile_number)
{
    if (!cfg) return NULL;
    for (int i = 0; i < cfg->range_count; i++)
    {
        const lh_tile_group_range* r = &cfg->ranges[i];
        if (tile_number >= r->first_tile && tile_number <= r->last_tile)
            return r->group_name;
    }
    return NULL;
}
