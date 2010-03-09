#include "nanotap.h"
#include <stdio.h>
#include <assert.h>
#include "../nanoexif.h"

#ifdef DEBUG
#define D(...) printf(__VA_ARGS__);
#else
#define D(...)
#endif

int main(int argc, char **argv) {
    const char * src = "t/data/sample-iphone.jpg";
    FILE *ifp;
    if (!(ifp = fopen(src, "rb"))) {
        perror(argv[0]);
        return 1;
    }
    uint32_t jpeg_byte_count = 0;
    uint8_t * thumb;
    uint32_t ifd0_offset;
    nanoexif * ne = nanoexif_init(ifp, &ifd0_offset);
    assert(ne);

    // read ifd0
    uint16_t orientation = 0;
    uint32_t ifd1_offset;
    {
        uint16_t cnt;
        nanoexif_ifd_entry* entries = nanoexif_read_ifd(ne, ifd0_offset, &ifd1_offset, &cnt);
        assert(entries);
        int i;
        for (i=0; i<cnt; i++) {
            D("-- %d\n", i);
            switch (entries[i].tag) {
            case NANOEXIF_TAG_ORIENTATION:
                assert(entries[i].type == NANOEXIF_TYPE_SHORT);
                uint16_t *x = nanoexif_get_ifd_entry_data_short(ne, &entries[i]);
                orientation = *x;
                free(x);
                break;
            case NANOEXIF_TAG_MAKE:
                assert(entries[i].type == NANOEXIF_TYPE_ASCII);
                {
                    char *make = nanoexif_get_ifd_entry_data_ascii(ne, &entries[i]);
                    assert(make);
                    ok(strcmp("Apple", make) ==0, "Make");
                    free(make);
                }
                break;
            default:
                {
                    switch (entries[i].type) {
                    case NANOEXIF_TYPE_ASCII:
                        {
                            char *make = nanoexif_get_ifd_entry_data_ascii(ne, &entries[i]);
                            D("ascii: %d, %s\n", entries[i].tag, make);
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
                entries[i].tag,
                entries[i].type,
                entries[i].count
            );
        }
        free(entries);
    }

    // ifd1
    {
        uint16_t cnt;
        uint32_t ifd2_offset;
        nanoexif_ifd_entry* entries = nanoexif_read_ifd(ne, ifd1_offset, &ifd2_offset, &cnt);
        assert(entries);
        ok(ifd2_offset == 0, "ifd2 offset is 0");
        int i;
        bool compression_ok      = false;
        uint32_t jpeg_offset     = 0;
        for (i=0; i<cnt; i++) {
            D("-- %d\n", i);
            switch (entries[i].tag) {
            case NANOEXIF_TAG_COMPRESSION:
                {
                    assert(entries[i].type == NANOEXIF_TYPE_SHORT);
                    uint16_t * o = nanoexif_get_ifd_entry_data_short(ne, &entries[i]);
                    assert(*o==6);
                    D("jpeg ok\n");
                    free(o);
                    compression_ok = true;
                    break;
                }
            case NANOEXIF_TAG_JPEG_IF_OFFSET:
                {
                    uint32_t *offset = nanoexif_get_ifd_entry_data_long(ne, &entries[i]);
                    assert(offset);
                    D("offset: %d\n", *offset);
                    jpeg_offset = *offset;
                    free(offset);
                    break;
                }
            case NANOEXIF_TAG_JPEG_IF_BYTE_COUNT:
                {
                    uint32_t *offset = nanoexif_get_ifd_entry_data_long(ne, &entries[i]);
                    D("byte_count: %d\n", *offset);
                    jpeg_byte_count = *offset;
                    free(offset);
                    break;
                }
            }
        }
        assert(compression_ok && jpeg_offset && jpeg_byte_count);
        D("jpeg offset: %X\n", jpeg_offset);
        thumb = (uint8_t*)malloc(jpeg_byte_count);
        assert(thumb);
        memcpy(thumb, ne->buf + jpeg_offset, jpeg_byte_count);
        free(entries);
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
