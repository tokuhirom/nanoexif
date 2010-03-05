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

This module supports C99.

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

#define HEADER_SIZE (2+2+2+6+2+2+4)
#define SOI_APP1    "\xFF\xD8\xFF\xE1"
#define EXIF_HEADER "\x45\x78\x69\x66\x00\x00"

#define EXIF_THUMBNAIL_TAG_COMPRESSION        0x103
#define EXIF_THUMBNAIL_TAG_JPEG_IF_OFFSET     0x201
#define EXIF_THUMBNAIL_TAG_JPEG_IF_BYTE_COUNT 0x202

#define EXIF_THUMBNAIL_TYPE_SHORT 0x0003

typedef enum {
    EXIF_THUMBNAIL_LITTLE_ENDIAN,
    EXIF_THUMBNAIL_BIG_ENDIAN,
} exif_thumbnail_endian;

typedef struct {
    uint16_t tag;
    uint16_t type;
    uint32_t count;
    uint8_t  offset[4];
} exif_thumbnail_ifd_entry;

#ifdef DEBUG
#define D(...) printf(__VA_ARGS__);
#else
#define D(x)
#endif


static inline uint32_t _read_32(exif_thumbnail_endian endian, const uint8_t *buf) {
    if (endian == EXIF_THUMBNAIL_LITTLE_ENDIAN) {
        D("LITTLE_ENDIAN\n");
        return (buf[3]<<24) | (buf[2]<<16) | (buf[1]<<8) | buf[0];
    } else {
        return (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3];
    }
}

static inline uint16_t _read_16(exif_thumbnail_endian endian, const uint8_t *buf) {
    if (endian == EXIF_THUMBNAIL_LITTLE_ENDIAN) {
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

uint8_t * exif_thumbnail_read(FILE *fp, size_t *bytes) {
    uint8_t buf[HEADER_SIZE];
    exif_thumbnail_endian endian;
    exif_thumbnail_endian machine_endian;

    int x=1; // 0x00000001
    if (*(char*)&x) {
        /* little endian. memory image 01 00 00 00 */
        machine_endian = EXIF_THUMBNAIL_LITTLE_ENDIAN;
    }else{
        /* big endian. memory image 00 00 00 01 */
        machine_endian = EXIF_THUMBNAIL_BIG_ENDIAN;
    }


    if (fread(buf, sizeof(char), HEADER_SIZE, fp) != HEADER_SIZE) {
        D("CANNOT OPEN\n");
        return NULL;
    }
    if (memcmp(buf, SOI_APP1, sizeof(SOI_APP1)-1)!=0) {
        D("SOI\n");
        return NULL;
    }
    if (memcmp(buf+6, EXIF_HEADER, sizeof(EXIF_HEADER)-1)!=0) {
        D("EXIFHEADER\n");
        return NULL;
    }
    D("ENDIAN: %X\n", buf[12]);
    if (memcmp(buf+12, "\x4d\x4d", 2) == 0) {
        D("BIG ENDIAN\n");
        endian = EXIF_THUMBNAIL_BIG_ENDIAN;
    } else { // 4949
        D("LITTLE ENDIAN\n");
        endian = EXIF_THUMBNAIL_LITTLE_ENDIAN;
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
    {
        // skip ifd0
        {
            uint8_t ifdcntbuf[2];
            int read = fread(ifdcntbuf, sizeof(char), sizeof(ifdcntbuf), fp);
            if (read != 2) {
                D("cannot read 1: %d\n", read);
                return NULL;
            }
            uint16_t cnt = _read_16(endian, ifdcntbuf);
#define IFD_FIELD_SIZE 12
            if (fseek(fp, cnt*IFD_FIELD_SIZE, SEEK_CUR) != 0) {
                D("cannot seek 2\n");
                return NULL;
            }
        }
        {
            uint8_t lenbuf[4];
            if (fread(lenbuf, sizeof(char), sizeof(lenbuf), fp) != sizeof(lenbuf)) {
                D("cannot read 3\n");
                return NULL;
            }
            uint32_t skip = _read_32(endian, lenbuf);
            D("SEEK TO skip; %d\n", skip);
            if (fseek(fp, HEADER_SIZE+skip-8, SEEK_SET) != 0) {
                D("cannot seek 4\n");
                return NULL;
            }
        }
    }
    {
        // process ifd1
        uint8_t ifdcntbuf[2];
        if (fread(ifdcntbuf, sizeof(uint8_t), sizeof(ifdcntbuf), fp) != 2) {
            D("CANNOT read\n");
            return NULL;
        }
        uint16_t entry_num = _read_16(endian, ifdcntbuf);
        int i;
        D("cnt: %d\n", entry_num);
        exif_thumbnail_ifd_entry entry;
        bool compression_ok      = false;
        uint32_t jpeg_offset     = 0;
        uint32_t jpeg_byte_count = 0;
        for (i=0; i<entry_num; i++) {
            D("-- %d\n", i);
            if (fread(&entry, sizeof(exif_thumbnail_ifd_entry), 1, fp) != 2) {
                D("tag: %X\ntype: %X\ncount: %d\n",
                    entry.tag,
                    entry.type,
                    entry.count
                );
                if (machine_endian != endian) {
                    entry.tag    = swap_endian_16(entry.tag);
                    entry.type   = swap_endian_16(entry.type);
                    entry.count  = swap_endian_32(entry.count);
                }
                // TODO: endian
                D("tag: %X\ntype: %X\ncount: %d\n",
                    entry.tag,
                    entry.type,
                    entry.count
                );
                switch (entry.tag) {
                case EXIF_THUMBNAIL_TAG_COMPRESSION:
                    {
                        if (entry.type != EXIF_THUMBNAIL_TYPE_SHORT) {
                            D("unknown type: %d\n", entry.type);
                            return NULL;
                        }
                        uint16_t o = _read_16(endian, entry.offset);
                        if (o == 6) {
                            D("jpeg ok\n");
                            compression_ok = true;
                        } else {
                            D("unknown compression type: %d\n", o);
                            return NULL;
                        }
                        break;
                    }
                case EXIF_THUMBNAIL_TAG_JPEG_IF_OFFSET:
                    {
                        uint32_t offset = _read_32(endian, entry.offset);
                        D("offset: %d\n", offset);
                        jpeg_offset = offset;
                        break;
                    }
                case EXIF_THUMBNAIL_TAG_JPEG_IF_BYTE_COUNT:
                    {
                        uint32_t offset = _read_32(endian, entry.offset);
                        D("byte_count: %d\n", offset);
                        jpeg_byte_count = offset;
                        break;
                    }
                }
            }
        }
        D("%d, %X, %X\n", compression_ok ? 1 : 0, jpeg_offset, jpeg_byte_count);
        if (!compression_ok || !jpeg_offset || !jpeg_byte_count) {
            D("not supported!\n");
            return NULL;
        }
        D("jpeg offset: %X\n", jpeg_offset);
        if (fseek(fp, HEADER_SIZE-8+jpeg_offset, SEEK_SET) != 0) {
            D("cannot seek 9\n");
            return NULL;
        }
        uint8_t *buf = (uint8_t*)malloc(jpeg_byte_count);
        if (!buf) {
            return NULL;
        }
        if (fread(buf, sizeof(uint8_t), jpeg_byte_count, fp) != jpeg_byte_count) {
        // if (fread(buf, sizeof(uint8_t), jpeg_byte_count, fp) != jpeg_byte_count) {
            D("FAIL!\n");
        }
        D("%X, %X\n", buf[0], buf[1]);
        D("%X, %X\n", buf[jpeg_byte_count-2], buf[jpeg_byte_count-1]);
        *bytes = jpeg_byte_count;
        return buf;
    }
    D("DONE\n");
    return NULL;
}

#ifdef TEST
int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s src.jpg thumb.jpg\n", argv[0]);
        return 2;
    }
    FILE *ifp;
    if (!(ifp = fopen(argv[1], "rb"))) {
        perror(argv[0]);
        return 1;
    }
    size_t bytes = 0;
    uint8_t* thumb = exif_thumbnail_read(ifp, &bytes);
    if (!thumb) {
        printf("cannot load thumbnail\n");
        return 0;
    }
    FILE *ofp;
    if (!(ofp = fopen(argv[2], "wb"))) {
        perror(argv[0]);
        return 1;
    }
    if (fwrite(thumb, sizeof(uint8_t), bytes, ofp) != bytes) {
        perror(argv[0]);
        return 1;
    }
    free(thumb);
    fclose(ifp);
    fclose(ofp);
    return 0;
}
#endif

