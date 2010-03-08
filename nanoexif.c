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

/* private constants */
#define NANOEXIF_EXIF_HEADER_SIZE (2+2+2+6+2+2+4)
#define NANOEXIF_SOI_APP1    "\xFF\xD8\xFF\xE1"
#define NANOEXIF_EXIF_HEADER "\x45\x78\x69\x66\x00\x00"

#define NANOEXIF_TAG_COMPRESSION        0x103
#define NANOEXIF_TAG_MAKE               0x10f
#define NANOEXIF_TAG_ORIENTATION        0x112
#define NANOEXIF_TAG_JPEG_IF_OFFSET     0x201
#define NANOEXIF_TAG_JPEG_IF_BYTE_COUNT 0x202

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
} nanoexif;

#ifdef DEBUG
#define D(...) printf(__VA_ARGS__);
#else
#define D(...)
#endif

static inline uint32_t _read_32(nanoexif_endian endian, const uint8_t *buf) {
    if (endian == NANOEXIF_LITTLE_ENDIAN) {
        D("LITTLE_ENDIAN\n");
        return (buf[3]<<24) | (buf[2]<<16) | (buf[1]<<8) | buf[0];
    } else {
        return (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3];
    }
}

static inline uint16_t _read_16(nanoexif_endian endian, const uint8_t *buf) {
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
    uint32_t len = _read_32(endian, buf+16);
    uint8_t *p = buf+16;
    D("%d %d %d %d\n", p[0], p[1], p[2], p[3]);
    if (len != 8) { // normally 8.
        D("seek : %d\n", len);
        if (fseek(fp, len-8, SEEK_CUR) != 0) {
            D("CANNOT SEEK\n");
            return NULL; // error
        }
    }

    nanoexif * ne = malloc(sizeof(nanoexif));
    if (!ne) {
        return NULL;
    }
    ne->endian         = endian;
    ne->fp             = fp;
    return ne;
}

void nanoexif_free(nanoexif * ne) {
    free(ne);
}

uint16_t nanoexif_read_ifd_cnt(nanoexif * ne) {
    uint8_t ifdcntbuf[2];
    int read = fread(ifdcntbuf, sizeof(char), sizeof(ifdcntbuf), ne->fp);
    if (read != 2) {
        D("cannot read 1: %d\n", read);
        return false;
    }
    return _read_16(ne->endian, ifdcntbuf);
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

uint16_t nanoexif_get_ifd_entry_data_short(nanoexif *ne, nanoexif_ifd_entry *entry) {
    return _read_16(ne->endian, entry->offset);
}
/**
 * @return allocated string. you should free(2) the buffer.
 */
char * nanoexif_get_ifd_entry_data_ascii(nanoexif *ne, nanoexif_ifd_entry *entry) {
    if (entry->count <= 4) {
        return (char*)(entry->offset);
    } else {
        uint32_t offset = _read_32(ne->endian, entry->offset);
        long orig = ftell(ne->fp);
        if (orig == -1) {
            return NULL;
        }
        if (fseek(ne->fp, NANOEXIF_EXIF_HEADER_SIZE+offset-8, SEEK_SET) != 0) {
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

/**
 * @return -1: cannot read skip bytes
 *         -2: cannot seek
 *          0: nothing next ifd
 *         >0: skipped bytes
 */
uint32_t nanoexif_skip_ifd_body(nanoexif *ne) {
    uint8_t lenbuf[4];
    if (fread(lenbuf, sizeof(char), sizeof(lenbuf), ne->fp) != sizeof(lenbuf)) {
        D("cannot read 3\n");
        return -1;
    }
    uint32_t skip = _read_32(ne->endian, lenbuf);
    if (skip != 0) {
        D("SEEK TO skip; %d\n", skip);
        if (fseek(ne->fp, NANOEXIF_EXIF_HEADER_SIZE+skip-8, SEEK_SET) != 0) {
            D("cannot seek 4\n");
            return -2;
        }
        return skip;
    } else {
        return 0;
    }
}

#ifdef TEST

#include "nanotap.h"
#include <assert.h>

int main(int argc, char **argv) {
    const char * src = "t/data/sample-iphone.jpg";
    FILE *ifp;
    if (!(ifp = fopen(src, "rb"))) {
        perror(argv[0]);
        return 1;
    }
    uint32_t jpeg_byte_count = 0;
    uint8_t * thumb;
    nanoexif * ne = nanoexif_init(ifp);
    assert(ne);

    // read ifd0
    uint16_t orientation = 0;
    {
        uint16_t ifd_entry_cnt = nanoexif_read_ifd_cnt(ne);
        int i;
        for (i=0; i<ifd_entry_cnt; i++) {
            nanoexif_ifd_entry entry;
            assert(nanoexif_read_ifd_entry(ne, &entry));
            D("-- %d\n", i);
            switch (entry.tag) {
            case NANOEXIF_TAG_ORIENTATION:
                assert(entry.type == NANOEXIF_TYPE_SHORT);
                orientation = nanoexif_get_ifd_entry_data_short(ne, &entry);
                D("orientation: %d\n", orientation);
                break;
            case NANOEXIF_TAG_MAKE:
                assert(entry.type == NANOEXIF_TYPE_ASCII);
                {
                    char *make = nanoexif_get_ifd_entry_data_ascii(ne, &entry);
                    assert(make);
                    ok(strcmp("Apple", make) ==0, "Make");
                    free(make);
                }
                break;
            default:
                {
                    switch (entry.type) {
                    case NANOEXIF_TYPE_ASCII:
                        {
                            char *make = nanoexif_get_ifd_entry_data_ascii(ne, &entry);
                            D("ascii: %d, %s\n", entry.tag, make);
                            assert(make);
                            free(make);
                        }
                        break;
                    }
                }
                break;
            }
            // TODO: endian
            D("tag: %X\ntype: %X\ncount: %d\n",
                entry.tag,
                entry.type,
                entry.count
            );
        }
        assert( nanoexif_skip_ifd_body(ne) > 0 );
    }

    // ifd1
    {
        uint16_t ifd_entry_cnt = nanoexif_read_ifd_cnt(ne);
        int i;
        bool compression_ok      = false;
        uint32_t jpeg_offset     = 0;
        for (i=0; i<ifd_entry_cnt; i++) {
            nanoexif_ifd_entry entry;
            assert(nanoexif_read_ifd_entry(ne, &entry));
            D("-- %d\n", i);
            switch (entry.tag) {
            case NANOEXIF_TAG_COMPRESSION:
                {
                    assert(entry.type == NANOEXIF_TYPE_SHORT);
                    uint16_t o = nanoexif_get_ifd_entry_data_short(ne, &entry);
                    assert(o==6);
                    D("jpeg ok\n");
                    compression_ok = true;
                    break;
                }
            case NANOEXIF_TAG_JPEG_IF_OFFSET:
                {
                    uint32_t offset = _read_32(ne->endian, entry.offset);
                    D("offset: %d\n", offset);
                    jpeg_offset = offset;
                    break;
                }
            case NANOEXIF_TAG_JPEG_IF_BYTE_COUNT:
                {
                    uint32_t offset = _read_32(ne->endian, entry.offset);
                    D("byte_count: %d\n", offset);
                    jpeg_byte_count = offset;
                    break;
                }
            }
            // TODO: endian
            D("tag: %X\ntype: %X\ncount: %d\n",
                entry.tag,
                entry.type,
                entry.count
            );
        }
        assert(compression_ok && jpeg_offset && jpeg_byte_count);
        D("jpeg offset: %X\n", jpeg_offset);
        if (fseek(ne->fp, NANOEXIF_EXIF_HEADER_SIZE-8+jpeg_offset, SEEK_SET) != 0) {
            D("cannot seek 9\n");
            assert(0);
        }
        thumb = (uint8_t*)malloc(jpeg_byte_count);
        assert(thumb);
        if (fread(thumb, sizeof(uint8_t), jpeg_byte_count, ne->fp) != jpeg_byte_count) {
        // if (fread(buf, sizeof(uint8_t), jpeg_byte_count, ne->fp) != jpeg_byte_count) {
            D("FAIL!\n");
        }
    }

    ok(!!thumb, "thumbnail generated");
    if (!(thumb && jpeg_byte_count > 100)) {
        printf("cannot load thumbnail\n");
        return 0;
    }
    nanoexif_free(ne);
    ok(memcmp(thumb, "\xFF\xD8", 2)==0, "jpeg?");
    ok(memcmp(thumb+(jpeg_byte_count-2), "\xFF\xD9", 2)==0, "jpeg eoi");
    free(thumb);
    fclose(ifp);

    done_testing();
    return 0;
}
#endif

