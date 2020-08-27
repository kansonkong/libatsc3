#
# Application.mk
#
# note that APP_ABI is overridden by build.gradle setting.
# jjustman-2020-08-12- remove individual linkage to APP_STL := c++_shared as android studio 4.0 complains about
# 3 files found with path 'lib/arm64-v8a/libc++_shared.so'.

APP_DEBUG := true
APP_STRIP_MODE := "none"
#APP_STL := c++_shared

# APP_ABI := all

APP_CPPFLAGS += -std=c++11  -fexceptions -D_ANDROID

