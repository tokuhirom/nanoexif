#ifndef NANOEXIF_EASY_H__
#define NANOEXIF_EASY_H__
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#include <stdint.h>
#include <stdio.h>

char * nanoexif_easy_thumbnail(FILE * fp, uint16_t *orientation, uint32_t *jpeg_byte_count);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* NANOEXIF_EASY_H__ */
