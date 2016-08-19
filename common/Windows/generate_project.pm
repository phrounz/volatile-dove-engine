#!/usr/bin/perl

package generate_project;

use strict;
use warnings;
use File::Basename qw/dirname basename/;

#----------------------------------------------

my @L_DISABLE_3D_SKIPPED_MODULES = (
	'Animated3DModel','GroupOfQuads','Material','Meshes3D','Obj3D','RenderGroup','Scene3D',
	'Scene3DPrivate', 'VBO', 'VBOGroup', 'GroupOfQuadsByTexture' );
my @L_DISABLE_3D_SKIPPED_CPP = map { "$_.cpp" } @L_DISABLE_3D_SKIPPED_MODULES;
my @L_DISABLE_3D_SKIPPED_H = map { "$_.cpp" } @L_DISABLE_3D_SKIPPED_MODULES;

#----------------------------------------------

sub processFile($$$$$$$$$$)
{
	my ($input_file, $output_file, $rl_high_level_src, $disable_3d, $recent_visual_studio, $use_directx, $steam_sdk_path_or_empty,
		$rl_additional_include_dirs, $rl_additional_lib_dirs, $rl_additional_libs) = @_;
	die unless (defined $input_file && defined $output_file && defined $rl_high_level_src);
	
	#print "# Process file: $input_file\n";
	#print "# Output file is: $output_file\n";
	open FD, $input_file or die $input_file;
	my @lines = <FD>;
	close FD;

	foreach my $line (@lines)
	{
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
						next if (grep { basename($file) eq $line } (@L_DISABLE_3D_SKIPPED_CPP,@L_DISABLE_3D_SKIPPED_H));
					}
					$str .= getStrToInclude($file, $recent_visual_studio);
				}
			}
			foreach my $file2 (sort ($use_directx ? glob("../../source/directx/*") : glob("../../source/opengl/*")))
			{
				if ($disable_3d)
				{
					next if (grep { basename($file2) eq $line } (@L_DISABLE_3D_SKIPPED_CPP,@L_DISABLE_3D_SKIPPED_H));
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
		if ($recent_visual_studio)
		{
			if ((defined $steam_sdk_path_or_empty) && ($steam_sdk_path_or_empty ne '') && $line =~ m/^(.*)<PreprocessorDefinitions>([^<]+)<\/PreprocessorDefinitions>(.*)/)
			{
				$line = "$1<PreprocessorDefinitions>$2;USES_STEAM_INTEGRATION;<\/PreprocessorDefinitions>$3\n";
			}
			elsif (@$rl_additional_include_dirs > 0 && $line =~ m/^(.*)<AdditionalIncludeDirectories>([^<]+)<\/AdditionalLibraryDirectories>(.*)/)
			{
				$line = "$1<AdditionalIncludeDirectories>$2;".join(';',@$rl_additional_include_dirs).";<\/AdditionalIncludeDirectories>$3\n";
			}
			elsif (@$rl_additional_lib_dirs > 0 && $line =~ m/^(.*)<AdditionalLibraryDirectories>([^<]+)<\/AdditionalLibraryDirectories>(.*)/)
			{
				$line = "$1<AdditionalLibraryDirectories>$2;".join(';',@$rl_additional_lib_dirs).";<\/AdditionalLibraryDirectories>$3\n";
			}
			elsif (@$rl_additional_libs > 0 && $line =~ m/^(.*)<AdditionalDependencies>([^<]+)<\/AdditionalDependencies>(.*)/)
			{
				$line = "$1<AdditionalDependencies>$2;".join(';',@$rl_additional_libs).";<\/AdditionalDependencies>$3\n";
			}
		}
		else
		{
			if ((defined $steam_sdk_path_or_empty) && ($steam_sdk_path_or_empty ne '') && $line =~ m/^(.*)PreprocessorDefinitions=\"([^"]*)\"(.*)$/)
			{
				$line = $1.'PreprocessorDefinitions="'.$2.';USES_STEAM_INTEGRATION;"'.$3."\n";
			}
			elsif (@$rl_additional_include_dirs > 0 && $line =~ m/^(.*)AdditionalIncludeDirectories=\"([^"]*)\"(.*)$/)
			{
				$line = $1.'AdditionalIncludeDirectories="'.$2.' '.join(';',@$rl_additional_include_dirs).'"'.$3."\n";
			}
			elsif (@$rl_additional_lib_dirs > 0 && $line =~ m/^(.*)AdditionalLibraryDirectories=\"([^"]*)\"(.*)$/)
			{
				$line = $1.'AdditionalLibraryDirectories="'.$2.' '.join(';',@$rl_additional_lib_dirs).'"'.$3."\n";
			}
			elsif (@$rl_additional_libs > 0 && $line =~ m/^(.*)AdditionalDependencies=\"([^"]*)\"(.*)$/)
			{
				$line = $1.'AdditionalDependencies="'.$2.' '.join(' ',@$rl_additional_libs).'"'.$3."\n";
			}
		}
	}

	writeFileWithConfirmationForDifferences($output_file, join('', @lines));
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
			print "File '$output_file' changed, see below the differences:\n";
			##`color 9`;
			#print "$diff_output\n";
			print "................................................................\n";
			print "..... $_\n" foreach (split(/\n/, $diff_output));
			print "................................................................\n";
			##`color 7`;
			print "Confirm overwriting (y/n): ";
			my $yes_no = <>;# wait for user input
			chomp $yes_no;
			print "\n";
			rename "$output_file.new", $output_file if ($yes_no eq "y");
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
