/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.helper.EntityChecker
 *   com.b3dgs.lionengine.helper.EntityCheckerListener
 *
 * Viewport-culling helper. Two predicates: is the entity in the active
 * update zone? Is it in the active render zone? Notifies listeners on
 * transition. Used by EntityHelper to short-circuit per-entity work.
 */
#ifndef LH_LE_ENTITY_CHECKER_H
#define LH_LE_ENTITY_CHECKER_H

#include <stdbool.h>
#include "lh_le_feature.h"

#define LH_ENTITY_CHECKER_MAX_LISTENERS 4

typedef bool (*lh_entity_checker_pred)(void* user);
typedef void (*lh_entity_checker_listener)(bool checked, void* user);

typedef struct {
    lh_feature                 base;
    lh_entity_checker_pred     update_pred;
    lh_entity_checker_pred     render_pred;
    void*                      user;
    bool                       last_update;
    bool                       last_render;
    lh_entity_checker_listener update_listeners[LH_ENTITY_CHECKER_MAX_LISTENERS];
    lh_entity_checker_listener render_listeners[LH_ENTITY_CHECKER_MAX_LISTENERS];
    int                        update_listener_count;
    int                        render_listener_count;
} lh_entity_checker;

void lh_entity_checker_init    (lh_entity_checker* c, void* user);
void lh_entity_checker_set_update(lh_entity_checker* c, lh_entity_checker_pred p);
void lh_entity_checker_set_render(lh_entity_checker* c, lh_entity_checker_pred p);
void lh_entity_checker_add_update_listener(lh_entity_checker* c, lh_entity_checker_listener l);
void lh_entity_checker_add_render_listener(lh_entity_checker* c, lh_entity_checker_listener l);
void lh_entity_checker_update  (lh_entity_checker* c, double extrp);
bool lh_entity_checker_is_update_checked(const lh_entity_checker* c);
bool lh_entity_checker_is_render_checked(const lh_entity_checker* c);

#endif
