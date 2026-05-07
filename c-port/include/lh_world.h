/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.World.
 *
 * Top-level scene container. Owns services registry, handler, camera +
 * tracker, factory, map, landscape, checkpoints, and stage transition
 * state. update/render dispatch through Handler routine vtables.
 *
 * Phase 7 skeleton: load_stage parses XML + loads tiles but defers
 * factory entity spawning to phase 8 (Factory registry + 121 entity
 * types still empty). World boots cleanly, draws map + landscape, and
 * tracks the active camera.
 */
#ifndef LH_WORLD_H
#define LH_WORLD_H

#include <stdbool.h>

#include "lh_checkpoint_handler.h"
#include "lh_collision_formula_config.h"
#include "lh_collision_group_config.h"
#include "lh_game_config.h"
#include "lh_init_config.h"
#include "lh_landscape.h"
#include "lh_le_camera.h"
#include "lh_le_camera_tracker.h"
#include "lh_le_factory.h"
#include "lh_le_graphic.h"
#include "lh_le_handler.h"
#include "lh_le_image_buffer.h"
#include "lh_le_map_tile.h"
#include "lh_le_services.h"
#include "lh_load_next_stage.h"
#include "lh_stage_config.h"
#include "lh_tile_groups_config.h"
#include "lh_tile_sheets_config.h"

typedef struct {
    /* Aggregated state. */
    lh_services           services;
    lh_handler            handler;
    lh_camera             camera;
    lh_camera_tracker     tracker;
    lh_factory            factory;
    lh_map_tile           map;
    lh_landscape          landscape;
    lh_checkpoint_handler checkpoints;
    lh_load_next_stage    next_stage;

    /* Per-stage configs (populated on load_stage). */
    lh_stage_config             stage;
    lh_tile_sheets_config       sheets;
    lh_collision_formula_config formulas;
    lh_collision_group_config   groups;
    lh_tile_groups_config       tile_groups;

    /* Sheet PNG surfaces (asset cache, NOT owned). Indexed by sheet id. */
    lh_image_buffer*      sheet_surfaces[LH_TILE_SHEETS_MAX];

    /* Game-level inputs. */
    lh_game_config        game;
    lh_init_config        init;

    bool                  paused;
    bool                  loaded;
} lh_world;

bool lh_world_init       (lh_world* w,
                          const lh_game_config* game,
                          const lh_init_config* init,
                          int view_w, int view_h);
bool lh_world_load_stage (lh_world* w, const char* media);
void lh_world_update     (lh_world* w, double extrp);
void lh_world_render     (lh_world* w, lh_graphic* g);
void lh_world_destroy    (lh_world* w);

void lh_world_set_paused (lh_world* w, bool p);
bool lh_world_is_paused  (const lh_world* w);

#endif
