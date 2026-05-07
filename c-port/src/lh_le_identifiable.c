/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_identifiable.h"

static int s_next_id = 1;

void lh_identifiable_init(lh_identifiable* i, int id)
{
    lh_feature_init(&i->base, LH_FEAT_IDENTIFIABLE, NULL);
    i->id        = id > 0 ? id : lh_identifiable_next_id();
    i->destroyed = 0;
}

int lh_identifiable_get_id(const lh_identifiable* i)
{
    return i ? i->id : -1;
}

void lh_identifiable_destroy_request(lh_identifiable* i)
{
    if (i) i->destroyed = 1;
}

int lh_identifiable_is_destroyed(const lh_identifiable* i)
{
    return i ? i->destroyed : 0;
}

int lh_identifiable_next_id(void)
{
    return s_next_id++;
}

void lh_identifiable_reset_ids(void)
{
    s_next_id = 1;
}
