/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.io.FileReading.
 *
 * Big-endian binary reader. Mirrors Java DataInputStream interface used by
 * MapTilePersister + Snapshotable for level + save state I/O.
 */
#ifndef LH_LE_FILE_READING_H
#define LH_LE_FILE_READING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE* fp;
    bool  ok;
} lh_file_reading;

bool    lh_file_reading_open  (lh_file_reading* fr, const char* path);
void    lh_file_reading_close (lh_file_reading* fr);

bool    lh_file_reading_bool  (lh_file_reading* fr);
int8_t  lh_file_reading_byte  (lh_file_reading* fr);
int16_t lh_file_reading_short (lh_file_reading* fr);
int32_t lh_file_reading_int   (lh_file_reading* fr);
int64_t lh_file_reading_long  (lh_file_reading* fr);
float   lh_file_reading_float (lh_file_reading* fr);
double  lh_file_reading_double(lh_file_reading* fr);
size_t  lh_file_reading_string(lh_file_reading* fr, char* out, size_t out_len);

bool    lh_file_reading_is_ok (const lh_file_reading* fr);

#endif
