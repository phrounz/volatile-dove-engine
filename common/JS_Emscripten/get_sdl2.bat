curl -sL https://github.com/emscripten-ports/SDL2/archive/master.zip | unzip 
cd SDL-emscripten/
emconfigure ./configure --host=asmjs-unknown-emscripten --disable-assembly --disable-threads --enable-cpuinfo=false CFLAGS="-O2"
emmake make
PAUSE