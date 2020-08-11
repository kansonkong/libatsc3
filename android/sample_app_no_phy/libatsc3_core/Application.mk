#
# Application.mk
#
# note that APP_ABI is overridden by build.gradle setting.
#

#APP_STL := gnustl_shared
APP_STL := c++_shared

# APP_ABI := all
APP_ABI := arm64-v8a

APP_CPPFLAGS += -std=c++11 -D_ANDROID

