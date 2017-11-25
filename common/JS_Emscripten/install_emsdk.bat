@echo off

cd emsdk-portable-64bit || cd emsdk-portable || ( PAUSE && exit /B)

:: note: "emsdk activate --global latest" requires administrative privileges
emsdk update && emsdk install latest && emsdk activate latest && emsdk_env.bat && emsdk activate --global latest && PAUSE
