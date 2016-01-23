#!/bin/sh

cp ../battleThemeA.wav ../WorkDir/data/sounds/opengameart_cynicmusic_battleThemeA.wav
perl ../../common/JS_Emscripten/compile.pl ../App_VS2008GL/AnUnconventionalWeapon/*.cpp
rm -f ../WorkDir/data/sounds/opengameart_cynicmusic_battleThemeA.wav
