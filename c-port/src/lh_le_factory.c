/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_factory.h"
#include "lh_le_check.h"
#include <string.h>

void lh_factory_init(lh_factory* f)
{
    if (!f) return;
    memset(f, 0, sizeof(*f));
}

void lh_factory_register(lh_factory* f, const char* key, lh_factory_fn fn)
{
    lh_check_not_null(f);
    lh_check_not_null(key);
    lh_check_not_null((void*)fn);
    if (f->count >= LH_FACTORY_MAX)
    {
        LH_PANIC("factory overflow");
    }
    f->entries[f->count].key = key;
    f->entries[f->count].fn  = fn;
    f->count++;
}

lh_featurable* lh_factory_create(const lh_factory* f, const char* key, const lh_setup* setup)
{
    if (!f || !key) return NULL;
    for (int i = 0; i < f->count; i++)
    {
        if (strcmp(f->entries[i].key, key) == 0)
        {
            return f->entries[i].fn(setup);
        }
    }
    return NULL;
}
