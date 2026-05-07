/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_init_config.h"
#include <string.h>

void lh_init_config_default(lh_init_config* c)
{
    if (!c) return;
    memset(c, 0, sizeof(*c));
    c->stage         = NULL;
    c->health_max    = 4;
    c->talisment     = 0;
    c->life          = 2;
    c->sword         = 0;
    c->amulet        = false;
    c->credits       = 0;
    c->difficulty    = LH_DIFFICULTY_NORMAL;
    c->cheats        = false;
    c->spawn_present = false;
    c->spawn_x       = 0.0;
    c->spawn_y       = 0.0;
}

void lh_init_config_basic(lh_init_config* c, const char* stage,
                          int health_max, int life, lh_difficulty diff)
{
    lh_init_config_default(c);
    c->stage      = stage;
    c->health_max = health_max;
    c->life       = life;
    c->difficulty = diff;
}
