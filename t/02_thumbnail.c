#include "nanotap.h"
#include <nanoexif-easy.h>

int main(int argc, char **argv) {
    uint16_t orientation;
    uint32_t jpeg_byte_count;

    FILE * fp;
    if (!(fp = fopen("t/data/sample-iphone.jpg", "rb"))) {
        perror(argv[0]);
        return 1;
    }

    char * thumb = nanoexif_easy_thumbnail(fp, &orientation, &jpeg_byte_count);
    ok(!!thumb, "thumb");
    ok( orientation == 6, "orientation = 6");
    ok(memcmp(thumb, "\xFF\xD8", 2)==0, "jpeg soi");
    ok(memcmp(thumb+(jpeg_byte_count-2), "\xFF\xD9", 2)==0, "jpeg eoi");
    free(thumb);

    done_testing();
}

