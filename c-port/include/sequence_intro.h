/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.intro.Intro (skeleton).
 *
 * Java original is a four-part cinematic (Stories scroll, palace pan,
 * dragon flight, hero arrival) driven by Time + audio sync. Phase 6
 * skeleton: 4-state machine with fade transitions, placeholder fill per
 * part, skippable on any key. Real artwork + audio sync deferred until
 * the asset cache + sc68 music wire-up are finished.
 */
#ifndef LH_SEQUENCE_INTRO_H
#define LH_SEQUENCE_INTRO_H

#include "lh_engine.h"
#include "lh_game_config.h"

lh_sequence* intro_sequence_new(const lh_resolution* res, const lh_game_config* game);

#endif
