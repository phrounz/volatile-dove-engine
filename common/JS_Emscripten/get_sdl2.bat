C:\Users\moi\AppData\Local\GitHub\PortableGit_c7e0cbde92ba565cb218a521411d0e854079a28c\cmd\git clone https://github.com/gsathya/SDL-emscripten.git
cd SDL-emscripten/
emconfigure ./configure --host=asmjs-unknown-emscripten --disable-assembly --disable-threads --enable-cpuinfo=false CFLAGS="-O2"
emmake make
PAUSE