
CFLAGS=-Wno-nullability-completeness -Wno-expansion-to-defined -Wno-builtin-requires-header -Wno-c++11-extensions
CXXFLAGS=-std=c++11 

DEBUG=-g -O1

#DEBUG=-g -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls -O1 -fsanitize-address-use-after-scope
ASAN=-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls -O1 -fsanitize-address-use-after-scope

LOCAL_LD_LIBS=-lz -lm -lpthread -lpcap -lpcre2-8 -lstdc++
LOCAL_LD_LIBS_NCURSES=$(LOCAL_LD_LIBS) -lncurses

all: 	libatsc3-srt-live-transmit

clean:
	-rm -f libatsc3-srt-live-transmit
	
	
# /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++     -DENABLE_LOGGING=1 -Wall -Wextra -O3 -DNDEBUG -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk -Wl,-search_paths_first -Wl,-headerpad_max_install_names  CMakeFiles/srt-live-transmit.dir/apps/srt-live-transmit.cpp.o CMakeFiles/srtsupport_virtual.dir/apps/apputil.cpp.o CMakeFiles/srtsupport_virtual.dir/apps/logsupport.cpp.o CMakeFiles/srtsupport_virtual.dir/apps/socketoptions.cpp.o CMakeFiles/srtsupport_virtual.dir/apps/transmitmedia.cpp.o CMakeFiles/srtsupport_virtual.dir/apps/uriparser.cpp.o CMakeFiles/srtsupport_virtual.dir/apps/verbose.cpp.o  -o srt-live-transmit  libsrt.a /usr/local/opt/openssl@1.1/lib/libssl.dylib /usr/local/opt/openssl@1.1/lib/libcrypto.dylib 


libatsc3-srt-live-transmit:	Atsc3SRT_live_transmit.cpp \
	Atsc3SRT_Target.h

	g++ -g -O1 -std=c++11  Atsc3SRT_live_transmit.cpp \
		apputil.cpp \
		logsupport.cpp \
		socketoptions.cpp \
		transmitmedia.cpp \
		uriparser.cpp \
		verbose.cpp \
		-I . \
		-I ../../srt/build/include \
		-I ../../srt/srtcore \
		-I ../../srt \
		-L ../../srt/build/lib \
		-L /usr/local/opt/openssl@1.1/lib \
		/Users/jjustman/Desktop/libatsc3/srt/build/lib/libsrt.a \
		-lssl -lcrypto \
		-o libatsc3-srt-live-transmit

