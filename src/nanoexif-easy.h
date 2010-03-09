#ifndef NANOEXIF_EASY_H__
#define NANOEXIF_EASY_H__

#include <stdint.h>
#include <stdio.h>

char * nanoexif_easy_thumbnail(FILE * fp, uint16_t *orientation, uint32_t *jpeg_byte_count);

#endif  // NANOEXIF_EASY_H__
