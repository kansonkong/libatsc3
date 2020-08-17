#
# Application.mk
#
# note that APP_ABI is overridden by build.gradle setting.
#
APP_DEBUG := true

APP_ABI := all

APP_CPPFLAGS += -std=c++11 -fexceptions -D_ANDROID

