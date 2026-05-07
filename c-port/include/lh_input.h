/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Minimal keyboard input adapter. Polled from SDL key state once per
 * frame; consumers query lh_input_axis_h / axis_v / fire / jump.
 *
 * Replaces the AWT/lwjgl input chain stripped during pass-1.
 */
#ifndef LH_INPUT_H
#define LH_INPUT_H

#include <stdbool.h>

void   lh_input_update(void);                 /* call once per frame */
double lh_input_axis_h(void);                 /* -1..1 (LEFT / RIGHT) */
double lh_input_axis_v(void);                 /* -1..1 (UP / DOWN)    */
bool   lh_input_fire (void);                  /* X / SHIFT key  */
bool   lh_input_jump (void);                  /* SPACE or UP    */
bool   lh_input_quit (void);                  /* ESC pressed    */

#endif
