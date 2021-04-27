#
# Application.mk
#
# Airwavz PHY build configurations for asan/hwasan/debug/release

APP_STRIP_MODE := "none"
APP_CPPFLAGS += -std=c++11  -fexceptions -D_ANDROID

ifeq ($(debugging_asan_enabled), true)
$(info 'building atsc3_phy_airwavz with debugging_asan_enabled' )
    APP_DEBUG := true
    LOCAL_ARM_MODE := arm

	APP_CFLAGS += -fsanitize=address -fno-omit-frame-pointer -g -O0 -fno-optimize-sibling-calls -fsanitize-address-use-after-scope
	APP_LDFLAGS += -fsanitize=address
else ifeq ($(debugging_hwasan_enabled), true)
$(info building 'atsc3_phy_airwavz with debugging_hwasan_enabled' )
    APP_DEBUG := true
    LOCAL_ARM_MODE := arm

	APP_CFLAGS += -fsanitize=hwaddress -fno-omit-frame-pointer  -g -O0 -fno-optimize-sibling-calls -fsanitize-address-use-after-scope
	APP_LDFLAGS += -fsanitize=hwaddress
else ifeq ($(debugging_g_optimization_zero_flags), true)
$(info 'building atsc3_phy_airwavz with debugging_g_optimization_zero_flags' )
    APP_DEBUG := true
    LOCAL_ARM_MODE := arm

    APP_CFLAGS += -g -O0 -fno-optimize-sibling-calls
else
$(info 'building atsc3_phy_airwavz with APP_CLFAGS += -O2' )
    APP_CFLAGS += -O2
endif
