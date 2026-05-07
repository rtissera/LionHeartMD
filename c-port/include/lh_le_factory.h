/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.Factory.
 *
 * Entity dispatch. The Java original uses Class.forName on the
 * <lionengine:class> XML node; the C port replaces this with a closed-world
 * registry of factory functions, populated at phase 4 by a codegen script
 * that scans entity XML files.
 */
#ifndef LH_LE_FACTORY_H
#define LH_LE_FACTORY_H

#include "lh_le_featurable.h"
#include "lh_le_setup.h"

typedef lh_featurable* (*lh_factory_fn)(const lh_setup* setup);

#define LH_FACTORY_MAX 256

typedef struct {
    const char*   key;          /* media path or entity id string */
    lh_factory_fn fn;
} lh_factory_entry;

typedef struct {
    lh_factory_entry entries[LH_FACTORY_MAX];
    int              count;
} lh_factory;

void           lh_factory_init    (lh_factory* f);
void           lh_factory_register(lh_factory* f, const char* key, lh_factory_fn fn);
lh_featurable* lh_factory_create  (const lh_factory* f, const char* key, const lh_setup* setup);

#endif
