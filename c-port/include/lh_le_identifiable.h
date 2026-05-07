/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.Identifiable.
 *
 * Each Featurable gets a unique int id, assignable by an Identifiable
 * feature attached to it. Used by Handler/Services for cross-references.
 */
#ifndef LH_LE_IDENTIFIABLE_H
#define LH_LE_IDENTIFIABLE_H

#include "lh_le_feature.h"

typedef struct {
    lh_feature base;
    int        id;
    int        destroyed; /* 0 = alive, 1 = destroy requested */
} lh_identifiable;

void lh_identifiable_init   (lh_identifiable* i, int id);
int  lh_identifiable_get_id (const lh_identifiable* i);
void lh_identifiable_destroy_request(lh_identifiable* i);
int  lh_identifiable_is_destroyed(const lh_identifiable* i);

/* Global id allocator (mirrors Java IdentifiableModel.unique). */
int  lh_identifiable_next_id(void);
void lh_identifiable_reset_ids(void);

#endif
