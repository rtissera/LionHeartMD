/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * LionEngine Loader + Sequence run loop.
 */
#include "lh_engine.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

void lh_sequence_init(lh_sequence* s,
                      const lh_sequence_vt* vt,
                      const lh_resolution* res)
{
    s->vt           = vt;
    s->resolution   = *res;
    s->rate         = res->rate > 0 ? res->rate : 60;
    s->end_request  = false;
    s->next_factory = NULL;
    s->next_args    = NULL;
    s->user         = NULL;
}

void lh_sequence_end(lh_sequence* s)
{
    s->end_request  = true;
    s->next_factory = NULL;
    s->next_args    = NULL;
}

void lh_sequence_end_with(lh_sequence* s,
                          lh_sequence* (*factory)(void* args),
                          void* args)
{
    s->end_request  = true;
    s->next_factory = factory;
    s->next_args    = args;
}

int lh_loader_init(lh_loader* l, const lh_config* cfg)
{
    l->config  = *cfg;
    l->current = NULL;
    l->running = false;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    Uint32 flags = SDL_WINDOW_SHOWN;
    if (!cfg->windowed)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    l->window = SDL_CreateWindow(cfg->title ? cfg->title : "Lionheart Remake",
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 cfg->output.width,
                                 cfg->output.height,
                                 flags);
    if (!l->window)
    {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    l->renderer = SDL_CreateRenderer(l->window, -1,
                                     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!l->renderer)
    {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(l->window);
        SDL_Quit();
        return -1;
    }

    /* Render at source resolution, scale to output. */
    SDL_RenderSetLogicalSize(l->renderer, cfg->source.width, cfg->source.height);
    SDL_SetRenderDrawColor(l->renderer, 0, 0, 0, 255);

    return 0;
}

void lh_loader_shutdown(lh_loader* l)
{
    if (l->renderer) SDL_DestroyRenderer(l->renderer);
    if (l->window)   SDL_DestroyWindow(l->window);
    SDL_Quit();
    l->renderer = NULL;
    l->window   = NULL;
}

static void destroy_sequence(lh_sequence* s)
{
    if (s && s->vt && s->vt->destroy)
        s->vt->destroy(s);
}

int lh_loader_run(lh_loader* l, lh_sequence* initial)
{
    l->current = initial;
    l->running = true;

    while (l->running && l->current)
    {
        lh_sequence* seq = l->current;

        if (seq->vt && seq->vt->load)
            seq->vt->load(seq);

        const Uint64 freq      = SDL_GetPerformanceFrequency();
        const double tick_secs = 1.0 / (double)(seq->rate > 0 ? seq->rate : 60);
        const Uint64 tick_ticks = (Uint64)(tick_secs * (double)freq);
        Uint64 last = SDL_GetPerformanceCounter();
        Uint64 acc  = 0;

        while (!seq->end_request)
        {
            SDL_Event ev;
            while (SDL_PollEvent(&ev))
            {
                if (ev.type == SDL_QUIT
                 || (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE))
                {
                    seq->end_request = true;
                    seq->next_factory = NULL;
                    l->running = false;
                }
            }

            const Uint64 now   = SDL_GetPerformanceCounter();
            const Uint64 delta = now - last;
            last = now;
            acc += delta;

            int updates = 0;
            while (acc >= tick_ticks && updates < 5)
            {
                if (seq->vt && seq->vt->update)
                    seq->vt->update(seq, 1.0); /* extrp=1.0 fixed step */
                acc -= tick_ticks;
                updates++;
            }

            SDL_SetRenderDrawColor(l->renderer, 0, 0, 0, 255);
            SDL_RenderClear(l->renderer);
            if (seq->vt && seq->vt->render)
                seq->vt->render(seq, l->renderer);
            SDL_RenderPresent(l->renderer);
        }

        if (seq->vt && seq->vt->on_terminated)
            seq->vt->on_terminated(seq, seq->next_factory != NULL);

        lh_sequence* next = NULL;
        if (l->running && seq->next_factory)
            next = seq->next_factory(seq->next_args);

        destroy_sequence(seq);
        l->current = next;
    }

    return 0;
}
