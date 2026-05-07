/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_transformable.h"

void lh_transformable_init(lh_transformable* t)
{
    lh_feature_init(&t->base, LH_FEAT_TRANSFORMABLE, NULL);
    t->x = t->y = 0.0;
    t->ox = t->oy = 0.0;
    t->width = t->height = 0;
}

void lh_transformable_set_size(lh_transformable* t, int w, int h)
{
    if (!t) return;
    t->width  = w;
    t->height = h;
}

int lh_transformable_get_width  (const lh_transformable* t) { return t ? t->width  : 0; }
int lh_transformable_get_height (const lh_transformable* t) { return t ? t->height : 0; }
double lh_transformable_get_x   (const lh_transformable* t) { return t ? t->x  : 0.0; }
double lh_transformable_get_y   (const lh_transformable* t) { return t ? t->y  : 0.0; }
double lh_transformable_get_old_x(const lh_transformable* t){ return t ? t->ox : 0.0; }
double lh_transformable_get_old_y(const lh_transformable* t){ return t ? t->oy : 0.0; }

void lh_transformable_set_location(lh_transformable* t, double x, double y)
{
    if (!t) return;
    t->ox = t->x;
    t->oy = t->y;
    t->x  = x;
    t->y  = y;
}

void lh_transformable_set_x(lh_transformable* t, double x)
{
    if (!t) return;
    t->ox = t->x;
    t->x  = x;
}

void lh_transformable_set_y(lh_transformable* t, double y)
{
    if (!t) return;
    t->oy = t->y;
    t->y  = y;
}

void lh_transformable_teleport(lh_transformable* t, double x, double y)
{
    if (!t) return;
    t->x  = x;  t->y  = y;
    t->ox = x;  t->oy = y;
}

void lh_transformable_teleport_x(lh_transformable* t, double x)
{
    if (!t) return;
    t->x  = x;
    t->ox = x;
}

void lh_transformable_teleport_y(lh_transformable* t, double y)
{
    if (!t) return;
    t->y  = y;
    t->oy = y;
}

void lh_transformable_move(lh_transformable* t, double extrp, double dx, double dy)
{
    if (!t) return;
    t->ox = t->x;
    t->oy = t->y;
    t->x += dx * extrp;
    t->y += dy * extrp;
}

void lh_transformable_check(lh_transformable* t)
{
    if (!t) return;
    t->ox = t->x;
    t->oy = t->y;
}
