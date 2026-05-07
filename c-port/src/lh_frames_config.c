/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_frames_config.h"
#include <string.h>

bool lh_frames_config_import(lh_frames_config* fc, const lh_xml_node* root)
{
    if (!fc || !root) return false;
    memset(fc, 0, sizeof(*fc));

    lh_xml_node* n = lh_xml_node_first_child(root, LH_NODE_FRAMES);
    if (!n) return false;

    fc->horizontal = lh_xml_attr_int(n, "horizontal", 1);
    fc->vertical   = lh_xml_attr_int(n, "vertical",   1);
    fc->offset_x   = lh_xml_attr_int(n, "offsetX",    0);
    fc->offset_y   = lh_xml_attr_int(n, "offsetY",    0);
    return true;
}
