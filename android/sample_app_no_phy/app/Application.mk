#
# Application.mk
#
# note that APP_ABI is overridden by build.gradle setting.
#
APP_DEBUG := true
APP_STRIP_MODE := "none"

APP_STL := c++_shared
APP_CPPFLAGS += -std=c++11 -fexceptions -D_ANDROID

#-fsanitize=address -fno-omit-frame-pointer -g -O0
#
#APP_CFLAGS := -fsanitize=address -fno-omit-frame-pointer -g -O0
#APP_LDFLAGS := -fsanitize=address

#only the main app build will get the c++ shared .so



