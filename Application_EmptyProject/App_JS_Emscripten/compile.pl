#!/usr/bin/perl

use strict;
use warnings;
use Data::Dumper;

use constant ENABLE_SOUND => 0;

#----------------------------------
# uncompress third-party libraries if required

if (ENABLE_SOUND)
{
	system("cd third-party && unzip Vorbis-master.zip") unless (-d "third-party/Vorbis-master");
	system("cd third-party && unzip Ogg-master.zip") unless (-d "third-party/Ogg-master");
	system("cd third-party && unzip freealut-master.zip") unless (-d "third-party/freealut-master");
}

#----------------------------------
# list cpp files to compile

my @l_files = (
	"../MainClass.cpp", #glob("../App_VS2008GL/AnUnconventionalWeapon/*.cpp"),
	#glob("../../include/*.h"),
	glob("../../source/*.cpp"),
	glob("../../source/opengl/*.cpp"),
	);

unless (ENABLE_SOUND)
{
	my %h_files = map { $_ => 1 } @l_files;
	foreach my $key (keys %h_files) { delete $h_files{$key} if ($key =~ /Sound/); }
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
	$INCS.= " -Ithird-party/freealut-master/include -Ithird-party/Ogg-master/include -Ithird-party/Vorbis-master/include";
}
my $WARNINGS = "-Wno-tautological-constant-out-of-range-compare -Wno-dangling-else";
my $DEFINES = "-DUSES_LINUX -DUSES_JS_EMSCRIPTEN";
my $LIBS = " -lm -lGL -lGLU -lGLEW -lglut  -s DEMANGLE_SUPPORT=1 ";#-lGLESv2 -lEGL -lm -lX11

#----------------------------------
# run the compilation command

system("python $EMS_BIN $files $INCS $DEFINES $WARNINGS $LIBS -o output.html");# -s ERROR_ON_UNDEFINED_SYMBOLS=1
