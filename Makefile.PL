use inc::Module::Install;
name 'libnanoexif';
license 'mit';

clib_setup;

my $endian = unpack("S", pack("C2", 0, 1)) == 1 ? "NANOEXIF_BIG_ENDIAN" : "NANOEXIF_LITTLE_ENDIAN";
print "endian: $endian\n";

my $e = env_for_c(CCFLAGS => "-DDEBUG -std=c99 -DNANOEXIF_MACHINE_ENDIAN=$endian");
$e->enable_warnings;
$e->test('t/01_simple', ['t/01_simple.c', 'nanoexif.c', 'nanoexif-tagname.c']);
$e->program('./tools/nanoexif-dump', ['nanoexif.c', 'tools/nanoexif-dump.c', 'nanoexif-tagname.c']);

WriteAll();

# gcc -DDEBUG -DTEST -DNANOEXIF_MACHINE_ENDIAN=NANOEXIF_BIG_ENDIAN -arch ppc nanoexif.c -o nanoexif
