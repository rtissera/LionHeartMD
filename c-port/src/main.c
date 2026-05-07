/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of Main.java + AppLionheart.java entry chain.
 *
 * Java equivalents:
 *   Main.main()           -> main()
 *   AppLionheart.main()   -> app_lionheart_main()
 *   AppLionheart.run()    -> app_lionheart_run()
 *   AppLionheart.configure()/getDesktopResolution() -> build_config()
 *
 * Phase 5 wires up: asset cache + audio + Medias root before the loader
 * starts the Loading -> Menu -> Scene chain.
 */
#include "lh_constant.h"
#include "lh_engine.h"
#include "lh_audio.h"
#include "lh_asset_cache.h"
#include "lh_medias.h"
#include "lh_settings.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern lh_sequence* loading_sequence_new(const lh_resolution* res);

#define LH_DEFAULT_ASSETS_ROOT \
    "/home/romain/LionheartMD/lionheart-stripped/assets/src/main/resources"

static lh_resolution get_desktop_resolution(void)
{
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) == 0)
        return (lh_resolution){ dm.w, dm.h, dm.refresh_rate > 0 ? dm.refresh_rate : LH_RES_RATE };
    return (lh_resolution){ LH_RES_OUT_W, LH_RES_OUT_H, LH_RES_RATE };
}

static lh_config build_config(bool windowed)
{
    const lh_resolution desktop = get_desktop_resolution();
    (void)desktop;

    const lh_settings* st = lh_settings_get();
    lh_config cfg = {
        .source   = { LH_RES_W,        LH_RES_H,        LH_RES_RATE },
        .output   = { st->res_width,   st->res_height,  st->res_rate ? st->res_rate : LH_RES_RATE },
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

    /* Asset cache requires the SDL renderer; init after loader. */
    lh_asset_cache_init(loader.renderer);

    /* Resolve media paths under the stripped assets tree. Override via
     * LH_ASSETS_ROOT env var so the binary can run from any working dir. */
    const char* root = getenv("LH_ASSETS_ROOT");
    lh_medias_set_root(root ? root : LH_DEFAULT_ASSETS_ROOT);

    /* Audio: bring up the SDL device + mixer. Volumes from settings. */
    const lh_settings* st = lh_settings_get();
    lh_audio_init(44100, 2);
    lh_audio_set_master_volume(st->volume_music);

    lh_sequence* first = loading_sequence_new(&cfg->source);
    int rc = lh_loader_run(&loader, first);

    lh_audio_shutdown();
    lh_asset_cache_destroy();
    lh_loader_shutdown(&loader);
    return rc;
}

static int app_lionheart_main(int argc, char** argv)
{
    /* Optional CLI: -settings <path>. */
    const char* settings_file = NULL;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-settings") == 0 && i + 1 < argc)
        {
            settings_file = argv[++i];
        }
    }
    if (settings_file)
    {
        lh_settings_load_file(lh_settings_get(), settings_file);
    }
    else
    {
        lh_settings_load_default(lh_settings_get());
    }

    lh_config cfg = build_config(lh_settings_get()->res_windowed);
    return app_lionheart_run(&cfg);
}

int main(int argc, char** argv)
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
    lh_constant_init();

    /* Mirrors Main.java: with no args -> AppLionheart;
       "launcher" mode stripped (Swing GUI dropped during pass-1 strip). */
    if (argc > 1 && strcmp(argv[1], "launcher") == 0)
    {
        fprintf(stderr, "launcher mode stripped (Swing GUI dropped, CLI args only)\n");
        return 1;
    }
    return app_lionheart_main(argc, argv);
}
