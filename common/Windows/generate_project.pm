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

sub processFile($$$$$$)
{
	my ($input, $output, $rl_high_level_src, $disable_3d, $recent_visual_studio, $use_directx) = @_;
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
				$str .= getStrToInclude($file, $recent_visual_studio);
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
						next if (grep { basename($file) eq $_ } (@L_DISABLE_3D_SKIPPED_CPP,@L_DISABLE_3D_SKIPPED_H));
					}
					$str .= getStrToInclude($file, $recent_visual_studio);
				}
			}
			foreach my $file2 ($use_directx ? glob("../../source/directx/*") : glob("../../source/opengl/*"))
			{
				if ($disable_3d)
				{
					next if (grep { basename($file2) eq $_ } (@L_DISABLE_3D_SKIPPED_CPP,@L_DISABLE_3D_SKIPPED_H));
				}
				$str .= getStrToInclude($file2, $recent_visual_studio);
			}
			$_ = $str;
		}
		elsif ($_ =~ m/AUTOMATIC_HIGH_LEVEL_CODE/)
		{
			my $str = '';
			foreach my $file (@$rl_high_level_src)
			{
				$str .= getStrToInclude($file, $recent_visual_studio);
			}
			$_ = $str;
		}
	}
	
	open FDW, ">$output" or die $output;
	print FDW $_ foreach (@lines);
	close FDW;
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

#------------------------

1;
