use inc::Module::Install;
name 'libnanoexif';
clib_setup;

my $endian = unpack("S", pack("C2", 0, 1)) == 1 ? "NANOEXIF_BIG_ENDIAN" : "NANOEXIF_LITTLE_ENDIAN";
print "endian: $endian\n";

my $e = env_for_c();
$e->enable_warnings;
$e->test('./nanoexif', ['nanoexif.c'], CCFLAGS => "-DTEST -DDEBUG -std=c99 -DNANOEXIF_MACHINE_ENDIAN=$endian");

WriteAll();

# gcc -DDEBUG -DTEST -DNANOEXIF_MACHINE_ENDIAN=NANOEXIF_BIG_ENDIAN -arch ppc nanoexif.c -o nanoexif
