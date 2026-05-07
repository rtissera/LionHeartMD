/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_body.h"

static void body_update(lh_feature* f, double extrp)
{
    lh_body* b = (lh_body*)f;
    b->direction_v -= b->gravity * extrp;
    if (b->direction_v < -b->gravity_max)
    {
        b->direction_v = -b->gravity_max;
    }
}

static const lh_feature_update_vt s_body_update_vt = {
    .priority      = 0,
    .update_before = NULL,
    .update        = body_update,
    .update_after  = NULL,
};

void lh_body_init(lh_body* b)
{
    lh_feature_init(&b->base, LH_FEAT_BODY, NULL);
    b->base.update_vt = &s_body_update_vt;
    b->gravity     = 0.0;
    b->gravity_max = 0.0;
    b->direction_h = 0.0;
    b->direction_v = 0.0;
}

void lh_body_set_gravity     (lh_body* b, double g)    { if (b) b->gravity     = g;    }
void lh_body_set_gravity_max (lh_body* b, double gmax) { if (b) b->gravity_max = gmax; }
void lh_body_reset_gravity   (lh_body* b)              { if (b) b->direction_v = 0.0;  }
double lh_body_get_direction_h(const lh_body* b)       { return b ? b->direction_h : 0.0; }
double lh_body_get_direction_v(const lh_body* b)       { return b ? b->direction_v : 0.0; }
