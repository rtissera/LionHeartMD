/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.collidable.Collidable
 *   com.b3dgs.lionengine.game.feature.collidable.CollidableModel
 *   com.b3dgs.lionengine.game.feature.collidable.Collision
 *   com.b3dgs.lionengine.game.feature.collidable.CollidableListener
 *
 * Hitbox-bearing feature. Each Collidable owns one or more Collisions
 * (named rectangles, optionally mirror-aware). ComponentCollision (TBD,
 * phase 2c+) iterates Collidables and invokes listeners on overlap.
 */
#ifndef LH_LE_COLLIDABLE_H
#define LH_LE_COLLIDABLE_H

#include <stdbool.h>
#include "lh_le_feature.h"

#define LH_COLLISION_NAME_MAX 32
#define LH_COLLIDABLE_MAX_BOXES 8
#define LH_COLLIDABLE_MAX_LISTENERS 4

typedef struct lh_collision {
    char  name[LH_COLLISION_NAME_MAX];
    int   offset_x, offset_y;
    int   width,    height;
    bool  mirror;
} lh_collision;

typedef void (*lh_collidable_listener)(lh_feature* self,
                                       lh_feature* other,
                                       const lh_collision* with,
                                       const lh_collision* by);

typedef struct {
    lh_feature             base;
    lh_collision           boxes[LH_COLLIDABLE_MAX_BOXES];
    int                    box_count;
    lh_collidable_listener listeners[LH_COLLIDABLE_MAX_LISTENERS];
    int                    listener_count;
    bool                   enabled;
    bool                   debug_visible;
    int                    group;            /* collision group bitmask */
    int                    accept_mask;      /* groups we accept collision from */
} lh_collidable;

void lh_collidable_init        (lh_collidable* c);
void lh_collidable_set_enabled (lh_collidable* c, bool enabled);
void lh_collidable_set_visibility(lh_collidable* c, bool visible);
void lh_collidable_set_group   (lh_collidable* c, int group);
void lh_collidable_set_accept  (lh_collidable* c, int accept_mask);
void lh_collidable_add_box     (lh_collidable* c, const lh_collision* box);
void lh_collidable_clear_boxes (lh_collidable* c);
void lh_collidable_add_listener(lh_collidable* c, lh_collidable_listener l);

/* Test if two collidables overlap given their current Transformable
 * positions. Returns the index of the colliding pair into self->boxes
 * (0..box_count-1) or -1 if no collision. Used by ComponentCollision. */
int  lh_collidable_test_overlap(const lh_collidable* a, double ax, double ay,
                                const lh_collidable* b, double bx, double by);

/* Notify all attached listeners with the given collision pair. */
void lh_collidable_notify_collided(lh_collidable* self, lh_feature* other,
                                   const lh_collision* with, const lh_collision* by);

#endif
