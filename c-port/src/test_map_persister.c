/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com)
 *                          Pierre-Alexandre (contact@b3dgs.com)
 * C/SDL2 port: standalone CLI smoke test for the .lvl persister. Built
 * via `make test`; NOT linked into the main lionheart binary.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lh_le_map_tile.h"
#include "lh_map_tile_persister.h"

int main(int argc, char** argv)
{
    const char* path = (argc > 1)
        ? argv[1]
        : "/home/romain/LionheartMD/lionheart-stripped/assets/"
          "src/main/resources/com/b3dgs/lionheart/level/spidercave1/"
          "beginner/stage.lvl";

    printf("test_map_persister: %s\n", path);
    const bool ok = lh_map_tile_persister_self_test(path);
    return ok ? 0 : 1;
}
