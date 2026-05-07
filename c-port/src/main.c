/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of Main.java + AppLionheart.java entry chain (skeleton).
 *
 * Java equivalents:
 *   Main.main()           -> main()
 *   AppLionheart.main()   -> app_lionheart_main()
 *   AppLionheart.run()    -> app_lionheart_run()
 *   AppLionheart.configure()/getDesktopResolution() -> build_config()
 *
 * Phase 1 scope: parse args (subset), create config, run Loading sequence.
 * Settings/Tools/Launcher/Gamepad come in later phases.
 */
#include "lh_constant.h"
#include "lh_engine.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern lh_sequence* loading_sequence_new(const lh_resolution* res);

static lh_resolution get_desktop_resolution(void)
{
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) == 0)
        return (lh_resolution){ dm.w, dm.h, dm.refresh_rate > 0 ? dm.refresh_rate : LH_RES_RATE };
    return (lh_resolution){ LH_RES_OUT_W, LH_RES_OUT_H, LH_RES_RATE };
}

static lh_config build_config(bool windowed)
{
    /* TODO phase 3: read Settings.getInstance().getResolution(desktop). */
    const lh_resolution desktop = get_desktop_resolution();
    (void)desktop;

    lh_config cfg = {
        .source   = { LH_RES_W,     LH_RES_H,     LH_RES_RATE },
        .output   = { LH_RES_OUT_W, LH_RES_OUT_H, LH_RES_RATE },
        .windowed = windowed,
        .depth    = 32,
        .icon_path= NULL,
        .title    = LH_PROGRAM_NAME,
    };
    return cfg;
}

static int app_lionheart_run(const lh_config* cfg)
{
    lh_loader loader = {0};
    if (lh_loader_init(&loader, cfg) != 0)
        return 1;

    /* TODO phase 2: AudioFactory.addFormat(WavFormat,Sc68Format) */
    /* TODO phase 3: Util.init(Tools.generateWorldRaster) */

    lh_sequence* first = loading_sequence_new(&cfg->source);
    int rc = lh_loader_run(&loader, first);

    lh_loader_shutdown(&loader);
    return rc;
}

static int app_lionheart_main(int argc, char** argv)
{
    /* TODO phase 3: Tools.prepareSettingsCustom / prepareInputCustom */
    /* TODO phase 3: Settings.load(file) and GameConfig parsing */
    (void)argc; (void)argv;

    lh_config cfg = build_config(true /* windowed default */);
    return app_lionheart_run(&cfg);
}

int main(int argc, char** argv)
{
    /* Tools.initLog / disableAutoScale -> SDL log init */
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

    lh_constant_init();

    /* Mirrors Main.java: with no args -> AppLionheart;
       "launcher" arg -> Launcher (phase 3+). */
    if (argc > 1 && strcmp(argv[1], "launcher") == 0)
    {
        fprintf(stderr, "launcher mode not yet ported (phase 3+)\n");
        return 1;
    }
    return app_lionheart_main(argc, argv);
}
