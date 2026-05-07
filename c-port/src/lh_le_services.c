/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_services.h"
#include "lh_le_check.h"
#include <string.h>

void lh_services_init(lh_services* s)
{
    memset(s, 0, sizeof(*s));
}

void lh_services_destroy(lh_services* s)
{
    if (!s) return;
    /* Services do not own the singletons they reference. Caller frees. */
    memset(s, 0, sizeof(*s));
}

void* lh_services_add(lh_services* s, lh_service_id id, void* obj)
{
    lh_check_not_null(s);
    if (id <= LH_SVC_NONE || id >= LH_SVC_MAX)
    {
        LH_PANIC("invalid service id");
    }
    if (s->slots[id])
    {
        LH_PANIC("service already registered");
    }
    s->slots[id] = obj;
    return obj;
}

void* lh_services_get(const lh_services* s, lh_service_id id)
{
    lh_check_not_null(s);
    if (id <= LH_SVC_NONE || id >= LH_SVC_MAX || !s->slots[id])
    {
        LH_PANIC("service not registered");
    }
    return s->slots[id];
}

void* lh_services_get_opt(const lh_services* s, lh_service_id id)
{
    if (!s || id <= LH_SVC_NONE || id >= LH_SVC_MAX) return NULL;
    return s->slots[id];
}

bool lh_services_has(const lh_services* s, lh_service_id id)
{
    return lh_services_get_opt(s, id) != NULL;
}
