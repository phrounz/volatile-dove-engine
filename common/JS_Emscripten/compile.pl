#!/usr/bin/perl

use strict;
use warnings;
use Data::Dumper;

use constant ENABLE_SOUND => 1;
use constant ENABLE_3D => 0;# not supported yet
use constant ENABLE_LEGACY_MODE => 1;

print "--------------------------------------------\n";

#----------------------------------
# get files to compile (except the one from the engine)

my @src_files;
@src_files = (@src_files, glob($_)) foreach (@ARGV);

print "Source files:\n\t".join("\n\t", @src_files)."\n";

#----------------------------------
# list cpp files to compile

my @l_files = (
	@src_files,
	glob("../../source/*.cpp"),
	glob("../../source/opengl/*.cpp"),
	);
#glob("../../include/*.h"),

unless (ENABLE_SOUND)
{
	my %h_files = map { $_ => 1 } @l_files;
	foreach my $key (keys %h_files) {
		if ($key =~ /Sound/)
		{
			print "Removed from compilation: $key\n";
			delete $h_files{$key};
		}
	}
	@l_files = keys %h_files;
}

unless (ENABLE_3D)
{
	my %h_files = map { $_ => 1 } @l_files;
	foreach my $key (keys %h_files) {
		if ($key =~ m/\/(Obj3D|Scene3DPrivate|Scene3D|VBO|Camera|Material|VBOGroup|GroupOfQuads|GroupOfQuadsByTexture|Meshes3D)\.cpp$/)
		{
			print "Removed from compilation: $key\n";
			delete $h_files{$key};
		}
	}
	@l_files = keys %h_files;
}

my $files = join(' ', @l_files);

#----------------------------------
# prepare parts of command

my $EMS_PATH = ($^O eq 'linux' ? "../../common/JS_Emscripten/emsdk_portable/emscripten/master" : $ENV{'EMSCRIPTEN'}); #"D:/progs/Emscripten/emscripten/1.35.0";
if (!defined $EMS_PATH || $EMS_PATH eq '')
{
	print "ERROR: Could not find Emscripten path\n";
	exit 1;
}
my $EMS_INC = "$EMS_PATH/system/include";
my $EMS_BIN = "$EMS_PATH/em++";

my $INCS = "-I../../ -I$EMS_INC";
my $WARNINGS = "-Wno-tautological-constant-out-of-range-compare -Wno-dangling-else";
my $DEFINES = "-DUSES_JS_EMSCRIPTEN -DUSES_SDL_INSTEAD_OF_GLUT";#-DUSES_LINUX
$DEFINES .= " -DUSES_SOUND -DUSES_SDL_FOR_SOUND" if ENABLE_SOUND;
$DEFINES .= " -DUSES_SCENE3D" if ENABLE_3D;

my $OPTS = " -O2 -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=64000000 -s ASSERTIONS=1 ";
my $LIBS = " -s USE_SDL=2  -s GL_UNSAFE_OPTS=0  ".(ENABLE_LEGACY_MODE?'-s LEGACY_GL_EMULATION=1  ':'');
# -s USE_OGG=1 -s USE_VORBIS=1
my $DATA_LINK = " --preload-file ../WorkDir/default_font.png\@default_font.png";
$DATA_LINK .= " --preload-file ../WorkDir/data\@data" if (-d "../WorkDir/data");
# "-lGLESv2 -lEGL -lm -lX11"; -lGLEW -lm -lGL -lGLU -lglut
# -s DEMANGLE_SUPPORT=1 --bind -lglfw
# -s ERROR_ON_UNDEFINED_SYMBOLS=1

#----------------------------------
# run the compilation command

my $cmd = "python $EMS_BIN $OPTS $files $INCS $DEFINES $WARNINGS $LIBS -o output.html $DATA_LINK";
print "\n$cmd\n";
print "--------------------------------------------\n\n";
system($cmd);
