#include <nanoexif-easy.h>
#include <nanoexif.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * @args FILE * fp: file pointer for reading exif
 * @args uint16_t * orientation: jpeg file orientation from exif
 * @args jpeg_byte_count : byte count for thumbnail will set.
 * @return thumbnail data will be return.you should free(2) after used.return NULL if error occurred.
 */
char * nanoexif_easy_thumbnail(FILE * fp, uint16_t *orientation, uint32_t *jpeg_byte_count) {
    uint32_t ifd0_offset;
    nanoexif * ne = nanoexif_init(fp, &ifd0_offset);
    if (!ne) { return NULL; }

    // initialize
    *orientation     = 0;
    *jpeg_byte_count = 0;

    // ifd0
    uint32_t ifd1_offset;
    {
        uint16_t cnt;
        nanoexif_ifd_entry* entries = nanoexif_read_ifd(ne, ifd0_offset, &ifd1_offset, &cnt);
        int i;
        for (i=0; i<cnt; i++) {
            switch (entries[i].tag) {
            case NANOEXIF_TAG_ORIENTATION:
                {
                    uint16_t *x = nanoexif_get_ifd_entry_data_short(ne, &entries[i]);
                    if (!x) {
                        free(entries);
                        nanoexif_free(ne);
                        return NULL;
                    }
                    *orientation = *x;
                    free(x);
                }
                break;
            }
        }
        free(entries);
    }

    // ifd1
    uint8_t * thumb = NULL; 
    {
        uint16_t cnt;
        uint32_t ifd2_offset;
        nanoexif_ifd_entry* entries = nanoexif_read_ifd(ne, ifd1_offset, &ifd2_offset, &cnt);
        if (!entries) {
            nanoexif_free(ne);
            return NULL;
        }

        int i;
        bool compression_ok      = false;
        uint32_t jpeg_offset     = 0;
        for (i=0; i<cnt; i++) {
            switch (entries[i].tag) {
            case NANOEXIF_TAG_COMPRESSION:
                {
                    uint16_t * o = nanoexif_get_ifd_entry_data_short(ne, &entries[i]);
                    if (!o) {
                        free(entries);
                        nanoexif_free(ne);
                        return NULL;
                    }
                    if (*o!=6) {
                        free(o);
                        free(entries);
                        nanoexif_free(ne);
                        return NULL;
                    }
                    free(o);
                    compression_ok = true;
                    break;
                }
            case NANOEXIF_TAG_JPEG_IF_OFFSET:
                {
                    uint32_t *offset = nanoexif_get_ifd_entry_data_long(ne, &entries[i]);
                    if (!offset) {
                        free(entries);
                        nanoexif_free(ne);
                        return NULL;
                    }
                    jpeg_offset = *offset;
                    free(offset);
                    break;
                }
            case NANOEXIF_TAG_JPEG_IF_BYTE_COUNT:
                {
                    uint32_t *offset = nanoexif_get_ifd_entry_data_long(ne, &entries[i]);
                    if (!offset) {
                        free(entries);
                        nanoexif_free(ne);
                        return NULL;
                    }
                    *jpeg_byte_count = *offset;
                    free(offset);
                    break;
                }
            }
        }
        free(entries);

        if (!(compression_ok && jpeg_offset && *jpeg_byte_count)) {
            nanoexif_free(ne);
            return NULL;
        }

        thumb = (uint8_t*)malloc(*jpeg_byte_count);
        if (!thumb) {
            nanoexif_free(ne);
            return NULL;
        }
        memcpy(thumb, ne->buf + jpeg_offset, *jpeg_byte_count);
    }

    nanoexif_free(ne);
    return thumb;
}
