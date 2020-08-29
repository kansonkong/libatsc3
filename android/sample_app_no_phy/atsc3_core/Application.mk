#
# Application.mk
#
# jjustman-2020-08-12- remove individual linkage to APP_STL := c++_shared as android studio 4.0 complains about
# 3 files found with path 'lib/arm64-v8a/libc++_shared.so'.

APP_DEBUG := true
APP_STRIP_MODE := "none"
# APP_STL := c++_shared


APP_CPPFLAGS += -std=c++11 -D_ANDROID

