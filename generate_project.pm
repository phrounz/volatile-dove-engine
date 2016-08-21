#!/usr/bin/perl

package generate_project;

use strict;
use warnings;
use File::Basename qw/dirname basename/;
use Data::Dumper;

#----------------------------------------------

my @L_DISABLE_3D_SKIPPED_MODULES = (
	'Animated3DModel','GroupOfQuads','Material','Meshes3D','Obj3D','RenderGroup','Scene3D',
	'Scene3DPrivate', 'VBO', 'VBOGroup', 'GroupOfQuadsByTexture' );
my @L_DISABLE_3D_SKIPPED_CPP = map { "$_.cpp" } @L_DISABLE_3D_SKIPPED_MODULES;
my @L_DISABLE_3D_SKIPPED_H = map { "$_.cpp" } @L_DISABLE_3D_SKIPPED_MODULES;

#----------------------------------------------

sub processVisualStudioProjectUserFile($$)
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

sub processVisualStudioProjectFile($$$$$$$$$$$$)
{
	my ($input_file, $output_file, $rl_high_level_src, $disable_3d, $recent_visual_studio, $use_directx, $steam_sdk_path_or_empty,
		$rl_additional_include_dirs, $rl_additional_lib_dirs, $rl_additional_libs, $rl_additional_defines, $rl_visual_studio_app_guids) = @_;
	die unless (defined $input_file && defined $output_file && defined $rl_high_level_src);
	my $output_dir = basename(dirname($output_file));

	my @l_visual_studio_app_guids = filterOnlyAndExcept($rl_visual_studio_app_guids, $output_dir, undef);
	die scalar(@l_visual_studio_app_guids) unless (@l_visual_studio_app_guids <= 1);
	my $visual_studio_app_guid = (scalar(@l_visual_studio_app_guids)==0 ? '' : shift @l_visual_studio_app_guids);

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
	open FD, $input_file or die $input_file;
	my @lines = <FD>;
	close FD;

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
		elsif ((($line =~ m/<ProjectGuid>\{([A-Z0-9\-]+)\}<\/ProjectGuid>$/) || ($line =~ /ProjectGUID=\"\{([A-Z0-9\-]+)\}\"/)) && ($visual_studio_app_guid ne ''))
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
			{die;
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
# write a file <first-arg> with the content string <second-arg> (clobber it if it already exists)

sub writeFile($$) { (open FDWTMP, ">".shift()) and (print FDWTMP shift()) and (close FDWTMP) }

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

#------------------------

1;
