# atsc3_core
# Application.mk
#
APP_STRIP_MODE := "none"

APP_STL := c++_shared
APP_CPPFLAGS += -std=c++11 -fexceptions -D_ANDROID -g

ifeq ($(debugging_asan_enabled), true)
$(info 'building atsc3_core with debugging_asan_enabled' )
	APP_DEBUG := true
	LOCAL_ARM_MODE := arm

	APP_CFLAGS += -fsanitize=address -fno-omit-frame-pointer -g -O0 -fno-optimize-sibling-calls -fsanitize-address-use-after-scope
	APP_LDFLAGS += -fsanitize=address
else ifeq ($(debugging_hwasan_enabled), true)
$(info 'building atsc3_core with debugging_hwasan_enabled' )
	APP_DEBUG := true
	LOCAL_ARM_MODE := arm

	APP_CFLAGS += -fsanitize=hwaddress -fno-omit-frame-pointer  -g -O0 -fno-optimize-sibling-calls -fsanitize-address-use-after-scope
	APP_LDFLAGS += -fsanitize=hwaddress
else ifeq ($(debugging_g_optimization_zero_flags), true)
$(info 'building atsc3_core with debugging_g_optimization_zero_flags' )
	APP_DEBUG := true
	LOCAL_ARM_MODE := arm

	APP_CFLAGS += -g -O0 -fno-optimize-sibling-calls
else
$(info 'building atsc3_core with APP_CLFAGS += -O2' )
	APP_CFLAGS += -O2
endif
