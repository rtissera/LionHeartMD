/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_game_config.h"
#include <string.h>

void lh_game_config_default(lh_game_config* c)
{
    if (!c) return;
    memset(c, 0, sizeof(*c));
    c->type         = LH_GAME_STORY;
    c->players      = 1;
    c->stages       = NULL;
    c->one_button   = true;
    c->init_present = false;
    for (int i = 0; i < LH_GAME_MAX_PLAYERS; i++) c->controls[i] = i;
    lh_init_config_default(&c->init);
}

lh_split_type lh_game_config_get_split(const lh_game_config* c)
{
    if (!c || c->players <= 1) return LH_SPLIT_NONE;
    if (c->players == 2) return LH_SPLIT_TWO_HORIZONTAL;
    return LH_SPLIT_FOUR;
}
