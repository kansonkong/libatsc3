#
# Application.mk
#

APP_DEBUG := true
APP_STRIP_MODE := "none"

APP_STL := c++_shared
APP_ABI := arm64-v8a

APP_CPPFLAGS += -std=c++11 -fexceptions -D_ANDROID

# jjustman-2020-10-07 - hwasan is only available on android 10 or higher
APP_CFLAGS := -fsanitize=hwaddress -fno-omit-frame-pointer -g -O0
APP_LDFLAGS := -fsanitize=hwaddress

# fall back to plain asan
# ifeq ($(ENABLE_HWASAN),xxxx)
# APP_CFLAGS := -fsanitize=address -fno-omit-frame-pointer -g -O1 -fno-optimize-sibling-calls
# APP_LDFLAGS := -fsanitize=address
# endif


