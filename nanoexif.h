#ifndef NANOEXIF_H__
#define NANOEXIF_H__

/*
The MIT License

Copyright (c) 2010 Tokuhiro Matsuno

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    NANOEXIF_LITTLE_ENDIAN,
    NANOEXIF_BIG_ENDIAN,
} nanoexif_endian;

typedef struct {
    uint16_t tag;
    uint16_t type;
    uint32_t count;
    uint8_t  offset[4];
} nanoexif_ifd_entry;

typedef struct {
    FILE *fp;
    nanoexif_endian endian;
    uint32_t offset;
} nanoexif;

#define NANOEXIF_TAG_COMPRESSION        0x0103
#define NANOEXIF_TAG_MAKE               0x010f
#define NANOEXIF_TAG_ORIENTATION        0x0112
#define NANOEXIF_TAG_JPEG_IF_OFFSET     0x0201
#define NANOEXIF_TAG_JPEG_IF_BYTE_COUNT 0x0202
#define NANOEXIF_TAG_EXIF_OFFSET        0x8769
#define NANOEXIF_TAG_GPS_INFO           0x8825

#define NANOEXIF_TYPE_BYTE      0x0001
#define NANOEXIF_TYPE_ASCII     0x0002
#define NANOEXIF_TYPE_SHORT     0x0003
#define NANOEXIF_TYPE_LONG      0x0004
#define NANOEXIF_TYPE_RATIONAL  0x0005
#define NANOEXIF_TYPE_SBYTE     0x0006
#define NANOEXIF_TYPE_UNDEFINED 0x0007
#define NANOEXIF_TYPE_SSHORT    0x0008
#define NANOEXIF_TYPE_SLONG     0x0009
#define NANOEXIF_TYPE_SRATIONAL 0x000a
#define NANOEXIF_TYPE_FLOAT     0x000b
#define NANOEXIF_TYPE_DFLOAT    0x000c

#define NANOEXIF_EXIF_HEADER_SIZE (2+2+2+6+2+2+4)

nanoexif * nanoexif_init(FILE *fp);
void nanoexif_free(nanoexif * ne);
uint16_t nanoexif_read_ifd_cnt(nanoexif * ne);
bool nanoexif_read_ifd_entry(nanoexif *ne, nanoexif_ifd_entry *entry);
uint16_t *nanoexif_get_ifd_entry_data_short(nanoexif *ne, nanoexif_ifd_entry *entry);
char * nanoexif_get_ifd_entry_data_ascii(nanoexif *ne, nanoexif_ifd_entry *entry);
uint32_t * nanoexif_get_ifd_entry_data_rational(nanoexif *ne, nanoexif_ifd_entry *entry);
uint32_t * nanoexif_get_ifd_entry_data_long(nanoexif *ne, nanoexif_ifd_entry *entry);
int nanoexif_skip_ifd_body(nanoexif *ne);
const char *nanoexif_tag_name(uint32_t n);

#endif  // NANOEXIF_H__

