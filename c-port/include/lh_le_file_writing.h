/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.io.FileWriting.
 * Big-endian binary writer paired with lh_file_reading.
 */
#ifndef LH_LE_FILE_WRITING_H
#define LH_LE_FILE_WRITING_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE* fp;
    bool  ok;
} lh_file_writing;

bool lh_file_writing_open  (lh_file_writing* fw, const char* path);
void lh_file_writing_close (lh_file_writing* fw);

void lh_file_writing_bool  (lh_file_writing* fw, bool v);
void lh_file_writing_byte  (lh_file_writing* fw, int8_t v);
void lh_file_writing_short (lh_file_writing* fw, int16_t v);
void lh_file_writing_int   (lh_file_writing* fw, int32_t v);
void lh_file_writing_long  (lh_file_writing* fw, int64_t v);
void lh_file_writing_float (lh_file_writing* fw, float v);
void lh_file_writing_double(lh_file_writing* fw, double v);
void lh_file_writing_string(lh_file_writing* fw, const char* s);

bool lh_file_writing_is_ok (const lh_file_writing* fw);

#endif
