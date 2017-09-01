LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_MODULE    := ndkimage
LOCAL_SRC_FILES := \
jpegcompress.c \
ndkimage.c \
pngcompress.c

LOCAL_STATIC_LIBRARIES := libjpeg
LOCAL_STATIC_LIBRARIES += libpng
LOCAL_STATIC_LIBRARIES += zlib

LOCAL_C_INCLUDES += $(LOCAL_PATH)/jpeg
LOCAL_C_INCLUDES += $(LOCAL_PATH)/png
LOCAL_C_INCLUDES += $(LOCAL_PATH)/zlib

LOCAL_LDLIBS := -ljnigraphics
LOCAL_LDLIBS += -llog
APP_ALLOW_MISSING_DEPS :=true

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

include $(LOCAL_PATH)/jpeg/Android.mk  $(LOCAL_PATH)/png/Android.mk $(LOCAL_PATH)/zlib/Android.mk
