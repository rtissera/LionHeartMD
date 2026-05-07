/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.Transformable
 *   com.b3dgs.lionengine.game.feature.TransformableModel
 *
 * Position + size + previous position. moveLocation accumulates direction
 * vectors with extrapolation; teleport jumps without recording oldX/oldY.
 */
#ifndef LH_LE_TRANSFORMABLE_H
#define LH_LE_TRANSFORMABLE_H

#include "lh_le_feature.h"

typedef struct {
    lh_feature base;
    double x,  y;
    double ox, oy;     /* previous frame */
    int    width, height;
} lh_transformable;

void   lh_transformable_init       (lh_transformable* t);

void   lh_transformable_set_size   (lh_transformable* t, int w, int h);
int    lh_transformable_get_width  (const lh_transformable* t);
int    lh_transformable_get_height (const lh_transformable* t);

double lh_transformable_get_x      (const lh_transformable* t);
double lh_transformable_get_y      (const lh_transformable* t);
double lh_transformable_get_old_x  (const lh_transformable* t);
double lh_transformable_get_old_y  (const lh_transformable* t);

/* Set new pos, recording prior as old. */
void   lh_transformable_set_location(lh_transformable* t, double x, double y);
void   lh_transformable_set_x       (lh_transformable* t, double x);
void   lh_transformable_set_y       (lh_transformable* t, double y);

/* Jump without updating oldX/oldY (useful for spawn / clamp). */
void   lh_transformable_teleport    (lh_transformable* t, double x, double y);
void   lh_transformable_teleport_x  (lh_transformable* t, double x);
void   lh_transformable_teleport_y  (lh_transformable* t, double y);

/* Move by direction × extrp (mirrors Java moveLocation). */
void   lh_transformable_move        (lh_transformable* t, double extrp, double dx, double dy);

/* Commit current pos as old (call once per frame after physics). */
void   lh_transformable_check       (lh_transformable* t);

#endif
