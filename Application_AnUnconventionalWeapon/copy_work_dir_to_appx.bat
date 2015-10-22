@echo off

IF NOT EXIST WorkDirStore GOTO NOWorkDirStore

del /s /f /q WorkDirStore\AppX\data\*.* && for /f %%f in ('dir /ad /b WorkDirStore\AppX\data\') do rd /s /q WorkDirStore\AppX\data\%%f && rmdir WorkDirStore\AppX\data
xcopy WorkDir\data WorkDirStore\AppX\data /e

copy battleThemeA.wav WorkDirStore\AppX\data\sounds\opengameart_cynicmusic_battleThemeA.wav
del WorkDirStore\AppX\data\sounds\opengameart_cynicmusic_battleThemeA.ogg

:NOWorkDirStore
PAUSE
