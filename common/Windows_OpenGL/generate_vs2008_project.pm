#!/usr/bin/perl

package generate_vs2008_project;

use strict;
use warnings;
use File::Basename qw/dirname basename/;

my @L_DISABLE_3D_SKIPPED_MODULES = (
	'Animated3DModel','GroupOfQuads','Material','Meshes3D','Obj3D','RenderGroup','Scene3D',
	'Scene3DPrivate', 'VBO', 'VBOGroup', 'GroupOfQuadsByTexture' );
my @L_DISABLE_3D_SKIPPED_CPP = map { "$_.cpp" } @L_DISABLE_3D_SKIPPED_MODULES;
my @L_DISABLE_3D_SKIPPED_H = map { "$_.cpp" } @L_DISABLE_3D_SKIPPED_MODULES;

#----------------------------------------------

sub createSolutionFileVS2008($$) {
	my ($output_directory, $project_basename) = @_;
	open FDW, ">$output_directory/$project_basename.sln";
	print FDW 'Microsoft Visual Studio Solution File, Format Version 10.00
# Visual C++ Express 2008
Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "'.$project_basename.'", "'.$project_basename.'.vcproj", "{609CC8B0-B8BA-4DE1-9EB3-25C4925225E2}"
EndProject
Global
	GlobalSection(SolutionConfigurationPlatforms) = preSolution
		Debug|Win32 = Debug|Win32
		Release|Win32 = Release|Win32
	EndGlobalSection
	GlobalSection(ProjectConfigurationPlatforms) = postSolution
		{609CC8B0-B8BA-4DE1-9EB3-25C4925225E2}.Debug|Win32.ActiveCfg = Debug|Win32
		{609CC8B0-B8BA-4DE1-9EB3-25C4925225E2}.Debug|Win32.Build.0 = Debug|Win32
		{609CC8B0-B8BA-4DE1-9EB3-25C4925225E2}.Release|Win32.ActiveCfg = Release|Win32
		{609CC8B0-B8BA-4DE1-9EB3-25C4925225E2}.Release|Win32.Build.0 = Release|Win32
	EndGlobalSection
	GlobalSection(SolutionProperties) = preSolution
		HideSolutionNode = FALSE
	EndGlobalSection
EndGlobal
';
	close FDW;
}

#----------------------------------------------

sub processFile($$$$)
{
	my ($input, $output, $rl_high_level_src, $disable_3d) = @_;
	die unless (defined $input && defined $output && defined $rl_high_level_src);
	
	print "# Process file: $input\n";
	print "# Output file is: $output\n";
	open FD, $input or die $input;
	my @lines = <FD>;
	close FD;

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
					if ($disable_3d)
					{
						if (grep { basename($file) eq $_ } (@L_DISABLE_3D_SKIPPED_CPP,@L_DISABLE_3D_SKIPPED_H))
						{
							next;
						}
					}
					$file =~ s/\//\\/g;
					$str .= '<File RelativePath="'.$file.'"></File>'."\n";
				}
			}
			foreach my $file2 (glob("../../source/opengl/*"))
			{
				if ($disable_3d)
				{
					if (grep { basename($file2) eq $_ } (@L_DISABLE_3D_SKIPPED_CPP,@L_DISABLE_3D_SKIPPED_H))
					{
						next;
					}
				}
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
	
	open FDW, ">$output" or die $output;
	print FDW $_ foreach (@lines);
	close FDW;
}

1;
