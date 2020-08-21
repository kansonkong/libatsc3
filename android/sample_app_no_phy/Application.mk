#
# Application.mk
#
# note that APP_ABI is overridden by build.gradle setting.
#
APP_DEBUG := true
APP_STRIP_MODE := "none"

APP_STL := c++_shared
APP_CPPFLAGS += -std=c++11 -fexceptions -D_ANDROID