# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

# Endeavour
include $(CLEAR_VARS)

LOCAL_MODULE    := Endeavour-jni

LOCAL_SRC_FILES := Endeavour-jni.cpp
LOCAL_SRC_FILES += Endeavour/brCmd.cpp
LOCAL_SRC_FILES += Endeavour/brUser.cpp
LOCAL_SRC_FILES += Endeavour/IT9300.cpp

LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/sl_config.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/sl_utils.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/sl_i2c.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/sl_tuner.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/sl_demod.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/sl_demod_regs.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/sl_log.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/sl_ts.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/R855.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/I2C_Sys.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/atsc1.cpp
LOCAL_SRC_FILES += Endeavour/Receiver/SL3000_R855/atsc3.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/Endeavour

#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

# for do log
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog

include $(BUILD_SHARED_LIBRARY)
