use inc::Module::Install;
name 'libnanoexif';
license 'mit';

clib_setup;

my $endian = unpack("S", pack("C2", 0, 1)) == 1 ? "NANOEXIF_BIG_ENDIAN" : "NANOEXIF_LITTLE_ENDIAN";
print "endian: $endian\n";

my @src = qw(src/nanoexif.c src/nanoexif-tagname.c src/nanoexif-easy.c);

my $e = env_for_c(
    CCFLAGS => "-DDEBUG -std=c99 -DNANOEXIF_MACHINE_ENDIAN=$endian",
    CPPPATH => 'src/',
);
$e->enable_warnings;
$e->test('t/01_simple', ['t/01_simple.c', @src]);
$e->test('t/02_thumbnail', ['t/02_thumbnail.c', @src]);
$e->program('./tools/nanoexif-dump', ['tools/nanoexif-dump.c', @src]);

postambles(<<'...');
docs: Doxyfile src/*.c src/*.h
	doxygen && cd docs/ && git add . && git ci -m 'updated docs' && git push origin gh-pages && cd .. && git add docs && git ci -m 'updated docs' docs

...

WriteAll();

# gcc -DDEBUG -DTEST -DNANOEXIF_MACHINE_ENDIAN=NANOEXIF_BIG_ENDIAN -arch ppc nanoexif.c -o nanoexif
