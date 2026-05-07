/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.Services.
 *
 * Java's reflection-keyed Map<Class<?>, Object> becomes a closed-world enum
 * (lh_service_id) indexing a flat pointer array. Each game-wide singleton
 * (Camera, Handler, Factory, Spawner, etc.) registers under its own id.
 *
 * Codegen target: gen_dispatch.py reads `Services.add(Class, ...)` callsites
 * and emits the authoritative id list. For now the subset Lionheart Loading
 * and Menu need.
 */
#ifndef LH_LE_SERVICES_H
#define LH_LE_SERVICES_H

#include <stdbool.h>

typedef enum {
    LH_SVC_NONE = 0,
    LH_SVC_FACTORY,
    LH_SVC_HANDLER,
    LH_SVC_SPAWNER,
    LH_SVC_CAMERA,
    LH_SVC_CAMERA_TRACKER,
    LH_SVC_VIEWER,
    LH_SVC_MAP_TILE,
    LH_SVC_MAP_TILE_GROUP,
    LH_SVC_MAP_TILE_COLLISION,
    LH_SVC_MAP_TILE_VIEWER,
    LH_SVC_MAP_TILE_PERSISTER,
    LH_SVC_MAP_TILE_TRANSITION,
    LH_SVC_MAP_TILE_RASTERED,
    LH_SVC_MAP_TILE_HELPER,
    LH_SVC_MAP_TILE_WATER,
    LH_SVC_HUD,
    LH_SVC_RASTERBAR,
    LH_SVC_TICK,
    LH_SVC_GAME_CONFIG,
    LH_SVC_INIT_CONFIG,
    LH_SVC_SETTINGS,
    LH_SVC_DEVICE_CONTROLLER,
    LH_SVC_TIME,
    LH_SVC_LOAD_NEXT_STAGE,
    LH_SVC_CHECKPOINT_HANDLER,
    LH_SVC_TRACKABLE,
    LH_SVC_SOURCE_RESOLUTION,
    LH_SVC_SEQUENCER,
    LH_SVC_CHEATS,
    LH_SVC_MUSIC_PLAYER,
    LH_SVC_MAX = 64
} lh_service_id;

typedef struct {
    void* slots[LH_SVC_MAX];
} lh_services;

void  lh_services_init   (lh_services* s);
void  lh_services_destroy(lh_services* s);

/* Register a singleton. Returns the same pointer for chaining. */
void* lh_services_add    (lh_services* s, lh_service_id id, void* obj);
void* lh_services_get    (const lh_services* s, lh_service_id id);  /* panic on missing */
void* lh_services_get_opt(const lh_services* s, lh_service_id id);  /* NULL on missing */
bool  lh_services_has    (const lh_services* s, lh_service_id id);

#endif
