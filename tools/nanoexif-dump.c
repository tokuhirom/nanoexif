#include "../nanoexif.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void dump(nanoexif *ne, int level, uint32_t ifd_offset) {
    uint32_t next_offset;

    do {
        uint16_t cnt;
        nanoexif_ifd_entry* entries = nanoexif_read_ifd(ne, ifd_offset, &next_offset, &cnt);
        printf("tag cnt: %d, next offset: %d\n", cnt, next_offset);
        uint16_t i;
        for (i=0; i<cnt; i++) {
            const char * tag = nanoexif_tag_name(entries[i].tag);
            int j;
            for (j=0; j<level*3+1; j++) {
                printf("-");
            }
            printf(" tag: 0x%04X(%s), type:%d, count:%d\n", entries[i].tag, tag ? tag : "(null)", entries[i].type, entries[i].count);
            switch (entries[i].type) {
            case NANOEXIF_TYPE_RATIONAL:
                {
                    uint32_t *x = nanoexif_get_ifd_entry_data_rational(ne, &entries[i]);
                    assert(x);
                    uint16_t j;
                    for (j=0; j<entries[i].count*2; j+=2) {
                        printf("  %d/%d\n", x[j], x[j+1]);
                    }
                    free(x);
                }
                break;
            case NANOEXIF_TYPE_ASCII: // 2
                {
                    char *x = nanoexif_get_ifd_entry_data_ascii(ne, &entries[i]);
                    assert(x);
                    printf("  %s\n", x); // x is null terminated.
                    free(x);
                }
                break;
            case NANOEXIF_TYPE_SHORT:
                {
                    uint16_t *x = nanoexif_get_ifd_entry_data_short(ne, &entries[i]);
                    assert(x);
                    uint16_t j;
                    for (j=0;j<entries[i].count; j++) {
                        printf("  %d\n", x[j]);
                    }
                    free(x);
                }
                break;
            case NANOEXIF_TYPE_LONG:
                {
                    uint32_t *x = nanoexif_get_ifd_entry_data_long(ne, &entries[i]);
                    assert(x);
                    uint16_t j;
                    for (j=0;j<entries[i].count; j++) {
                        printf("  %d\n", x[j]);
                    }
                    free(x);
                }
                break;
            default:
                printf("UNKNOWN type: %d\n", entries[i].type);
                break;
            }
            if (entries[i].tag == NANOEXIF_TAG_EXIF_OFFSET || entries[i].tag == NANOEXIF_TAG_GPS_INFO) { // has sub id 
                uint32_t * x = nanoexif_get_ifd_entry_data_long(ne, &entries[i]);
                assert(x);
                uint32_t offset = *x;
                free(x);

                dump(ne, level+1, offset);
            }
        }
        ifd_offset = next_offset;
    } while (next_offset != 0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s src.jpg\n", argv[0]);
        return 1;
    }

    FILE * fp = fopen(argv[1], "rb");
    assert(fp);
    uint32_t ifd_offset;
    nanoexif * ne = nanoexif_init(fp, &ifd_offset);
    printf("offset: %d\n", ifd_offset);
    assert(ne);

    dump(ne, 0, ifd_offset);

    nanoexif_free(ne);
    fclose(fp);
    return 0;
}
