use inc::Module::Install;
name 'libnanoexif';
clib_setup;

my $e = env_for_c();
$e->enable_warnings;
$e->test('./nanoexif', ['nanoexif.c'], CCFLAGS => '-DTEST -DDEBUG -std=c99');

WriteAll();
