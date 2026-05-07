/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.Spawner.
 *
 * Service interface for spawning featurables at runtime. The C port keeps
 * it as a function-pointer typedef plus state pointer; concrete impl lives
 * in Lionheart's World/Scene at phase 7. Held in Services under
 * LH_SVC_SPAWNER as an lh_spawner*.
 */
#ifndef LH_LE_SPAWNER_H
#define LH_LE_SPAWNER_H

#include "lh_le_featurable.h"

typedef struct lh_spawner lh_spawner;

/* Spawn an entity from media path (XML resource id) at world (x,y).
 * Returns the new featurable, or NULL on failure. Concrete spawner caches
 * setup data and routes via Factory + Handler. */
typedef lh_featurable* (*lh_spawn_fn)(lh_spawner* self, const char* media, double x, double y);

struct lh_spawner {
    lh_spawn_fn spawn;
    void*       user;     /* concrete impl state */
};

static inline lh_featurable* lh_spawner_spawn(lh_spawner* s, const char* media, double x, double y)
{
    return (s && s->spawn) ? s->spawn(s, media, x, y) : (lh_featurable*)0;
}

#endif
