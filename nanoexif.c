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

/*

LIMITATION
----------

This module supports C99. I don't want to support legacy systems, but I can accept your patch.

*/


/**

exif structure

The starting marker order of is establised by EXIF spec.

    SOI => APP1

see.

http://hp.vector.co.jp/authors/VA032610/JPEGFormat/AboutExif.htm
http://www.ryouto.jp/f6exif/exif.html

*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "nanoexif.h"

/* private constants */
#define NANOEXIF_SOI_APP1    "\xFF\xD8\xFF\xE1"
#define NANOEXIF_EXIF_HEADER "\x45\x78\x69\x66\x00\x00"

#ifdef DEBUG
#define D(...) printf(__VA_ARGS__);
#else
#define D(...)
#endif

static inline uint32_t read_32(nanoexif_endian endian, const uint8_t *buf) {
    if (endian == NANOEXIF_LITTLE_ENDIAN) {
        D("LITTLE_ENDIAN\n");
        return (buf[3]<<24) | (buf[2]<<16) | (buf[1]<<8) | buf[0];
    } else {
        return (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3];
    }
}

static inline uint16_t read_16(nanoexif_endian endian, const uint8_t *buf) {
    if (endian == NANOEXIF_LITTLE_ENDIAN) {
        return (buf[1]<<8) | buf[0];
    } else {
        return (buf[0]<<8) | buf[1];
    }
}

static inline uint16_t swap_endian_16(uint16_t i) {
    return ((i&0xff)<<8) | ((i&0xff00)>>8);
}
static inline uint32_t swap_endian_32(uint32_t i) {
    return ((i&0x000000ff)<<24) | ((i&0x0000ff00)<<8) | ((i&0x00ff0000)>>8) | ((i&0xff000000)>>24);
}

nanoexif * nanoexif_init(FILE *fp) {
    nanoexif_endian endian;

    uint8_t buf[NANOEXIF_EXIF_HEADER_SIZE];
    if (fread(buf, sizeof(char), NANOEXIF_EXIF_HEADER_SIZE, fp) != NANOEXIF_EXIF_HEADER_SIZE) {
        D("CANNOT OPEN\n");
        return NULL;
    }
    if (memcmp(buf, NANOEXIF_SOI_APP1, sizeof(NANOEXIF_SOI_APP1)-1)!=0) {
        D("SOI\n");
        return NULL;
    }
    if (memcmp(buf+6, NANOEXIF_EXIF_HEADER, sizeof(NANOEXIF_EXIF_HEADER)-1)!=0) {
        D("EXIFHEADER\n");
        return NULL;
    }
    D("ENDIAN: %X\n", buf[12]);
    if (memcmp(buf+12, "\x4d\x4d", 2) == 0) {
        D("BIG ENDIAN\n");
        endian = NANOEXIF_BIG_ENDIAN;
    } else { // 4949
        D("LITTLE ENDIAN\n");
        endian = NANOEXIF_LITTLE_ENDIAN;
    }
    if (memcmp(buf+14, "\x00\x2a", 2) != 0) {
        D("tiff header\n");
        return NULL; // tiff
    }
    uint32_t len = read_32(endian, buf+16);
    uint8_t *p = buf+16;
    D("%d %d %d %d\n", p[0], p[1], p[2], p[3]);
    if (len != 8) { // normally 8.
        D("seek : %d\n", len);
        if (fseek(fp, len-8, SEEK_CUR) != 0) {
            D("CANNOT SEEK\n");
            return NULL; // error
        }
    }

    long offset = ftell(fp);
    if (offset == -1) { return NULL; }

    nanoexif * ne = malloc(sizeof(nanoexif));
    if (!ne) {
        return NULL;
    }
    ne->endian         = endian;
    ne->fp             = fp;
    ne->offset         = offset - 8;
    return ne;
}

void nanoexif_free(nanoexif * ne) {
    free(ne);
}

int32_t nanoexif_read_ifd_cnt(nanoexif * ne) {
    uint8_t ifdcntbuf[2];
    int read = fread(ifdcntbuf, sizeof(char), sizeof(ifdcntbuf), ne->fp);
    if (read != 2) {
        D("cannot read 1: %d\n", read);
        return -1;
    }
    return read_16(ne->endian, ifdcntbuf);
}

bool nanoexif_read_ifd_entry(nanoexif *ne, nanoexif_ifd_entry *entry) {
    if (fread(entry, sizeof(nanoexif_ifd_entry), 1, ne->fp) != 1) {
        D("cannot read\n");
        return false;
    }

    if (NANOEXIF_MACHINE_ENDIAN != ne->endian) {
        entry->tag    = swap_endian_16(entry->tag);
        entry->type   = swap_endian_16(entry->type);
        entry->count  = swap_endian_32(entry->count);
    }
    return true;
}

#define NE_SEEK(ne, offset_) (fseek(ne->fp, ne->offset+(offset_), SEEK_SET)==0)

uint16_t *nanoexif_get_ifd_entry_data_short(nanoexif *ne, nanoexif_ifd_entry *entry) {
    if (entry->count <= 4/sizeof(uint16_t)) {
        uint16_t *ret = malloc(sizeof(uint16_t)*entry->count);
        if (!ret) { return NULL; }
        *ret =read_16(ne->endian, entry->offset);
        if (entry->count == 2) {
            *(ret+1) =read_16(ne->endian, entry->offset+2);
        }
        return ret;
    } else {
        uint32_t offset = read_32(ne->endian, entry->offset);
        long orig = ftell(ne->fp);
        if (orig == -1) {
            return NULL;
        }
        if (!NE_SEEK(ne, offset)) {
            return NULL;
        }

        uint16_t * buf = (uint16_t*)malloc(entry->count*sizeof(uint16_t));
        if (!buf) { return NULL; }
        if (fread(buf, sizeof(uint16_t), entry->count, ne->fp) != entry->count) {
            free(buf);
            return NULL;
        }
        if (NANOEXIF_MACHINE_ENDIAN != ne->endian) {
            uint16_t i;
            uint16_t* p = buf;
            for (i=0; i<entry->count; i++) {
                *p = swap_endian_16(*p);
                p++;
            }
        }
        if (fseek(ne->fp, orig, SEEK_SET) != 0) { // restore original pos
            return NULL;
        }
        return buf;
    }
}

uint32_t *nanoexif_get_ifd_entry_data_long(nanoexif *ne, nanoexif_ifd_entry *entry) {
    if (entry->count <= 4/sizeof(uint32_t)) {
        uint32_t *ret = malloc(sizeof(uint32_t));
        if (!ret) { return NULL; }
        *ret = read_32(ne->endian, entry->offset);
        return ret;
    } else {
        uint32_t offset = read_32(ne->endian, entry->offset);
        long orig = ftell(ne->fp);
        if (orig == -1) {
            return NULL;
        }
        if (!NE_SEEK(ne, offset)) {
            return NULL;
        }

        uint32_t * buf = (uint32_t*)malloc(entry->count*sizeof(uint32_t));
        if (!buf) { return NULL; }
        if (fread(buf, sizeof(uint32_t), entry->count, ne->fp) != entry->count) {
            free(buf);
            return NULL;
        }
        if (NANOEXIF_MACHINE_ENDIAN != ne->endian) {
            uint16_t i;
            uint32_t* p = (uint32_t*)buf;
            for (i=0; i<entry->count; i++) {
                *p = swap_endian_32(*p);
                p++;
            }
        }
        if (fseek(ne->fp, orig, SEEK_SET) != 0) { // restore original pos
            return NULL;
        }
        return buf;
    }
}

/**
 * @return allocated string. you should free(2) the buffer.
 */
char * nanoexif_get_ifd_entry_data_ascii(nanoexif *ne, nanoexif_ifd_entry *entry) {
    if (entry->count <= 4) {
        char *ret = malloc(sizeof(char)*entry->count);
        if (!ret) { return NULL; }
        memcpy(ret, entry->offset, entry->count);
        return ret;
    } else {
        uint32_t offset = read_32(ne->endian, entry->offset);
        long orig = ftell(ne->fp);
        if (orig == -1) {
            return NULL;
        }
        if (!NE_SEEK(ne, offset)) {
            return NULL;
        }

        char * buf = (char*)malloc(entry->count);
        if (!buf) { return NULL; }
        if (fread(buf, 1, entry->count, ne->fp) != entry->count) {
            free(buf);
            return NULL;
        }
        if (fseek(ne->fp, orig, SEEK_SET) != 0) { // restore original pos
            return NULL;
        }
        return buf;
    }
}

uint32_t * nanoexif_get_ifd_entry_data_rational(nanoexif *ne, nanoexif_ifd_entry *entry) {
    /* rational's minimal size is 8 bytes.cannot put on the offset. */
    uint32_t offset = read_32(ne->endian, entry->offset);
    long orig = ftell(ne->fp);
    if (orig == -1) {
        return NULL;
    }
    if (!NE_SEEK(ne, offset)) {
        return NULL;
    }

    char * buf = (char*)malloc(entry->count*sizeof(uint32_t)*2);
    if (!buf) { return NULL; }
    if (fread(buf, sizeof(uint32_t), entry->count*2, ne->fp) != entry->count*2) {
        free(buf);
        return NULL;
    }
    if (NANOEXIF_MACHINE_ENDIAN != ne->endian) {
        uint16_t i;
        uint32_t* p = (uint32_t*)buf;
        for (i=0; i<entry->count*2; i++) {
            *p = swap_endian_32(*p);
            p++;
        }
    }
    if (fseek(ne->fp, orig, SEEK_SET) != 0) { // restore original pos
        free(buf);
        return NULL;
    }
    return (uint32_t*)buf;
}

/**
 * @return -1: cannot read skip bytes
 *         -2: cannot seek
 *          0: nothing next ifd
 *          1: skipped
 */
int nanoexif_skip_ifd_body(nanoexif *ne) {
    uint8_t lenbuf[4];
    if (fread(lenbuf, sizeof(char), sizeof(lenbuf), ne->fp) != sizeof(lenbuf)) {
        D("cannot read 3\n");
        return -1;
    }
    uint32_t skip = read_32(ne->endian, lenbuf);
    if (skip != 0) {
        if (fseek(ne->fp, ne->offset+skip, SEEK_SET) != 0) {
            perror("WHY");
            D("cannot seek 4\n");
            return -2;
        }
        return skip;
    } else {
        return 0; // nothing next ifd
    }
}

