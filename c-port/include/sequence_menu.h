/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.menu.Menu (skeleton).
 *
 * Java original is a hierarchical menu (LAUNCHER → MAIN → OPTIONS / NEW
 * GAME / CONTINUE / CREDITS, plus splash transitions). Phase 5 ships a
 * minimal main-screen state that accepts SPACE/ENTER to start a new game
 * and ESC to quit. Full menu hierarchy + sprite UI lands phase 9 polish.
 */
#ifndef LH_SEQUENCE_MENU_H
#define LH_SEQUENCE_MENU_H

#include "lh_engine.h"
#include "lh_game_config.h"

lh_sequence* menu_sequence_new(const lh_resolution* res, const lh_game_config* game);

#endif
