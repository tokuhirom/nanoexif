#include "../nanoexif.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

static inline uint32_t read_32(nanoexif_endian endian, const uint8_t *buf) {
    if (endian == NANOEXIF_LITTLE_ENDIAN) {
        return (buf[3]<<24) | (buf[2]<<16) | (buf[1]<<8) | buf[0];
    } else {
        return (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3];
    }
}

bool dump(nanoexif *ne, int level) {
    uint16_t cnt = nanoexif_read_ifd_cnt(ne);
    uint16_t i;
    for (i=0; i<cnt; i++) {
        nanoexif_ifd_entry entry;
        assert(nanoexif_read_ifd_entry(ne, &entry));
        const char * tag = nanoexif_tag_name(entry.tag);
        int j;
        for (j=0; j<level*3+1; j++) {
            printf("-");
        }
        printf(" tag: 0x%04X(%s), type:%d, count:%d\n", entry.tag, tag ? tag : "(null)", entry.type, entry.count);
        switch (entry.type) {
        case NANOEXIF_TYPE_RATIONAL:
            {
                uint32_t *x = nanoexif_get_ifd_entry_data_rational(ne, &entry);
                assert(x);
                uint16_t j;
                for (j=0; j<entry.count*2; j+=2) {
                    printf("  %d/%d\n", x[j], x[j+1]);
                }
                free(x);
            }
            break;
        case NANOEXIF_TYPE_ASCII: // 2
            {
                char *x = nanoexif_get_ifd_entry_data_ascii(ne, &entry);
                assert(x);
                printf("  %s\n", x); // x is null terminated.
                free(x);
            }
            break;
        case NANOEXIF_TYPE_SHORT:
            {
                uint16_t *x = nanoexif_get_ifd_entry_data_short(ne, &entry);
                assert(x);
                uint16_t j;
                for (j=0;j<entry.count; j++) {
                    printf("  %d\n", x[j]);
                }
                free(x);
            }
            break;
        case NANOEXIF_TYPE_LONG:
            {
                uint32_t *x = nanoexif_get_ifd_entry_data_long(ne, &entry);
                assert(x);
                uint16_t j;
                for (j=0;j<entry.count; j++) {
                    printf("  %d\n", x[j]);
                }
                free(x);
            }
            break;
        default:
            printf("UNKNOWN type: %d\n", entry.type);
            break;
        }
        if (entry.tag == NANOEXIF_TAG_EXIF_OFFSET || entry.tag == NANOEXIF_TAG_GPS_INFO) { // has sub id 
            uint32_t * x = nanoexif_get_ifd_entry_data_long(ne, &entry);
            assert(x);
            uint32_t offset = *x;
            free(x);

            long orig = ftell(ne->fp);
            if (orig == -1) {
                return false;
            }
            if (fseek(ne->fp, ne->offset+(offset), SEEK_SET)!=0) {
                printf("err\n");
                break;
            }
            dump(ne, level+1);

            if (fseek(ne->fp, orig, SEEK_SET)!=0) { // restore
                printf("oops\n");
                return false;
            }
        }
    }
    int ret = nanoexif_skip_ifd_body(ne);
    assert(ret >= 0);
    if (ret == 0) { // finished
        return false;
    } else {
        return true;
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s src.jpg\n", argv[0]);
        return 1;
    }

    FILE * fp = fopen(argv[1], "rb");
    assert(fp);
    nanoexif * ne = nanoexif_init(fp);
    assert(ne);
    while (dump(ne, 0)) {
        ; // nop
    }
    nanoexif_free(ne);
    fclose(fp);
    return 0;
}
