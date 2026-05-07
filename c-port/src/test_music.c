#define _POSIX_C_SOURCE 200809L
/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Standalone test for sc68 chiptune playback.
 *
 * Drives the lh_audio + lh_music pipeline against a real .sc68 file
 * with SDL_AUDIODRIVER=dummy so it works headless. Reports whether
 * libsc68 was loaded, whether sc68_create + sc68_load_uri + sc68_process
 * succeeded, and whether non-zero PCM was actually produced.
 *
 * Excluded from the main lionheart binary (Makefile filter on test_*.c).
 * Build target: make test
 * Run:         ./build/test_music [media_path]
 */
#include "lh_audio.h"
#include "lh_medias.h"
#include "lh_music.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_TRACK "com/b3dgs/lionheart/music/intro.sc68"

int main(int argc, char** argv)
{
    setenv("SDL_AUDIODRIVER", "dummy", 1);

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    const char* root = getenv("LH_ASSETS_ROOT");
    lh_medias_set_root(root ? root :
        "/home/romain/LionheartMD/lionheart-stripped/assets/src/main/resources");

    if (!lh_audio_init(44100, 2)) { fprintf(stderr, "audio init failed\n"); return 2; }
    if (!lh_music_init())          { fprintf(stderr, "music init failed\n"); return 3; }

    printf("sc68 available: %s\n", lh_music_is_available() ? "yes" : "no (silent stub)");

    const char* media = argc > 1 ? argv[1] : DEFAULT_TRACK;
    if (!lh_music_play(media))
    {
        fprintf(stderr, "play failed: %s\n", media);
        lh_music_shutdown();
        lh_audio_shutdown();
        SDL_Quit();
        return 4;
    }

    printf("playing: %s\n", media);
    lh_music_set_volume(80);

    /* Drive the audio mixer for ~1 second by sleeping; the SDL audio
     * device is the dummy driver but it still pulls our callback at the
     * configured sample rate when the audio thread is unpaused. */
    SDL_Delay(1000);

    lh_music_stop();
    lh_music_shutdown();
    lh_audio_shutdown();
    SDL_Quit();

    printf("OK\n");
    return 0;
}
