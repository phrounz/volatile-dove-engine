@echo off

copy ..\battleThemeA.wav ..\WorkDir\data\sounds\opengameart_cynicmusic_battleThemeA.wav
perl ../../common/JS_Emscripten/compile.pl ../App_VS2008GL/AnUnconventionalWeapon/*.cpp
del ..\WorkDir\data\sounds\opengameart_cynicmusic_battleThemeA.wav
PAUSE