#!/usr/bin/perl

use strict;
use warnings;
use File::Basename qw/basename/;

sub processFile($$$)
{
	my ($input, $output, $rl_high_level_src) = @_;
	print "Process file $input into $output\n";
	open FD, $input;
	my @lines = <FD>;

	foreach (@lines)
	{
		if ($_ =~ m/AUTOMATIC_INCLUDE_PATHS/)
		{
			my $str = '';
			foreach my $file (glob("../../include/*"))
			{
				$file =~ s/\//\\/g;
				$str .= '<File RelativePath="'.$file.'"></File>'."\n";
			}
			$_ = $str;
		}
		elsif ($_ =~ m/AUTOMATIC_SOURCE_PATHS/)
		{
			my $str = '';
			foreach my $file (glob("../../source/*"))
			{
				unless ((basename($file) eq 'directx') or (basename($file) eq 'opengl'))
				{
					$file =~ s/\//\\/g;
					$str .= '<File RelativePath="'.$file.'"></File>'."\n";
				}
			}
			foreach my $file2 (glob("../../source/opengl/*"))
			{
				$file2 =~ s/\//\\/g;
				$str .= '<File RelativePath="'.$file2.'"></File>'."\n";
			}
			$_ = $str;
		}
		elsif ($_ =~ m/AUTOMATIC_HIGH_LEVEL_CODE/)
		{
			my $str = '';
			foreach my $file (@$rl_high_level_src)
			{
				$file =~ s/\//\\/g;
				$str .= '<File RelativePath="'.$file.'"></File>'."\n";
			}
			$_ = $str;
		}
	}

	open FDW, ">$output";
	print FDW $_ foreach (@lines);
	close FDW;
}

processFile("App_VS2008_SDL.vcproj.src", "App_VS2008_SDL.vcproj", [ "../MainClass.cpp"]);
processFile("App_VS2008_OpenGL.vcproj.src", "App_VS2008_OpenGL.vcproj", [ "../MainClass.cpp" ]);



# <FileConfiguration
# Name="Debug|Win32"
# >
# <Tool
	# Name="VCCLCompilerTool"
	# AdditionalIncludeDirectories="C:\progs\GnuWin32\include;&quot;d:\progs\OpenAL 1.1 SDK\include&quot;;"
# />
# </FileConfiguration>
# <FileConfiguration
# Name="Release|Win32"
# >
# <Tool
	# Name="VCCLCompilerTool"
	# AdditionalIncludeDirectories="C:\progs\GnuWin32\include;&quot;d:\progs\OpenAL 1.1 SDK\include&quot;;"
# />
