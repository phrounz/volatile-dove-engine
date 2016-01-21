#!/bin/bash

wget https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz || (echo failed && exit 1) # note: prefix with https_proxy=... if you have a proxy
tar zxvf emsdk-portable.tar.gz || (echo failed && exit 1)
cd emsdk_portable/ || (echo failed && exit 1)
SDK_MASTER=`uname -a|grep x86_64>/dev/null && echo sdk-master-64bit || echo sdk-master-32bit`
./emsdk install $SDK_MASTER || (echo failed && exit 1) # note: prefix with https_proxy=... if you have a proxy
