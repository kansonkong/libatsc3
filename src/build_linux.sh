#!/bin/bash

LIBATSC3_SRC_DIR=`pwd`

sudo ../linux-build-deps

# Correctly link xlocale.h
sudo ln -s /usr/include/locale.h /usr/include/xlocale.h

# Compile libmicrohttpd
cd ../libmicrohttpd && tar -xvzf libmicrohttpd-latest.tar.gz && cd libmicrohttpd-0.9.63 && ./configure --prefix=`pwd`/build && make install

# Compile Bento4
cd ../../bento && mv lib lib-tmp && mkdir lib && mv lib-tmp/place_libap4.a_bento_file_here lib && mv lib-tmp/.gitignore lib && rm -Rf lib-tmp && cd lib && git clone https://github.com/axiomatic-systems/Bento4.git && cd Bento4 && mkdir cmakebuild && cd cmakebuild && cmake -DCMAKE_BUILD_TYPE=Debug .. && make && mv * ../..

# Build local openssl for standalone SRT utils
cd $LIBATSC3_SRC_DIR
cd ../openssl && KERNEL_BITS=64 ./Configure  no-asm -g3 -O0 -fno-omit-frame-pointer -fno-inline-functions --prefix=`pwd`/build_ssl --openssldir=`pwd`/build_ssl '-Wl,-rpath,$(LIBRPATH)'c && make && make install


# Compile SRT
cd $LIBATSC3_SRC_DIR
cd ../srt && rm -rf CMakeFiles && rm -rf build && rm -f CMakeCache.txt && ./configure --prefix `pwd`/build --openssl-include-dir=`pwd`/../openssl/build_ssl/include --openssl-ssl-library=`pwd`/../openssl/build_ssl/lib/libssl.a --openssl-crypto-library=`pwd`/../openssl/build_ssl/lib/libcrypto.a && make && make install

# Compile libatsc3
cd $LIBATSC3_SRC_DIR
make clean && make all
