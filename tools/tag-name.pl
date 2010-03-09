use common::sense;
use autodie;

use Image::ExifTool::Exif;
use Scalar::Util qw/reftype/;
use String::CamelCase qw/camelize decamelize/;

my %src = %Image::ExifTool::Exif::Main;
my @key = sort { $a <=> $b } grep /^[0-9]+$/, keys %src;

do {
    open my $fh, '>', 'nanoexif-tagname.c';
    print {$fh} <<'...';
#include <stdint.h>

const char *nanoexif_tag_name(uint32_t n) {
    switch (n) {
...
    for my $k (@key) {
        my @name = get_name($src{$k});
        if (@name==1) {
            printf {$fh} qq{    case 0x%04X: return "$name[0]";\n}, $k;
        }
    }
    print {$fh} <<'...';
    }
    return NULL;
}
...
    close $fh;
};

#ub {
#   return;
#   open my $fh, '>', 'nanoexif-tag.h';
#   print {$fh} "#ifndef NANOEXIF_TAG_H__\n";
#   print {$fh} "#define NANOEXIF_TAG_H__\n\n";
#   my %seen;
#   for my $k (@key) {
#       next if $k > 0x300;
#       my @name = get_name($src{$k});
#       for my $name (@name) {
#           # printf "SEEEEN $name %d\n", $k if $name =~ /ImageWidth/;
#           print "SEEEEN $name $k\n" if $seen{$name} && $seen{$name} ne $k;
#           $seen{$name} = $k;
#           printf {$fh} qq{#define NANOEXIF_TAG_%-30s 0x%04X\n}, uc(decamelize($name)), $k;
#       }
#   }
#   print {$fh} "\n#endif  // NANOEXIF_TAG_H__\n";
#   close $fh;
#->();

sub get_name {
    my $v = shift;
    my $type = reftype($v);
    given ($type) {
        when ('ARRAY') {
            return map { get_name($_) } @$v;
        }
        when ('HASH') {
            return ($v->{Name});
        }
        default {
            return ($v);
        }
    }
};
