/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.GameType.
 */
#ifndef LH_GAME_TYPE_H
#define LH_GAME_TYPE_H

typedef enum {
    LH_GAME_STORY = 0,
    LH_GAME_TRAINING,
    LH_GAME_SPEEDRUN,
    LH_GAME_BATTLE,
    LH_GAME_VERSUS,
    LH_GAME_COUNT
} lh_game_type;

static inline lh_game_type lh_game_type_from(int idx)
{
    if (idx < 0) return LH_GAME_STORY;
    if (idx >= LH_GAME_COUNT) return LH_GAME_VERSUS;
    return (lh_game_type)idx;
}

#endif
