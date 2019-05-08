#include "png.h"
#include "pngcompress.h"
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <assert.h>
#include <android/log.h>

#define TAG    "dds_libpng"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

//r g b a
#define CC_RGB_PREMULTIPLY_APLHA_RGBA(vr, vg, vb, va)\
  ( (unsigned)(vr))|\
  ( (unsigned)(vg) << 8)|\
  ( (unsigned)(vb) << 16)|\
  ( (unsigned)(va) << 24)

//读取png图片，并返回宽高，若出错则返回NULL
unsigned char *ReadPng(const char *path, int *width, int *height) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        LOGE("file open failed,file is not found");
        return NULL;
    }
    // 初始化libpng
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    // 创建图像信息
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        LOGE("png_create_info_struct failed!");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(file);
        return NULL;
    }
    // 设置错误返回点
    if (setjmp(png_jmpbuf(png_ptr))) {
        LOGE("setjmp error!");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(file);
        return NULL;
    }
    // 设置libpng的数据源
    png_init_io(png_ptr, file);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);

    int m_width = *width = png_get_image_width(png_ptr, info_ptr);
    int m_height = *height = png_get_image_height(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);


    int bytesPerComponent = 3, i = 0, j = 0, p = 0;
    if (color_type & PNG_COLOR_MASK_ALPHA) {
        bytesPerComponent = 4;
        p = 1;
    }

    size_t size = (size_t) (m_height * m_width * bytesPerComponent);
    unsigned char *pImageRawData = (unsigned char *) malloc(size);
    png_bytep *rowPointers = png_get_rows(png_ptr, info_ptr);

    int bytesPerRow = m_width * bytesPerComponent;
    if (p == 1) {
        unsigned int *tmp = (unsigned int *) pImageRawData;
        for (i = 0; i < m_height; i++) {
            for (j = 0; j < bytesPerRow; j += 4) {
                *tmp++ = CC_RGB_PREMULTIPLY_APLHA_RGBA(rowPointers[i][j],
                                                       rowPointers[i][j + 1],
                                                       rowPointers[i][j + 2],
                                                       rowPointers[i][j + 3]);
            }
        }
    } else {
        for (j = 0; j < m_height; ++j) {
            memcpy(pImageRawData + j * bytesPerRow, rowPointers[j], (size_t) bytesPerRow);
        }
    }
    // 释放信息
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(file);
    LOGD("ReadPng success!");
    return pImageRawData;
}
