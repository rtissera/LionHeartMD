/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_featurable.h"
#include "lh_le_check.h"
#include <string.h>

void lh_featurable_init(lh_featurable* f)
{
    memset(f, 0, sizeof(*f));
}

void lh_featurable_destroy(lh_featurable* f)
{
    if (!f) return;
    for (int i = 0; i < LH_FEAT_MAX; i++)
    {
        lh_feature* feat = f->features[i];
        if (feat && feat->vt && feat->vt->destroy)
        {
            feat->vt->destroy(feat);
        }
        f->features[i] = NULL;
    }
}

lh_feature* lh_featurable_add_feature(lh_featurable* f, lh_feature* feat)
{
    lh_check_not_null(f);
    lh_check_not_null(feat);
    if (feat->id <= LH_FEAT_NONE || feat->id >= LH_FEAT_MAX)
    {
        LH_PANIC("invalid feature id");
    }
    if (f->features[feat->id])
    {
        LH_PANIC("feature already registered");
    }
    f->features[feat->id] = feat;
    feat->owner = f;
    return feat;
}

lh_feature* lh_featurable_get_feature(const lh_featurable* f, lh_feature_id id)
{
    if (!f || id <= LH_FEAT_NONE || id >= LH_FEAT_MAX) return NULL;
    return f->features[id];
}

bool lh_featurable_has_feature(const lh_featurable* f, lh_feature_id id)
{
    return lh_featurable_get_feature(f, id) != NULL;
}

void lh_featurable_prepare(lh_featurable* f)
{
    if (!f) return;
    for (int i = 0; i < LH_FEAT_MAX; i++)
    {
        lh_feature* feat = f->features[i];
        if (feat && feat->vt && feat->vt->prepare)
        {
            feat->vt->prepare(feat, f);
        }
    }
}

void lh_featurable_recycle(lh_featurable* f)
{
    if (!f) return;
    for (int i = 0; i < LH_FEAT_MAX; i++)
    {
        lh_feature* feat = f->features[i];
        if (feat && feat->vt && feat->vt->recycle)
        {
            feat->vt->recycle(feat);
        }
    }
}
