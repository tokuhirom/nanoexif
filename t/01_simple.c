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
    nanoexif * ne = nanoexif_init(ifp);
    assert(ne);

    // read ifd0
    uint16_t *orientation = NULL;
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
                    uint16_t * o = nanoexif_get_ifd_entry_data_short(ne, &entry);
                    assert(*o==6);
                    D("jpeg ok\n");
                    free(o);
                    compression_ok = true;
                    break;
                }
            case NANOEXIF_TAG_JPEG_IF_OFFSET:
                {
                    uint32_t *offset = nanoexif_get_ifd_entry_data_long(ne, &entry);
                    assert(offset);
                    D("offset: %d\n", *offset);
                    jpeg_offset = *offset;
                    free(offset);
                    break;
                }
            case NANOEXIF_TAG_JPEG_IF_BYTE_COUNT:
                {
                    uint32_t *offset = nanoexif_get_ifd_entry_data_long(ne, &entry);
                    D("byte_count: %d\n", *offset);
                    jpeg_byte_count = *offset;
                    free(offset);
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
