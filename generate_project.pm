#!/usr/bin/perl

package generate_project;

use strict;
use warnings;
use File::Basename qw/dirname basename/;
use Data::Dumper;
use File::Copy qw/copy/;
use Cwd;

#--------------------------------------------------------------------------------------------
# config
#--------------------------------------------------------------------------------------------

my @L_DISABLE_3D_SKIPPED_MODULES = (
	'Animated3DModel','GroupOfQuads','Material','Meshes3D','Obj3D','RenderGroup','Scene3D',
	'Scene3DPrivate', 'VBO', 'VBOGroup', 'GroupOfQuadsByTexture' );
my @L_DISABLE_3D_SKIPPED_CPP = map { "$_.cpp" } @L_DISABLE_3D_SKIPPED_MODULES;
my @L_DISABLE_3D_SKIPPED_H = map { "$_.cpp" } @L_DISABLE_3D_SKIPPED_MODULES;

#--------------------------------------------------------------------------------------------
# public functions
#--------------------------------------------------------------------------------------------

#----------------------------------------------

sub processEmscripten($$)
{
	my ($project_name, $emscripten_dir) = @_;
	mkd("$project_name/$emscripten_dir");
	generate_project::writeFileWithConfirmationForDifferences("$project_name/$emscripten_dir/.gitignore",
		join("\n", qw/output.js output.data output.html output.html.mem compile.bat.new compile.sh.new .gitignore.new/)."\n");
	generate_project::writeFileWithConfirmationForDifferences("$project_name/$emscripten_dir/compile.bat",
		'@echo off'."\n"
		.'perl ../../common/JS_Emscripten/compile.pl ../code/*.cpp'."\n"
		."PAUSE\n");
	generate_project::writeFileWithConfirmationForDifferences("$project_name/$emscripten_dir/compile.sh",
		'#!/bin/sh'."\n\n"
		.'perl ../../common/JS_Emscripten/compile.pl ../code/*.cpp'."\n"
		);
}

#----------------------------------------------

sub processLinuxFiles($$$$)
{
	my ($project_name, $linux_dir, $linux_make_additional_arguments, $rl_steam_sdk_path_or_empty) = @_;
	
	mkd("$project_name/$linux_dir");
	
	generate_project::writeFileWithConfirmationForDifferences("$project_name/$linux_dir/.gitignore", "compile.sh.new\n.gitignore.new\n");
	
	my $steam_sdk_path_or_empty = filterOnlyAndExceptOne($rl_steam_sdk_path_or_empty, $linux_dir, undef);
	my $add_args = (defined $linux_make_additional_arguments?$linux_make_additional_arguments:"");
	$add_args .= " STEAMSDK_PATH='$steam_sdk_path_or_empty' " if ((defined $steam_sdk_path_or_empty) && ($steam_sdk_path_or_empty ne ''));
	writeFileWithConfirmationForDifferences("$project_name/$linux_dir/compile.sh",
		'#!/bin/sh'."\n"
		.'make $* -f ../../common/Linux/Makefile SRCS=\'$(wildcard ../code/*.cpp)\''." $add_args\n"
		);
}

#------------------------

sub processVS($$$$$$$)
{
	my ($name, $relt, $rl_code, $disable_3d, $recent_visual_studio, $use_directx, $rll_args_process) = @_;
	
	mkd("$relt/$name");
	
	my $gitignore_visual_studio = join("\n", qw/Debug Release *.ncb *.suo *.vcproj.*.user *.vcproj.new *.vcxproj.new .gitignore.new/)."\n";
	writeFileWithConfirmationForDifferences("$relt/App_VS2008_OpenGL/.gitignore", $gitignore_visual_studio);

	my $vcproj = ($recent_visual_studio ? 'vcxproj' : 'vcproj');
	processVisualStudioProjectFile_("$name.$vcproj.src", "$relt/$name/$name.$vcproj", $rl_code, $disable_3d, $recent_visual_studio, $use_directx, $rll_args_process);

	processVisualStudioProjectUserFile_("$name.$vcproj.user.src", "$relt/$name/$name.$vcproj.user");

	my $rl_visual_studio_app_guids = $$rll_args_process[5];
	processVisualStudioSolutionFile_("$name.sln", "$relt/$name/$name.sln", $rl_visual_studio_app_guids);
}

#------------------------

sub processOtherStuffVSStore($$)
{
	my ($project_name, $output_dir) = @_;
	my $in_st = "./common/Windows/Windows_VS2013_DX_Store";
	my $out_st = "$project_name/$output_dir";
	copyOrFail("$in_st/Package.appxmanifest", "$out_st/Package.appxmanifest", 0);
	copyr("$in_st/Assets", "$out_st/Assets", 0);
	copyOrFail("$in_st/copy_work_dir_to_appx.bat", "$project_name/copy_work_dir_to_appx.bat", 0);
}

#------------------------

sub processWorkDir($$$)
{
	my ($project_name, $workdir, $steam_sdk_path) = @_;
	
	mkd("$project_name/$workdir");
	generate_project::writeFileWithConfirmationForDifferences("$project_name/$workdir/.gitignore",
		join("\n", qw/old *.dll *.zip *.exe steam_appid.txt App_Linux_*bit app32.sh app64.sh/)."\n");
	mkd("$project_name/$workdir/linux_dependancies");
	mkd("$project_name/$workdir/linux_dependancies/32bit");
	mkd("$project_name/$workdir/linux_dependancies/64bit");
	
	if (defined $steam_sdk_path && $steam_sdk_path ne '')
	{
		copyOrFail($steam_sdk_path."/sdk/public/steam/lib/win32/sdkencryptedappticket.dll", "$project_name/$workdir/sdkencryptedappticket.dll");
		copyOrFail($steam_sdk_path."/sdk/public/steam/lib/win64/sdkencryptedappticket64.dll", "$project_name/$workdir/sdkencryptedappticket64.dll");
		copyOrFail($steam_sdk_path."/sdk/redistributable_bin/steam_api.dll", "$project_name/$workdir/steam_api.dll");
		copyOrFail($steam_sdk_path."/sdk/redistributable_bin/win64/steam_api64.dll", "$project_name/$workdir/steam_api64.dll");
		copyOrFail($steam_sdk_path."/sdk/public/steam/lib/linux32/libsdkencryptedappticket.so",
			"$project_name/$workdir/linux_dependancies/32bit/libsdkencryptedappticket.so");
		copyOrFail($steam_sdk_path."/sdk/public/steam/lib/linux64/libsdkencryptedappticket.so",
			"$project_name/$workdir/linux_dependancies/64bit/libsdkencryptedappticket.so");
		copyOrFail($steam_sdk_path."/sdk/redistributable_bin/linux32/libsteam_api.so", "$project_name/$workdir/linux_dependancies/32bit/libsteam_api.so");
		copyOrFail($steam_sdk_path."/sdk/redistributable_bin/linux64/libsteam_api.so", "$project_name/$workdir/linux_dependancies/64bit/libsteam_api.so");
	}
	writeFile("$project_name/$workdir/app32.sh", "#!/bin/sh\n".'LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./linux_dependancies/32bit ./App_Linux_32bit*');
	writeFile("$project_name/$workdir/app64.sh", "#!/bin/sh\n".'LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./linux_dependancies/64bit ./App_Linux_64bit*');

	mkd("$project_name/$workdir/data");
	unless (-f "$project_name/$workdir/data/default_font.png")
	{
		copy("common/default_font.png", "$project_name/$workdir/data/default_font.png");
	}
	foreach my $file (glob("dependancy_libraries/dll/*"))
	{
		copy($file, "$project_name/$workdir/".basename($file));
	}
	mkd("$project_name/$workdir/shaders");
	copy("./shaders/fragmentshader.glsl", "$project_name/$workdir/shaders/fragmentshader.glsl");
	copy("./shaders/vertexshader.glsl", "$project_name/$workdir/shaders/vertexshader.glsl");
	mkd("$project_name/WorkDirStore");
	mkd("$project_name/WorkDirStore/AppX");
	mkd("$project_name/WorkDirStore/AppX/data");

}


#--------------------------------------------------------------------------------------------
# private functions
#--------------------------------------------------------------------------------------------

sub processVisualStudioSolutionFile_($$$)
{
	my ($input_file, $output_file, $rl_visual_studio_app_guids) = @_;
	
	my $output_dir = basename(dirname($output_file));

	my @l_lines = readFile($input_file);
	my $visual_studio_app_guid = filterOnlyAndExceptOne($rl_visual_studio_app_guids, $output_dir, undef);
	$visual_studio_app_guid = '90347322-85ff-4500-95e8-aafdc8cfa1bf' if ($visual_studio_app_guid eq ''); # note: you can use guidgen.exe
	foreach (@l_lines)
	{
		if ($_ =~ m/^(.*Project\(\"\{[A-Za-z0-9\-]+\}\"\) = \".+\", \".+\", \"\{)[A-Za-z0-9\-]+(\}\".*)$/ || $_ =~ m/^(.*\{)[A-Za-z0-9\-]+(\}\..*)$/)
		{
			$_ = $1.$visual_studio_app_guid.$2."\n";
		}
	}
	writeFileWithConfirmationForDifferences($output_file, join('', @l_lines));
}

#----------------------------------------------

sub processVisualStudioProjectUserFile_($$)
{
	my ($input_file, $output_file) = @_;

	open FD, $input_file or die $input_file;
	my @lines = <FD>;
	close FD;

	foreach my $line (@lines)
	{
		if ($line =~ m/^(.*)AUTOMATIC_HOST(.*)$/)
		{
			my $host = uc `hostname`;
			chomp $host;
			$line = "$1$host$2";
		}
	}

	writeFileWithConfirmationForDifferences($output_file, join('', @lines));
}

#----------------------------------------------

sub processVisualStudioProjectFile_($$$$$$$)
{
	my ($input_file, $output_file, $rl_high_level_src, $disable_3d, $recent_visual_studio, $use_directx, $rl_args_process) = @_;
	my ($rl_steam_sdk_path_or_empty,
		$rl_additional_include_dirs,
		$rl_additional_lib_dirs,
		$rl_additional_libs,
		$rl_additional_defines,
		$rl_visual_studio_app_guids,
		$rl_steam_sdk_path_or_empty) = @$rl_args_process;
	die unless (defined $input_file && defined $output_file && defined $rl_high_level_src);
	my $output_dir = basename(dirname($output_file));

	my @l_additional_include_dirs_32 = filterOnlyAndExcept($rl_additional_include_dirs, $output_dir, "32");
	my @l_additional_lib_dirs_32 = filterOnlyAndExcept($rl_additional_lib_dirs, $output_dir, "32");
	my @l_additional_libs_32 = filterOnlyAndExcept($rl_additional_libs, $output_dir, "32");
	my @l_additional_defines_32 = filterOnlyAndExcept($rl_additional_defines, $output_dir, "32");

	my @l_additional_include_dirs_64 = filterOnlyAndExcept($rl_additional_include_dirs, $output_dir, "64");
	my @l_additional_lib_dirs_64 = filterOnlyAndExcept($rl_additional_lib_dirs, $output_dir, "64");
	my @l_additional_libs_64 = filterOnlyAndExcept($rl_additional_libs, $output_dir, "64");
	my @l_additional_defines_64 = filterOnlyAndExcept($rl_additional_defines, $output_dir, "64");

	my @l_additional_include_dirs_arm = filterOnlyAndExcept($rl_additional_include_dirs, $output_dir, "ARM");
	my @l_additional_lib_dirs_arm = filterOnlyAndExcept($rl_additional_lib_dirs, $output_dir, "ARM");
	my @l_additional_libs_arm = filterOnlyAndExcept($rl_additional_libs, $output_dir, "ARM");
	my @l_additional_defines_arm = filterOnlyAndExcept($rl_additional_defines, $output_dir, "ARM");
	
	my $visual_studio_app_guid = filterOnlyAndExceptOne($rl_visual_studio_app_guids, $output_dir, undef);
	my $steam_sdk_path_or_empty = filterOnlyAndExceptOne($rl_steam_sdk_path_or_empty, $output_dir, undef);
 
	if ($steam_sdk_path_or_empty ne '')
	{
		push @l_additional_include_dirs_32, "$steam_sdk_path_or_empty\\sdk\\public";
		push @l_additional_include_dirs_64, "$steam_sdk_path_or_empty\\sdk\\public\\steam\\lib\\win64";
		push @l_additional_lib_dirs_32, "$steam_sdk_path_or_empty\\sdk\\redistributable_bin";
		push @l_additional_lib_dirs_64, "$steam_sdk_path_or_empty\\sdk\\redistributable_bin";
		push @l_additional_lib_dirs_32, "$steam_sdk_path_or_empty\\sdk\\public\\steam\\lib\\win32";
		push @l_additional_lib_dirs_64, "$steam_sdk_path_or_empty\\sdk\\public\\steam\\lib\\win64";
		#push @l_additional_libs_32, "sdkencryptedappticket.lib";# already in Steam.cpp
		#push @l_additional_libs_64, "sdkencryptedappticket64.lib";# already in Steam.cpp

		push @l_additional_defines_32, 'USES_STEAM_INTEGRATION';
		push @l_additional_defines_64, 'USES_STEAM_INTEGRATION';
		push @l_additional_defines_arm, 'USES_STEAM_INTEGRATION';
	}

	#print "# Process file: $input_file\n";
	#print "# Output file is: $output_file\n";
	my @lines = readFile($input_file);

	my $arch = "32";
	foreach my $line (@lines)
	{
		if ($line =~ m/\'(Release|Debug)\|Win32\'/)
		{
			$arch = "32";
		}
		elsif ($line =~ m/\'(Release|Debug)\|x64\'/)
		{
			$arch = "64";
		}
		elsif ($line =~ m/\'(Release|Debug)\|ARM\'/)
		{
			$arch = "ARM";
		}

		if ($line =~ m/<File RelativePath="[^\"]+"><\/File>/)
		{
		}
		elsif ($line =~ m/AUTOMATIC_INCLUDE_PATHS/)
		{
			my $str = '';
			foreach my $file (glob("../../include/*"))
			{
				$str .= getStrToInclude($file, $recent_visual_studio);
			}
			$line = $str;
		}
		elsif ($line =~ m/AUTOMATIC_SOURCE_PATHS/)
		{
			my $str = '';
			foreach my $file (sort glob("../../source/*"))
			{
				unless ((basename($file) eq 'directx') or (basename($file) eq 'opengl'))
				{
					if ($disable_3d)
					{
						next if (grep { basename($file) eq $_ } (@L_DISABLE_3D_SKIPPED_CPP,@L_DISABLE_3D_SKIPPED_H));
					}
					$str .= getStrToInclude($file, $recent_visual_studio);
				}
			}
			foreach my $file2 (sort ($use_directx ? glob("../../source/directx/*") : glob("../../source/opengl/*")))
			{
				if ($disable_3d)
				{
					next if (grep { basename($file2) eq $_ } (@L_DISABLE_3D_SKIPPED_CPP,@L_DISABLE_3D_SKIPPED_H));
				}
				$str .= getStrToInclude($file2, $recent_visual_studio);
			}
			$line = $str;
		}
		elsif ($line =~ m/AUTOMATIC_HIGH_LEVEL_CODE/)
		{
			my $str = '';
			foreach my $file (@$rl_high_level_src)
			{
				$str .= getStrToInclude($file, $recent_visual_studio);
			}
			$line = $str;
		}
		elsif ($line =~ m/^(.*)AUTOMATIC_STORE_GUID(.*)$/)
		{
			$visual_studio_app_guid = '90347322-85ff-4500-95e8-aafdc8cfa1bf' if ($visual_studio_app_guid eq '');
			$line = $1.$visual_studio_app_guid.$2."\n";
		}
		elsif ((($line =~ m/^(.*<ProjectGuid>\{)[A-Z0-9\-]+(\}<\/ProjectGuid>.*)$/) || ($line =~ /^(.*ProjectGUID=\"\{)[A-Z0-9\-]+(\}\".*)$/)) && ($visual_studio_app_guid ne ''))
		{
			$line = $1.$visual_studio_app_guid.$2."\n";
		}
		if ($recent_visual_studio)
		{
			# if ((defined $steam_sdk_path_or_empty) && ($steam_sdk_path_or_empty ne '') && $line =~ m/^(.*)<PreprocessorDefinitions>([^<]+)<\/PreprocessorDefinitions>(.*)/)
			# {
				# $line = "$1<PreprocessorDefinitions>$2;USES_STEAM_INTEGRATION;<\/PreprocessorDefinitions>$3\n";
			# }
			if (($arch eq '32') && @l_additional_include_dirs_32 > 0 && $line =~ m/^(.*)<AdditionalIncludeDirectories>([^<]+)<\/AdditionalIncludeDirectories>(.*)/)
			{
				$line = "$1<AdditionalIncludeDirectories>$2;".join(';',@l_additional_include_dirs_32).";<\/AdditionalIncludeDirectories>$3\n";
			}
			elsif (($arch eq '32') && @l_additional_lib_dirs_32 > 0 && $line =~ m/^(.*)<AdditionalLibraryDirectories>([^<]+)<\/AdditionalLibraryDirectories>(.*)/)
			{
				$line = "$1<AdditionalLibraryDirectories>$2;".join(';',@l_additional_lib_dirs_32).";<\/AdditionalLibraryDirectories>$3\n";
			}
			elsif (($arch eq '32') && @l_additional_libs_32 > 0 && $line =~ m/^(.*)<AdditionalDependencies>([^<]+)<\/AdditionalDependencies>(.*)/)
			{
				$line = "$1<AdditionalDependencies>$2;".join(';',@l_additional_libs_32).";<\/AdditionalDependencies>$3\n";
			}
			elsif (($arch eq '32') && @l_additional_defines_32 > 0 && $line =~ m/^(.*)<PreprocessorDefinitions>([^<]+)<\/PreprocessorDefinitions>(.*)/)
			{
				$line = "$1<PreprocessorDefinitions>$2;".join(';',@l_additional_defines_32).";<\/PreprocessorDefinitions>$3\n";
			}
			elsif (($arch eq '64') && @l_additional_include_dirs_64 > 0 && $line =~ m/^(.*)<AdditionalIncludeDirectories>([^<]+)<\/AdditionalIncludeDirectories>(.*)/)
			{
				$line = "$1<AdditionalIncludeDirectories>$2;".join(';',@l_additional_include_dirs_64).";<\/AdditionalIncludeDirectories>$3\n";
			}
			elsif (($arch eq '64') && @l_additional_lib_dirs_64 > 0 && $line =~ m/^(.*)<AdditionalLibraryDirectories>([^<]+)<\/AdditionalLibraryDirectories>(.*)/)
			{
				$line = "$1<AdditionalLibraryDirectories>$2;".join(';',@l_additional_lib_dirs_64).";<\/AdditionalLibraryDirectories>$3\n";
			}
			elsif (($arch eq '64') && @l_additional_libs_64 > 0 && $line =~ m/^(.*)<AdditionalDependencies>([^<]+)<\/AdditionalDependencies>(.*)/)
			{
				$line = "$1<AdditionalDependencies>$2;".join(';',@l_additional_libs_64).";<\/AdditionalDependencies>$3\n";
			}
			elsif (($arch eq '64') && @l_additional_defines_64 > 0 && $line =~ m/^(.*)<PreprocessorDefinitions>([^<]+)<\/PreprocessorDefinitions>(.*)/)
			{
				$line = "$1<PreprocessorDefinitions>$2;".join(';',@l_additional_defines_64).";<\/PreprocessorDefinitions>$3\n";
			}
			elsif (($arch eq 'ARM') && @l_additional_include_dirs_arm > 0 && $line =~ m/^(.*)<AdditionalIncludeDirectories>([^<]+)<\/AdditionalIncludeDirectories>(.*)/)
			{
				$line = "$1<AdditionalIncludeDirectories>$2;".join(';',@l_additional_include_dirs_arm).";<\/AdditionalIncludeDirectories>$3\n";
			}
			elsif (($arch eq 'ARM') && @l_additional_lib_dirs_arm > 0 && $line =~ m/^(.*)<AdditionalLibraryDirectories>([^<]+)<\/AdditionalLibraryDirectories>(.*)/)
			{
				$line = "$1<AdditionalLibraryDirectories>$2;".join(';',@l_additional_lib_dirs_arm).";<\/AdditionalLibraryDirectories>$3\n";
			}
			elsif (($arch eq 'ARM') && @l_additional_libs_arm > 0 && $line =~ m/^(.*)<AdditionalDependencies>([^<]+)<\/AdditionalDependencies>(.*)/)
			{
				$line = "$1<AdditionalDependencies>$2;".join(';',@l_additional_libs_arm).";<\/AdditionalDependencies>$3\n";
			}
			elsif (($arch eq 'ARM') && @l_additional_defines_arm > 0 && $line =~ m/^(.*)<PreprocessorDefinitions>([^<]+)<\/PreprocessorDefinitions>(.*)/)
			{
				$line = "$1<PreprocessorDefinitions>$2;".join(';',@l_additional_defines_arm).";<\/PreprocessorDefinitions>$3\n";
			}
		}
		else
		{
			# if ((defined $steam_sdk_path_or_empty) && ($steam_sdk_path_or_empty ne '') && $line =~ m/^(.*)PreprocessorDefinitions=\"([^"]*)\"(.*)$/)
			# {
				# $line = $1.'PreprocessorDefinitions="'.$2.';USES_STEAM_INTEGRATION;"'.$3."\n";
			# }
			if (@l_additional_include_dirs_32 > 0 && $line =~ m/^(.*)AdditionalIncludeDirectories=\"([^"]*)\"(.*)$/)
			{
				$line = $1.'AdditionalIncludeDirectories="'.$2.';'.join(';',@l_additional_include_dirs_32).'"'.$3."\n";
			}
			elsif (@l_additional_lib_dirs_32 > 0 && $line =~ m/^(.*)AdditionalLibraryDirectories=\"([^"]*)\"(.*)$/)
			{
				$line = $1.'AdditionalLibraryDirectories="'.$2.';'.join(';',@l_additional_lib_dirs_32).'"'.$3."\n";
			}
			elsif (@l_additional_libs_32 > 0 && $line =~ m/^(.*)AdditionalDependencies=\"([^"]*)\"(.*)$/)
			{
				$line = $1.'AdditionalDependencies="'.$2.' '.join(' ',@l_additional_libs_32).'"'.$3."\n";
			}
			elsif (@l_additional_defines_32 > 0 && $line =~ m/^(.*)PreprocessorDefinitions=\"([^"]*)\"(.*)$/)
			{
				$line = $1.'PreprocessorDefinitions="'.$2.';'.join(';',@l_additional_defines_32).'"'.$3."\n";
			}
		}
	}

	writeFileWithConfirmationForDifferences($output_file, join('', @lines));
}

#------------------------

sub filterOnlyAndExceptOne($$$)
{
	my ($rl_visual_studio_app_guids, $output_dir, $str_arch) = @_;
	my @l_visual_studio_app_guids = filterOnlyAndExcept($rl_visual_studio_app_guids, $output_dir, $str_arch);
	die scalar(@l_visual_studio_app_guids) unless (@l_visual_studio_app_guids <= 1);
	return (scalar(@l_visual_studio_app_guids)==0 ? '' : shift @l_visual_studio_app_guids);
}

#------------------------

sub filterOnlyAndExcept($$$)
{
	my ($rl_values, $output_dir, $str_arch) = @_;
	my @l_values;
	foreach (@$rl_values)
	{
		if ( !($_ =~ m/^ONLY\:.+\:.+$/) && !($_ =~ m/^EXCEPT\:.+\:.+$/)
			&& !($_ =~ m/^ONLY\:.+\:.+\:.+$/) && !($_ =~ m/^EXCEPT\:.+\:.+\:.+$/)
			&& !($_ =~ m/^ONLY_ARCH\:.+\:.+$/) && !($_ =~ m/^EXCEPT_ARCH\:.+\:.+$/))
		{
			push @l_values, $_;
		}
		elsif (($_ =~ m/^ONLY\:.+\:.+$/) && ($_ =~ m/^ONLY\:$output_dir\:(.+)$/))
		{
			push @l_values, $1;
		}
		elsif (defined $str_arch && ($_ =~ m/^ONLY\:.+\:.+\:.+$/) && ($_ =~ m/^ONLY\:$output_dir\:$str_arch\:(.+)$/))
		{
			push @l_values, $1;
		}
		elsif (defined $str_arch && ($_ =~ m/^ONLY_ARCH\:.+\:.+$/) && ($_ =~ m/^ONLY_ARCH\:$str_arch\:(.+)$/))
		{
			push @l_values, $1;
		}
		elsif (!($_ =~ m/^EXCEPT\:$output_dir\:.+$/) && ($_ =~ m/^EXCEPT\:.+\:(.+)$/))
		{
			push @l_values, $1;
		}
		elsif (defined $str_arch && !($_ =~ m/^EXCEPT\:$output_dir\:$str_arch\:.+$/) && ($_ =~ m/^EXCEPT\:.+\:.+\:(.+)$/))
		{
			push @l_values, $1;
		}
		elsif (defined $str_arch && !($_ =~ m/^EXCEPT_ARCH\:$str_arch\:.+$/) && ($_ =~ m/^EXCEPT_ARCH\:.+\:(.+)$/))
		{
			push @l_values, $1;
		}
	}
	return @l_values;
}

#------------------------

sub writeFileWithConfirmationForDifferences($$)
{
	my $output_file = shift;
	my $content = shift;

	if (-f $output_file)
	{
		writeFile("$output_file.new", $content);
		my $diff_output = diffCrossPlaftorm("$output_file.new", $output_file);
		if (defined $diff_output)
		{
			##system("D:\\progs\\WinMerge\\winmergeU.exe '$output_file' '$output_file.new'");
			##`color 9`;
			#print "$diff_output\n";
			print "................................................................\n";
			print "..... $_\n" foreach (split(/\n/, $diff_output));
			print "................................................................\n";
			##`color 7`;
			print "\nFile '$output_file' changed, see above the differences:\n";
			print "Confirm overwriting (y/n): ";
			my $yes_no = <>;# wait for user input
			chomp $yes_no;
			print "\n";
			rename "$output_file.new", $output_file if ($yes_no eq "y");
		}
		else
		{
			unlink "$output_file.new";
		}
	}
	else
	{
		writeFile($output_file, $content);
	}
}

#------------------------

sub diffCrossPlaftorm($)
{
	my ($file1, $file2) = @_;
	if ($^O eq 'MSWin32')
	{
		my $file1w = $file1;
		$file1w =~ s/\//\\/g;
		my $file2w = $file2;
		$file2w =~ s/\//\\/g;
		my $diff_output = `fc $file1w $file2w`;
		return undef if ($diff_output =~ m/FC: no differences encountered/);
		return $diff_output;
	}
	else
	{
		my $diff_output = `diff '$file1' '$file2'`;
		chomp $diff_output;
		return undef if ($diff_output eq '');
		return $diff_output;
	}
}

#------------------------
# read a file, get its contents

sub readFile($)
{
	my $filepath = shift;
	open (FDRTMP, $filepath) or die("could not read $filepath (current dir = ".getcwd().")");
	my @l_lines = <FDRTMP>;
	close FDRTMP;
	return @l_lines;
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

sub getStrToInclude($$)
{
	my ($file, $recent_visual_studio) = @_;
	$file =~ s/\//\\/g;
	if ($recent_visual_studio)
	{
		my $compile_or_include = (isSourceFile($file) ? 'ClCompile' : 'ClInclude');
		return '<'.$compile_or_include.' Include="'.$file.'" />'."\n";
	}
	else
	{
		return '<File RelativePath="'.$file.'"></File>'."\n";
	}
}

#------------------------

sub isSourceFile() { my $file = shift; return ($file=~m/\.cpp$/ || $file=~m/\.c$/ || $file=~m/\.cc$/ ? 1 : 0); }

#--------------------------------------------------------------------------------------------
#--------------------------------------------------------------------------------------------

1;
