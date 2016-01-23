#!/usr/bin/perl

use strict;
use warnings;
use Data::Dumper;

use constant ENABLE_SOUND => 1;
use constant ENABLE_3D => 0;# not supported yet
use constant ENABLE_LEGACY_MODE => 0;
my $THIRD_PARTY_LIBS_DIR = "../../dependancy_libraries/emscripten-third-party";

print "--------------------------------------------\n";

#----------------------------------
# get files to compile (except the one from the engine)

my @src_files;
@src_files = (@src_files, glob($_)) foreach (@ARGV);

print "Source files:\n\t".join("\n\t", @src_files)."\n";

#----------------------------------
# uncompress third-party libraries if required

sub unzipIfRequired($)
{
	my $dir = shift;
	if (!-d "$THIRD_PARTY_LIBS_DIR/$dir" || -d "$THIRD_PARTY_LIBS_DIR/$dir/$dir")
	{
		if ($^O eq  'linux')
		{
			system("cd $THIRD_PARTY_LIBS_DIR && unzip $dir.zip") ;
		}
		else
		{print "$THIRD_PARTY_LIBS_DIR/$dir/$dir\n";
			while (!-d "$THIRD_PARTY_LIBS_DIR/$dir" || -d "$THIRD_PARTY_LIBS_DIR/$dir/$dir")
			{
				print "Please unzip $THIRD_PARTY_LIBS_DIR/$dir (note: do not put $dir in $dir)\n";
				system("PAUSE");
			}
		}
	}
}

if (ENABLE_SOUND)
{
	unzipIfRequired('Vorbis-master');
	unzipIfRequired('Ogg-master');
	unzipIfRequired('freealut-master');
}

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

my $EMS_PATH = $ENV{'EMSCRIPTEN'}; #"D:/progs/Emscripten/emscripten/1.35.0";
my $EMS_INC = "$EMS_PATH/system/include";
my $EMS_BIN = "$EMS_PATH/em++";

my $INCS = "-I../../ -I$EMS_INC";
if (ENABLE_SOUND)
{
	$INCS.= " -I$THIRD_PARTY_LIBS_DIR/freealut-master/include -I$THIRD_PARTY_LIBS_DIR/Ogg-master/include -I$THIRD_PARTY_LIBS_DIR/Vorbis-master/include";
}
my $WARNINGS = "-Wno-tautological-constant-out-of-range-compare -Wno-dangling-else";
my $DEFINES = "-DUSES_JS_EMSCRIPTEN -DUSES_SDL_INSTEAD_OF_GLUT";#-DUSES_LINUX 
$DEFINES .= " -DUSES_SOUND -DUSES_SDL_FOR_SOUND" if ENABLE_SOUND;
$DEFINES .= " -DUSES_SCENE3D" if ENABLE_3D;

my $OPTS = " -O2 -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=64000000 -s ASSERTIONS=1 ";
my $LIBS = " -s USE_SDL=2  -s GL_UNSAFE_OPTS=0  ".(ENABLE_LEGACY_MODE?'-s LEGACY_GL_EMULATION=1  ':'');
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
