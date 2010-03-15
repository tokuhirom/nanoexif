#include <nanoexif.h>
#include <nanoexif-easy.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s src.jpg dst.jpg\n", argv[0]);
        return 1;
    }

    FILE * ifp = fopen(argv[1], "rb");
    assert(ifp);
    uint16_t orientation;
    uint32_t jpeg_byte_count;
    char * thumb = nanoexif_easy_thumbnail(ifp, &orientation, &jpeg_byte_count);
    assert(thumb);
    fclose(ifp);

    FILE * ofp = fopen(argv[2], "wb");
    assert(ofp);
    assert(fwrite(thumb, sizeof(char), jpeg_byte_count, ofp) == jpeg_byte_count);
    fclose(ofp);
}

