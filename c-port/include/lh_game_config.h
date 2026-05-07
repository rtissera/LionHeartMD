/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.GameConfig.
 *
 * Top-level game configuration set at boot. Network strip applied —
 * Optional<Network> field removed (single-player only).
 */
#ifndef LH_GAME_CONFIG_H
#define LH_GAME_CONFIG_H

#include <stdbool.h>
#include "lh_game_type.h"
#include "lh_init_config.h"

#define LH_GAME_MAX_PLAYERS 4

typedef enum {
    LH_SPLIT_NONE = 0,
    LH_SPLIT_TWO_HORIZONTAL,
    LH_SPLIT_FOUR
} lh_split_type;

typedef struct {
    lh_game_type   type;
    int            players;
    const char*    stages;            /* nullable */
    bool           one_button;
    int            controls[LH_GAME_MAX_PLAYERS];   /* control index per player */
    lh_init_config init;
    bool           init_present;       /* false = menu boot, true = direct stage */
} lh_game_config;

void lh_game_config_default(lh_game_config* c);

lh_split_type lh_game_config_get_split(const lh_game_config* c);

#endif
