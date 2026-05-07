/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_tile_sheets_config.h"

#include <stdio.h>
#include <string.h>

bool lh_tile_sheets_load(lh_tile_sheets_config* cfg, const char* media)
{
    if (!cfg || !media) return false;
    memset(cfg, 0, sizeof(*cfg));

    lh_xml_doc* doc = lh_xml_load(media);
    if (!doc) return false;

    const lh_xml_node* root = lh_xml_root(doc);
    if (!root || strcmp(lh_xml_node_name(root), LH_TILE_SHEETS_NODE_ROOT) != 0)
    {
        lh_xml_free(doc);
        return false;
    }

    cfg->tile_width  = lh_xml_attr_int(root, "tileWidth",  16);
    cfg->tile_height = lh_xml_attr_int(root, "tileHeight", 16);

    for (lh_xml_node* s = lh_xml_node_first_child((lh_xml_node*)root, LH_TILE_SHEETS_NODE_SHEET);
         s;
         s = lh_xml_node_next_sibling(s, LH_TILE_SHEETS_NODE_SHEET))
    {
        if (cfg->count >= LH_TILE_SHEETS_MAX) break;
        const char* txt = lh_xml_node_text(s);
        if (txt)
        {
            snprintf(cfg->paths[cfg->count], LH_TILE_SHEET_PATH_MAX, "%s", txt);
            cfg->count++;
        }
    }
    lh_xml_free(doc);
    return cfg->count > 0;
}
