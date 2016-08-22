#!/usr/bin/perl

use strict;
use warnings;
use File::Basename qw/dirname basename/;
use Cwd qw/getcwd/;
##use File::Copy::Recursive qw(rcopy);
use File::Copy qw/copy/;
use Data::Dumper;

use lib ".";
use generate_project;

#------------------------
# main function

sub main()
{
	# if windows, change the width of the console window
	#if ($^O eq 'MSWin32') { `mode 180,1200 ` }

	$SIG{INT} = sub { print "\nCaught a sigint, stopping now.\n"; exit 1; };

	print "Please select a project to create/maintain:\n";
	print "0) NEW PROJECT\n";
	my $i = 1;
	my %h_project_by_answer;
	foreach my $project (glob("*"))
	{
		if (-d $project && -f "$project/setup.ini")
		{
			print "$i) $project\n";
			$h_project_by_answer{$i} = $project;
			$i++;
		}
	}

	print "Answer: ";
	my $project_id = <>;# wait for user input
	chomp $project_id;

	my $project_name = undef;
	if ($project_id == 0)
	{
		print "Please enter the name of the project: ";
		$project_name = <>;# wait for user input
		chomp $project_name;
		die if ($project_name eq '');
	}
	else
	{
		$project_name = $h_project_by_answer{$project_id};
		die "bad arg\n" unless defined $project_name;
	}
	print "\n";

	#------
	print "Generating root directory '$project_name'\n";
	mkd($project_name);

	#------
	print "Generating or reading setup.ini\n";
	my $rh_setup_desc_by_var = {
		steam_sdk_path => 'Steam SDK path or empty string if the project does not use Steam SDK'."\n; Example: ADD_CC=' -I/usr/include/lua5.2 -fpermissive' ADD_LD='-llua5.2 ' ARCH=64",
		linux_make_additional_arguments => 'Arguments added when building App_Linux/compile.sh'."\n; Example: c:\\Steam SDK - My Project",
		windows_additional_include_dirs => 'Additional include directories for Windows, comma-separated'."\n; ".'Example: $(SolutionDir)\..\lua\lua51_VS2008\include;..\..',
		windows_additional_lib_dirs => 'Additional library directories for Windows, comma-separated',
		windows_additional_libs => 'Additional libraries for Windows, comma-separated'."\n; ".'Example: $(SolutionDir)\..\lua\lua51_VS2008\lua5.1.lib',
		windows_additional_defines => 'Additional #defines for Windows, comma-separated',
		visual_studio_app_guid => 'GUID of the Visual Studio project (or empty for default)'
		};
	my $rh_setup_value_by_var = {};
	if (-f "$project_name/setup.ini")
	{
		$rh_setup_value_by_var = readSetupIniFile("$project_name/setup.ini");
		foreach my $var (keys %$rh_setup_desc_by_var)
		{
			$rh_setup_value_by_var->{$var} = '' unless defined $rh_setup_value_by_var->{$var};
		}
		writeFile("$project_name/setup.ini",
			join("\n", map { "; $rh_setup_desc_by_var->{$_}\n$_=".($rh_setup_value_by_var->{$_})."\n" } sort keys %$rh_setup_desc_by_var));
	}
	else
	{
		writeFile("$project_name/setup.ini",
			join("\n", map { "; $rh_setup_desc_by_var->{$_}\n$_=\n" } sort keys %$rh_setup_desc_by_var));
		foreach (keys %$rh_setup_desc_by_var) { $rh_setup_value_by_var->{$_} = '' }
	}
		#USES_STEAM_INTEGRATION;

	#------
	print "Generating code directory\n";
	my $DIRSRCS = "$project_name/code";
	mkd($DIRSRCS);
	writeFile("$DIRSRCS/MainClass.cpp", getStrMainClass()) unless (-f "$DIRSRCS/MainClass.cpp");

	#------
	print "Generating App_JS_Emscripten\n";
	mkd("$project_name/App_JS_Emscripten");
	generate_project::writeFileWithConfirmationForDifferences("$project_name/App_JS_Emscripten/.gitignore",
		join("\n", qw/output.js output.data output.html output.html.mem compile.bat.new compile.sh.new .gitignore.new/)."\n");
	generate_project::writeFileWithConfirmationForDifferences("$project_name/App_JS_Emscripten/compile.bat",
		'@echo off'."\n"
		.'perl ../../common/JS_Emscripten/compile.pl ../code/*.cpp'."\n"
		."PAUSE\n");
	generate_project::writeFileWithConfirmationForDifferences("$project_name/App_JS_Emscripten/compile.sh",
		'#!/bin/sh'."\n\n"
		.'perl ../../common/JS_Emscripten/compile.pl ../code/*.cpp'."\n"
		);

	my @l_windows_additional_include_dirs = split(/,/, $rh_setup_value_by_var->{windows_additional_include_dirs});
	my @l_windows_additional_lib_dirs = split(/,/, $rh_setup_value_by_var->{windows_additional_lib_dirs});
	my @l_windows_additional_libs = split(/,/, $rh_setup_value_by_var->{windows_additional_libs});
	my @l_windows_additional_defines = split(/,/, $rh_setup_value_by_var->{windows_additional_defines});
	my @l_visual_studio_app_guids = split(/,/, $rh_setup_value_by_var->{visual_studio_app_guid});
	my @l_steam_sdk_path = split(/,/, $rh_setup_value_by_var->{steam_sdk_path});
	my @l_args_process = (
		\@l_steam_sdk_path, \@l_windows_additional_include_dirs, \@l_windows_additional_lib_dirs, 
		\@l_windows_additional_libs, \@l_windows_additional_defines, \@l_visual_studio_app_guids, \@l_steam_sdk_path);
	
	#------
	
	print "Generating App_Linux\n";
	mkd("$project_name/App_Linux");
	generate_project::processLinuxFiles($project_name, 'App_Linux', $rh_setup_value_by_var->{linux_make_additional_arguments}, \@l_steam_sdk_path);

	#------
	print "Generating App_VS2008_OpenGL, App_VS2008_SDL, App_VS2013_DX_Desktop, and App_VS2013_DX_Store\n";
	# create directories
	mkd("$project_name/App_VS2008_OpenGL");
	mkd("$project_name/App_VS2008_SDL");
	mkd("$project_name/App_VS2013_DX_Desktop");
	mkd("$project_name/App_VS2013_DX_Store");
	# create projects
	my $COMWIN = "./common/Windows";
	my $prevdir = getcwd;
	chdir $COMWIN or die $COMWIN;
	my $relt = "../../$project_name";
	my @l_code = (
		glob("../../$DIRSRCS/*.cpp"), glob("../../$DIRSRCS/*.h")
		#glob("../../$DIRSRCS/*/*.cpp"), glob("../../$DIRSRCS/*/*.h")
		);
	#print "Source is: ".join(",", @l_code)."\n";
	my $gitignore_visual_studio = join("\n", qw/Debug Release *.ncb *.suo *.vcproj.*.user *.vcproj.new *.vcxproj.new .gitignore.new/)."\n";
	generate_project::writeFileWithConfirmationForDifferences("$relt/App_VS2008_OpenGL/.gitignore", $gitignore_visual_studio);
	generate_project::writeFileWithConfirmationForDifferences("$relt/App_VS2008_SDL/.gitignore", $gitignore_visual_studio);
	generate_project::writeFileWithConfirmationForDifferences("$relt/App_VS2013_DX_Desktop/.gitignore", $gitignore_visual_studio);
	generate_project::writeFileWithConfirmationForDifferences("$relt/App_VS2013_DX_Store/.gitignore", $gitignore_visual_studio);

	generate_project::processVisualStudioProjectFile(
		"App_VS2008_OpenGL.vcproj.src", "$relt/App_VS2008_OpenGL/App_VS2008_OpenGL.vcproj", \@l_code, 0, 0, 0, \@l_args_process);
	generate_project::processVisualStudioProjectFile(
		"App_VS2008_SDL.vcproj.src", "$relt/App_VS2008_SDL/App_VS2008_SDL.vcproj", \@l_code, 1, 0, 0, \@l_args_process);
	generate_project::processVisualStudioProjectFile(
		"App_VS2013_DX_Desktop.vcxproj.src", "$relt/App_VS2013_DX_Desktop/App_VS2013_DX_Desktop.vcxproj", \@l_code, 0, 1, 1, \@l_args_process);
	generate_project::processVisualStudioProjectFile(
		"App_VS2013_DX_Store.vcxproj.src", "$relt/App_VS2013_DX_Store/App_VS2013_DX_Store.vcxproj", \@l_code, 0, 1, 1, \@l_args_process);

	generate_project::processVisualStudioProjectUserFile(
		"App_VS2008_OpenGL.vcproj.user.src", "$relt/App_VS2008_OpenGL/App_VS2008_OpenGL.vcproj.user");
	generate_project::processVisualStudioProjectUserFile(
		"App_VS2008_SDL.vcproj.user.src", "$relt/App_VS2008_SDL/App_VS2008_SDL.vcproj.user");
	generate_project::processVisualStudioProjectUserFile(
		"App_VS2013_DX_Desktop.vcxproj.user.src", "$relt/App_VS2013_DX_Desktop/App_VS2013_DX_Desktop.vcxproj.user");
	generate_project::processVisualStudioProjectUserFile(
		"App_VS2013_DX_Store.vcxproj.user.src", "$relt/App_VS2013_DX_Store/App_VS2013_DX_Store.vcxproj.user");

	chdir $prevdir or die $prevdir;
	# create solution files
	copyOrFail(
		"./common/Windows/App_VS2008_OpenGL.sln",
		"$project_name/App_VS2008_OpenGL/App_VS2008_OpenGL.sln",
		1);
	copyOrFail(
		"./common/Windows/App_VS2008_SDL.sln",
		"$project_name/App_VS2008_SDL/App_VS2008_SDL.sln",
		1);
	copyOrFail(
		"./common/Windows/App_VS2013_DX_Desktop.sln",
		"$project_name/App_VS2013_DX_Desktop/App_VS2013_DX_Desktop.sln",
		1);
	copyOrFail(
		"./common/Windows/Windows_VS2013_DX_Store/App_VS2013_DX_Store.sln",
		"$project_name/App_VS2013_DX_Store/App_VS2013_DX_Store.sln",
		1);

	mkd("$project_name/WorkDir/linux_dependancies");
	mkd("$project_name/WorkDir/linux_dependancies/32bit");
	mkd("$project_name/WorkDir/linux_dependancies/64bit");
	if (defined $rh_setup_value_by_var->{steam_sdk_path} && $rh_setup_value_by_var->{steam_sdk_path} ne '')
	{
		copyOrFail($rh_setup_value_by_var->{steam_sdk_path}."/sdk/public/steam/lib/win32/sdkencryptedappticket.dll", "$project_name/WorkDir/sdkencryptedappticket.dll");
		copyOrFail($rh_setup_value_by_var->{steam_sdk_path}."/sdk/public/steam/lib/win64/sdkencryptedappticket64.dll", "$project_name/WorkDir/sdkencryptedappticket64.dll");
		copyOrFail($rh_setup_value_by_var->{steam_sdk_path}."/sdk/redistributable_bin/steam_api.dll", "$project_name/WorkDir/steam_api.dll");
		copyOrFail($rh_setup_value_by_var->{steam_sdk_path}."/sdk/redistributable_bin/win64/steam_api64.dll", "$project_name/WorkDir/steam_api64.dll");
		copyOrFail($rh_setup_value_by_var->{steam_sdk_path}."/sdk/public/steam/lib/linux32/libsdkencryptedappticket.so",
			"$project_name/WorkDir/linux_dependancies/32bit/libsdkencryptedappticket.so");
		copyOrFail($rh_setup_value_by_var->{steam_sdk_path}."/sdk/public/steam/lib/linux64/libsdkencryptedappticket.so",
			"$project_name/WorkDir/linux_dependancies/64bit/libsdkencryptedappticket.so");
		copyOrFail($rh_setup_value_by_var->{steam_sdk_path}."/sdk/redistributable_bin/linux32/libsteam_api.so", "$project_name/WorkDir/linux_dependancies/32bit/libsteam_api.so");
		copyOrFail($rh_setup_value_by_var->{steam_sdk_path}."/sdk/redistributable_bin/linux64/libsteam_api.so", "$project_name/WorkDir/linux_dependancies/64bit/libsteam_api.so");
	}
	writeFile("$project_name/WorkDir/app32.sh", "#!/bin/sh\n".'LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./linux_dependancies/32bit ./App_Linux_32bit*');
	writeFile("$project_name/WorkDir/app64.sh", "#!/bin/sh\n".'LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./linux_dependancies/64bit ./App_Linux_64bit*');

	#------
	print "Generating other stuff for App_VS2013_DX_Store\n";
	my $in_st = "./common/Windows/Windows_VS2013_DX_Store";
	my $out_st = "$project_name/App_VS2013_DX_Store";
	copyOrFail("$in_st/Package.appxmanifest", "$out_st/Package.appxmanifest", 0);
	copyr("$in_st/Assets", "$out_st/Assets", 0);
	copyOrFail("$in_st/copy_work_dir_to_appx.bat", "$project_name/copy_work_dir_to_appx.bat", 0);

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
	#unless (scalar glob("App_VS2008_OpenGL.vcproj.*.user") > 0 || -f scalar glob("App_VS2008_OpenGL.vcproj.user") > 0
	print "You may need to manually set up the new Visual Studio Project GUID numbers and Package.appxmanifest identity name for App_VS2013_DX_Store.\n";
	print "\n";

	system('PAUSE') unless ($^O eq 'linux');

	return 0;
}

exit main();

#------------------------

sub readSetupIniFile($)
{
	my $path = shift;
	my $rh_setup_value_by_var = {};
	open (FD, $path) or die $path;
	foreach my $line (<FD>)
	{
		chomp $line;
		unless ($line =~ /^;/ || $line eq '')
		{
			my @l_tabs = split(/=/, $line);
			my $var = shift @l_tabs;
			my $value = join('=', @l_tabs);
			$rh_setup_value_by_var->{$var} = $value;
		}
	}
	return $rh_setup_value_by_var;
}

#------------------------
# create a directory <arg> if it does not exist

sub mkd($) { my $d = shift;unless (-d $d) { mkdir $d or die $d } }

#------------------------
# write a file <first-arg> with the content string <second-arg> (clobber it if it already exists)

sub writeFile($$) { (open FDWTMP, ">".shift()) and (print FDWTMP shift()) and (close FDWTMP) }

#------------------------
# copy all files from <first-arg> into <second-arg> (create <second-arg> if it does not exist)
# if <third-arg> is true, clobber existing files

sub copyr($$$) {
	my ($src, $dest, $clobber) = @_;
	mkd($dest);
	foreach (glob("$src/*")) {
		copyOrFail($_,"$dest/".basename($_), $clobber);
	}
}

#------------------------
# copy file <first-arg> <second-arg> (clobber it if it already exists and <third-arg> is true)

sub copyOrFail($$$) {
	my ($src, $dest, $clobber) = @_;
	if ($clobber || !(-f $dest)) {
		copy($src, $dest) or die "copy from $src to $dest failed";
	}
}

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
