LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE:= zlib
LOCAL_SRC_FILES:= \
adler32.c \
compress.c \
crc32.c \
deflate.c \
gzclose.c \
gzlib.c \
gzread.c \
gzwrite.c \
infback.c \
inflate.c \
inftrees.c \
inffast.c \
trees.c \
uncompr.c \
zutil.c 

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
include $(BUILD_STATIC_LIBRARY)
