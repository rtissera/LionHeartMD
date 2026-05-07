/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.Scene.
 *
 * lh_sequence wrapper that owns an lh_world. Drives world.update + render
 * each frame, handles ESC to terminate, polls LoadNextStage to chain
 * stage transitions.
 */
#ifndef LH_SCENE_H
#define LH_SCENE_H

#include "lh_engine.h"
#include "lh_game_config.h"
#include "lh_init_config.h"
#include "lh_world.h"

typedef struct {
    lh_sequence    seq;        /* must be first for cast compatibility */
    lh_world       world;
    lh_game_config game;
    lh_init_config init;
    char           stage_buf[256]; /* backing storage for init.stage on transition */
    bool           started;
} lh_scene;

/* Allocate + return as lh_sequence* for lh_loader_run. Caller frees the
 * scene via the sequence's destroy hook. */
lh_sequence* lh_scene_new(const lh_game_config* game, const lh_init_config* init);

#endif
