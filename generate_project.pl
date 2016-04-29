#!/usr/bin/perl

use strict;
use warnings;
use File::Basename qw/dirname basename/;
use Cwd qw/getcwd/;
##use File::Copy::Recursive qw(rcopy);
use File::Copy qw/copy/;

use lib "./common/Windows_OpenGL/";
use generate_vs2008_project;

#------------------------
# main function

sub main()
{
	print "Please enter a project name: ";
	my $project_name = <>;# wait for user input
	chomp $project_name;
	die if ($project_name eq '');
	print "\n";

	#------
	print "Generating root directory '$project_name'\n";
	mkd($project_name);

	#------
	print "Generating code\n";
	my $DIRSRCS = "$project_name/code";
	mkd($DIRSRCS);
	writeFile("$DIRSRCS/MainClass.cpp", getStrMainClass()) unless (-f "$DIRSRCS/MainClass.cpp");
	
	#------
	print "Generating App_JS_Emscripten\n";
	mkd("$project_name/App_JS_Emscripten");
	writeFile("$project_name/App_JS_Emscripten/compile.bat", 
		'@echo off'."\n"
		.'perl ../../common/JS_Emscripten/compile.pl ../code/*.cpp'."\n"
		.'PAUSE');
	writeFile("$project_name/App_JS_Emscripten/compile.sh", 
		'#!/bin/sh'."\n\n"
		.'perl ../../common/JS_Emscripten/compile.pl ../code/*.cpp'."\n"
		);

	#------
	print "Generating App_Linux\n";
	mkd("$project_name/App_Linux");
	writeFile("$project_name/App_Linux/compile.sh", 
		'#!/bin/sh'."\n"
		.'make -f ../../common/Linux/Makefile SRCS=\'$(wildcard ../code/*.cpp)\' $*'."\n"
		);

	#------
	print "Generating App_VS2008_OpenGL/App_VS2008_SDL\n";
	mkd("$project_name/App_VS2008_OpenGL");
	mkd("$project_name/App_VS2008_SDL");
	my $COMWIN = "./common/Windows_OpenGL";
	my $prevdir = getcwd;
	chdir $COMWIN or die $COMWIN;
	my @l_code = (glob("../../$DIRSRCS/*.cpp"), glob("../../$DIRSRCS/*.h"));
	#print "Source is: ".join(",", @l_code)."\n";
	generate_vs2008_project::processFile(
		"App_VS2008_OpenGL.vcproj.src",
		"../../$project_name/App_VS2008_OpenGL/App_VS2008_OpenGL.vcproj",
		\@l_code,
		0);
	generate_vs2008_project::processFile(
		"App_VS2008_SDL.vcproj.src",
		"../../$project_name/App_VS2008_SDL/App_VS2008_SDL.vcproj",
		\@l_code,
		1);
	chdir $prevdir or die $prevdir;	
	generate_vs2008_project::createSolutionFileVS2008(
		"$project_name/App_VS2008_OpenGL", 'App_VS2008_OpenGL');
	generate_vs2008_project::createSolutionFileVS2008(
		"$project_name/App_VS2008_SDL", 'App_VS2008_SDL');

	#------
	print "Generating App_VS2013_DX_Desktop\n";
	copyr("./common/Windows_VS2013_DX_Desktop", "$project_name/App_VS2013_DX_Desktop", 1);
	
	#------
	print "Generating App_VS2013_DX_Store\n";
	my $in_st = "./common/Windows_VS2013_DX_Store";
	my $out_st = "$project_name/App_VS2013_DX_Store";
	mkd("$project_name/App_VS2013_DX_Store");
	copy("$in_st/App_VS2013_DX_Store.sln", "$out_st/App_VS2013_DX_Store.sln");
	copy("$in_st/App_VS2013_DX_Store.vcxproj", "$out_st/App_VS2013_DX_Store.vcxproj");
	copy("$in_st/Package.appxmanifest", "$out_st/Package.appxmanifest") unless (-f "$out_st/Package.appxmanifest");
	copyr("./common/Windows_VS2013_DX_Store/Assets", "$project_name/App_VS2013_DX_Store/Assets", 0);
	unless (-f "$project_name/copy_work_dir_to_appx.bat")
	{
		rename(
			"$project_name/App_VS2013_DX_Store/copy_work_dir_to_appx.bat", 
			"$project_name/copy_work_dir_to_appx.bat");
	}
	else
	{
		unlink "$project_name/App_VS2013_DX_Store/copy_work_dir_to_appx.bat";
	}
	
	#------
	print "Generating working directory\n";
	mkd("$project_name/WorkDir");
	mkd("$project_name/WorkDir/data");
	unless (-f "$project_name/WorkDir/data/default_font.png")
	{
		copy("common/default_font.png", "$project_name/WorkDir/data/default_font.png");
	}
	foreach my $file (glob("dependancy_libraries/dll/*"))
	{
		copy($file, "$project_name/WorkDir/".basename($file));
	}
	mkd("$project_name/WorkDir/shaders");
	copy("./shaders/fragmentshader.glsl", "$project_name/WorkDir/shaders/fragmentshader.glsl");
	copy("./shaders/vertexshader.glsl", "$project_name/WorkDir/shaders/vertexshader.glsl");
	mkd("$project_name/WorkDirStore");
	mkd("$project_name/WorkDirStore/AppX");
	mkd("$project_name/WorkDirStore/AppX/data");
	
	#------
	print "Done.\n";
	print "\n";
	print "Don't forget to set up the Working Directory of Visual Studio projects\n"
		."to '../WorkDir/' ('../WorkDirStore' for App_VS2013_DX_Store)\n";
	print "\n";
	
	system('PAUSE') unless ($^O eq 'linux');
	
	return 0;
}

exit main();

#------------------------
# copy all files from <first-arg> into <second-arg> (create <second-arg> if it does not exist)
# if <third-arg> is true, clobber existing files

sub copyr($$$)
{
	my ($src, $dest, $clobber) = @_;
	mkd($dest);
	foreach (glob("$src/*"))
	{
		copy($_,"$dest/".basename($_)) if ($clobber || !(-f "$dest/".basename($_)));
	}
}

#------------------------
# write a file <first-arg> with the content string <second-arg> (clobber it if it already exists)

sub writeFile($$) { (open FDWTMP, ">".shift()) and (print FDWTMP shift()) and (close FDWTMP) }

#------------------------
# create a directory <arg> if it does not exist

sub mkd($) { my $d = shift;unless (-d $d) { mkdir $d or die $d } }

#------------------------
# get the content of original MainClass.cpp

sub getStrMainClass() {
return '
#include <string>

#include "../../include/AbstractMainClass.h"
#include "../../include/Sprite.h"

class MainClass : public AbstractMainClass
{
public:
	MainClass(const std::string& arg0, const AppSetupInfos& appSetupInfos)
		:
		AbstractMainClass(arg0.c_str(), appSetupInfos),
		m_str("Press any key and click"),
		m_spritePositionAccurate(-100.0, -100.0) {
	}

	virtual ~MainClass() {}

	void init() {
		m_sprite = new Sprite("data/default_font.png");
	}

	void deinit() {
		delete m_sprite;
	}

	bool update() {
		m_spritePositionAccurate.data[0] +=
			Engine::instance().getFrameDuration() * 0.1;
		return true;
	}

	void render() {
		Engine::instance().clearScreen(CoreUtils::colorBlack);
		Engine::instance().getScene2DMgr().drawText(
			NULL, m_str.c_str(), Int2(10, 10), 18, CoreUtils::colorWhite);
		m_sprite->setPosition(CoreUtils::fromDouble2ToInt2(m_spritePositionAccurate));
		m_sprite->draw();
	}

	void onPointerPressed(MouseManager::ButtonType button, int x, int y) {
		m_spritePositionAccurate = CoreUtils::fromInt2ToDouble2(Int2(x, y));
	}

	void onKeyPressed(int virtualKey) {
		if (Engine::instance().getScene2DMgr().isDrawableCharacter((char)virtualKey)) {
			m_str = " ";
			m_str[0] = (char)virtualKey;
		}
	}

private:
	Sprite* m_sprite;
	Double2 m_spritePositionAccurate;
	std::string m_str;
};


AbstractMainClass* createAbstractMainClass(
	const std::vector<std::string>& arguments)
{
	AppSetupInfos appSetupInfos;
	appSetupInfos.windowSize = Int2(400, 300);
	appSetupInfos.defaultFontImgPath = "data/default_font.png";
	return new MainClass(arguments[0], appSetupInfos);
}


';
}