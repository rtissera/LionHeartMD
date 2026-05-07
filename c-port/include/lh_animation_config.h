/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.AnimationConfig.
 *
 * Parses an XML <animations> block into a name-keyed table of lh_animation
 * structs. Used by entity setup loaders + sprite font loader.
 *
 * Java XML form:
 *   <lionengine:animations>
 *     <lionengine:animation name="walk" first="1" last="6" speed="0.25"
 *                           reverse="false" repeat="true"/>
 *     ...
 *   </lionengine:animations>
 */
#ifndef LH_ANIMATION_CONFIG_H
#define LH_ANIMATION_CONFIG_H

#include "lh_le_anim.h"
#include "lh_xml.h"

#define LH_ANIM_CONFIG_MAX 64
#define LH_ANIM_NAME_MAX   32

typedef struct {
    char         name [LH_ANIM_NAME_MAX];
    lh_animation anim;       /* anim.name aliases name field */
} lh_anim_entry;

typedef struct {
    lh_anim_entry entries[LH_ANIM_CONFIG_MAX];
    int           count;
} lh_animation_config;

void                lh_animation_config_init(lh_animation_config* cfg);

/* Import animations from a setup-root xml node. Returns count parsed. */
int                 lh_animation_config_import(lh_animation_config* cfg,
                                               const lh_xml_node* setup_root);

const lh_animation* lh_animation_config_get  (const lh_animation_config* cfg,
                                              const char* name);

#endif
