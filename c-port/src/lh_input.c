/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_input.h"

#include <SDL2/SDL.h>

static double s_axis_h = 0.0;
static double s_axis_v = 0.0;
static bool   s_fire   = false;
static bool   s_jump   = false;
static bool   s_quit   = false;

void lh_input_update(void)
{
    /* Note: SDL_PollEvent is also drained by sequences; SDL_GetKeyboardState
     * works after any pump, so we just sample the state here. */
    SDL_PumpEvents();
    const Uint8* k = SDL_GetKeyboardState(NULL);

    s_axis_h = (k[SDL_SCANCODE_RIGHT] ? 1.0 : 0.0)
             - (k[SDL_SCANCODE_LEFT]  ? 1.0 : 0.0);
    s_axis_v = (k[SDL_SCANCODE_DOWN]  ? 1.0 : 0.0)
             - (k[SDL_SCANCODE_UP]    ? 1.0 : 0.0);

    s_fire = k[SDL_SCANCODE_X]      || k[SDL_SCANCODE_LSHIFT]
          || k[SDL_SCANCODE_RSHIFT] || k[SDL_SCANCODE_LCTRL];
    s_jump = k[SDL_SCANCODE_SPACE]  || k[SDL_SCANCODE_UP];
    s_quit = k[SDL_SCANCODE_ESCAPE];
}

double lh_input_axis_h(void) { return s_axis_h; }
double lh_input_axis_v(void) { return s_axis_v; }
bool   lh_input_fire (void)  { return s_fire;   }
bool   lh_input_jump (void)  { return s_jump;   }
bool   lh_input_quit (void)  { return s_quit;   }
