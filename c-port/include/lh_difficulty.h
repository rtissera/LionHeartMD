/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.Difficulty.
 */
#ifndef LH_DIFFICULTY_H
#define LH_DIFFICULTY_H

typedef enum {
    LH_DIFFICULTY_BEGINNER = 0,
    LH_DIFFICULTY_NORMAL,
    LH_DIFFICULTY_HARD,
    LH_DIFFICULTY_LIONHARD,
    LH_DIFFICULTY_COUNT
} lh_difficulty;

static inline lh_difficulty lh_difficulty_from(int idx)
{
    if (idx < 0) return LH_DIFFICULTY_BEGINNER;
    if (idx >= LH_DIFFICULTY_COUNT) return LH_DIFFICULTY_LIONHARD;
    return (lh_difficulty)idx;
}

#endif
