#
# Application.mk
#

APP_DEBUG := true
APP_STRIP_MODE := "none"
APP_CPPFLAGS += -std=c++11  -fexceptions -D_ANDROID
LOCAL_ARM_MODE := arm

ifeq ($(debugging_asan_enabled), true)
$(info building atsc3_phy_airwavz with debugging_asan_enabled )
	APP_CFLAGS += -fsanitize=address -fno-omit-frame-pointer -g -O0 -fno-optimize-sibling-calls -fsanitize-address-use-after-scope
	APP_LDFLAGS += -fsanitize=address
else ifeq ($(debugging_hwasan_enabled), true)
$(info building atsc3_phy_airwavz with debugging_hwasan_enabled )
	APP_CFLAGS += -fsanitize=hwaddress -fno-omit-frame-pointer  -g -O0 -fno-optimize-sibling-calls -fsanitize-address-use-after-scope
	APP_LDFLAGS += -fsanitize=hwaddress
endif