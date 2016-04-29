#!/usr/bin/perl

use strict;
use warnings;
use File::Basename qw/dirname basename/;
use Cwd qw/getcwd/;
use File::Copy::Recursive qw(rcopy);
use File::Copy qw/copy/;

use lib "./common/Windows_OpenGL/";
use generate_vs2008_project;

sub main()
{
	print "Please enter your application name: ";
	my $application_name = <>;
	chomp $application_name;
	die if ($application_name eq '');
	print "\n";

	print "Generating root directory '$application_name'\n";
	mkd($application_name);

	print "Generating code\n";
	my $DIRSRCS = "$application_name/code";
	mkd($DIRSRCS);
	writeFile("$DIRSRCS/MainClass.cpp", getStrMainClass());
	
	print "Generating App_JS_Emscripten\n";
	mkd("$application_name/App_JS_Emscripten");
	writeFile("$application_name/App_JS_Emscripten/compile.bat", 
		'@echo off'."\n"
		.'perl ../../common/JS_Emscripten/compile.pl ../code/*.cpp'."\n"
		.'PAUSE');
	writeFile("$application_name/App_JS_Emscripten/compile.sh", 
		'#!/bin/sh'."\n\n"
		.'perl ../../common/JS_Emscripten/compile.pl ../code/*.cpp'."\n"
		);

	print "Generating App_Linux\n";
	mkd("$application_name/App_Linux");
	writeFile("$application_name/App_Linux/compile.sh", 
		'#!/bin/sh'."\n"
		.'make -f ../../common/Linux/Makefile SRCS=\'$(wildcard ../code/*.cpp)\' $*'."\n"
		);

	print "Generating App_VS2008_OpenGL/App_VS2008_SDL\n";
	mkd("$application_name/App_VS2008_OpenGL");
	mkd("$application_name/App_VS2008_SDL");
	my $COMWIN = "./common/Windows_OpenGL";
	my $prevdir = getcwd;
	chdir $COMWIN or die $COMWIN;
	my @l_code = (glob("../../$DIRSRCS/*.cpp"), glob("../../$DIRSRCS/*.h"));
	#print "Source is: ".join(",", @l_code)."\n";
	generate_vs2008_project::processFile(
		"App_VS2008_OpenGL.vcproj.src",
		"../../$application_name/App_VS2008_OpenGL/App_VS2008_OpenGL.vcproj",
		\@l_code,
		0);
	generate_vs2008_project::processFile(
		"App_VS2008_SDL.vcproj.src",
		"../../$application_name/App_VS2008_SDL/App_VS2008_SDL.vcproj",
		\@l_code,
		1);
	chdir $prevdir or die $prevdir;
		
	generate_vs2008_project::createSolutionFileVS2008(
		"$application_name/App_VS2008_OpenGL", 'App_VS2008_OpenGL');
	generate_vs2008_project::createSolutionFileVS2008(
		"$application_name/App_VS2008_SDL", 'App_VS2008_SDL');

	print "Generating App_VS2013_DX_Desktop\n";
	rcopy("./common/Windows_VS2013_DX_Desktop", "$application_name/App_VS2013_DX_Desktop");
	
	print "Generating App_VS2013_DX_Store\n";
	rcopy("./common/Windows_VS2013_DX_Store", "$application_name/App_VS2013_DX_Store");
	rename(
		"$application_name/App_VS2013_DX_Store/copy_work_dir_to_appx.bat", 
		"$application_name/copy_work_dir_to_appx.bat");
	
	print "Generating working directory\n";
	mkd("$application_name/WorkDir");
	mkd("$application_name/WorkDir/data");
	copy("common/default_font.png", "$application_name/WorkDir/data/default_font.png");
	foreach my $file (glob("dependancy_libraries/dll/*"))
	{
		copy($file, "$application_name/WorkDir/".basename($file));
	}
	rcopy("shaders", "$application_name/WorkDir/shaders");
	mkdir("$application_name/WorkDirStore");
	mkdir("$application_name/WorkDirStore/AppX");
	mkdir("$application_name/WorkDirStore/AppX/data");
	
	print "Done.\n";
	print "\n";
	print "Don't forget to set up the Working Directory of Visual Studio \n"
		."projects to '../WorkDir/' ('../WorkDirStore' for App_VS2013_DX_Store)\n";
	print "\n";
	
	system('PAUSE') unless ($^O eq 'linux');
	
	return 0;
}

exit main();

#------------------------

sub writeFile($$) { (open FDWTMP, ">".shift()) and (print FDWTMP shift()) and (close FDWTMP) }

#------------------------

sub mkd($) { my $d = shift;unless (-d $d) { mkdir $d or die $d } }

#------------------------

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