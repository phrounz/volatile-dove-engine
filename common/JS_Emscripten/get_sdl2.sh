#!/usr/bin/sh

wget https://github.com/emscripten-ports/SDL2/archive/master.zip 
unzip master.zip 
cd SDL2-master
../emsdk_portable/emscripten/master/emconfigure ./configure --host=asmjs-unknown-emscripten --disable-assembly --disable-threads --enable-cpuinfo=false CFLAGS="-O2"
../emsdk_portable/emscripten/master/emmake make
