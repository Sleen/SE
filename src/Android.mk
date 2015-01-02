# Copyright (C) 2010 The Android Open Source Project
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

LIB := ../../lib
LIB2 := ../lib

include $(CLEAR_VARS)
LOCAL_MODULE := minizip
LOCAL_SRC_FILES := $(LIB)/minizip/bin/libminizip.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := png
LOCAL_SRC_FILES := $(LIB)/png/bin/libpng.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := freetype2
LOCAL_SRC_FILES := $(LIB)/freetype2/bin/libfreetype2.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := lua
LOCAL_SRC_FILES := $(LIB)/lua/bin/liblua.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

SOURCES := core/MyTime.cpp
SOURCES += core/json11.cpp
SOURCES += core/Type.cpp
SOURCES += core/Impl.cpp
SOURCES += core/Stream.cpp
SOURCES += core/FileSystem.cpp
SOURCES += math/Matrix.cpp
SOURCES += graphics/Brush.cpp
SOURCES += graphics/Pen.cpp
SOURCES += graphics/Graphics.cpp
SOURCES += graphics/ImageLoader.cpp
SOURCES += graphics/Bitmap.cpp
SOURCES += graphics/GLHelper.cpp
SOURCES += graphics/Shader.cpp
SOURCES += graphics/Model.cpp
SOURCES += graphics/Texture.cpp
SOURCES += graphics/Font.cpp
SOURCES += graphics/Transform.cpp
SOURCES += app/Control.cpp
SOURCES += app/WindowCommon.cpp
SOURCES += app/AndroidWindow.cpp
SOURCES += gui/Button.cpp
SOURCES += test/test_main.cpp

#SOURCES := main.cpp
LOCAL_MODULE    := native-activity

LOCAL_C_INCLUDES := .
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/core 
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/math
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/graphics
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/app
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/gui
LOCAL_C_INCLUDES +=	$(LIB2)/minizip/include
LOCAL_C_INCLUDES +=	$(LIB2)/png/include
LOCAL_C_INCLUDES +=	$(LIB2)/freetype2/include
LOCAL_C_INCLUDES +=	$(LIB2)/lua/include

LOCAL_CPPFLAGS += -DANDROID_NDK
# -D_STLP_NO_CWCHAR
LOCAL_CPPFLAGS += -std=c++11 -D_DEBUG -Wno-error=format-security -Wno-invalid-offsetof
LOCAL_SRC_FILES := $(SOURCES)
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -lz
# -lGLESv1_CM
LOCAL_STATIC_LIBRARIES := android_native_app_glue cpufeatures ndk_helper minizip png freetype2 lua

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,android/cpufeatures)
$(call import-module,android/ndk_helper)
