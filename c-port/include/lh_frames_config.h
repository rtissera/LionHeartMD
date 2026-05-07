/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.FramesConfig.
 * Reads sprite sheet frame layout from a setup XML.
 *   <lionengine:frames horizontal="N" vertical="M" offsetX="..." offsetY="..."/>
 */
#ifndef LH_FRAMES_CONFIG_H
#define LH_FRAMES_CONFIG_H

#include "lh_xml.h"

#define LH_NODE_FRAMES "lionengine:frames"

typedef struct {
    int horizontal;
    int vertical;
    int offset_x;
    int offset_y;
} lh_frames_config;

bool lh_frames_config_import(lh_frames_config* fc, const lh_xml_node* setup_root);

#endif
