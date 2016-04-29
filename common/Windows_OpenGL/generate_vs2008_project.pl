#!/usr/bin/perl

use strict;
use warnings;
use generate_vs2008_project;

generate_vs2008_project::processFile(
	"App_VS2008_SDL.vcproj.src", "App_VS2008_SDL.vcproj", [ "../MainClass.cpp"], 1);
generate_vs2008_project::processFile(
	"App_VS2008_OpenGL.vcproj.src", "App_VS2008_OpenGL.vcproj", [ "../MainClass.cpp" ], 0);
