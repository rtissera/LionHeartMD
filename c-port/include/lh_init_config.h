/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.InitConfig.
 *
 * Per-stage initial state passed from Menu/AppLionheart to the Scene
 * sequence: which stage to load, hero stats to start with, difficulty,
 * optional spawn override.
 */
#ifndef LH_INIT_CONFIG_H
#define LH_INIT_CONFIG_H

#include <stdbool.h>
#include "lh_difficulty.h"

typedef struct {
    const char*   stage;          /* media path; nullable for menu boot */
    int           health_max;
    int           talisment;
    int           life;
    int           sword;
    bool          amulet;
    int           credits;
    lh_difficulty difficulty;
    bool          cheats;
    /* Optional spawn override: spawn_present=true uses spawn_x/spawn_y. */
    bool          spawn_present;
    double        spawn_x, spawn_y;
} lh_init_config;

void lh_init_config_default(lh_init_config* c);
void lh_init_config_basic  (lh_init_config* c, const char* stage,
                            int health_max, int life, lh_difficulty diff);

#endif
