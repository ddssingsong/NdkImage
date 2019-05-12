#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <malloc.h>
#include "jpegcompress.h"
#include <android/log.h>
#include <jpeglib.h>

#define TAG    "dds_native_image"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


#define JPEG_FILE_TYPE          1
#define PNG_FILE_TYPE           2
#define BMP_FILE_TYPE           3
#define GIF_FILE_TYPE           4

jint generate_image_thumbnail(JNIEnv *pEnv, const char *input, const char *output, jint quality);

int checkFileType(const unsigned char *buf);

extern "C"
JNIEXPORT void JNICALL
Java_com_dds_ndkimage_NativeImageUtils_compressBitmap(JNIEnv *env, jclass type, jobject bitmap,
                                                      jint q,
                                                      jstring p_) {
    if (bitmap == NULL) {
        LOGE("bitmap is null");
        return;
    }
    const char *path = env->GetStringUTFChars(p_, 0);
    AndroidBitmapInfo info;
    // 获取bitmap中信息
    AndroidBitmap_getInfo(env, bitmap, &info);

    // 获取图片中的像素信息
    uint8_t *pixels;
    AndroidBitmap_lockPixels(env, bitmap, (void **) (&pixels));

    int w = info.width;
    int h = info.height;
    int color;
    //data中可以存放图片的所有内容
    uint8_t *data = (uint8_t *) malloc((size_t) (w * h * 3));
    uint8_t *temp = data;
    uint8_t r, g, b;//byte

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            color = *(int *) pixels;
            //取出rgb
            r = (uint8_t) ((color >> 16) & 0xFF);//    #00rrggbb  16  0000rr   8  00rrgg
            g = (uint8_t) ((color >> 8) & 0xFF);
            b = (uint8_t) (color & 0xFF);
            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data += 3;
            //指针跳过4个字节
            pixels += 4;
        }
    }
    //把得到的新的图片的信息存入一个新文件 中
    jpegcompress::write_JPEG_file(temp, w, h, q, path);
    //释放内存
    free(temp);
    AndroidBitmap_unlockPixels(env, bitmap);
    env->ReleaseStringUTFChars(p_, path);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_dds_ndkimage_NativeImageUtils_zoomCompress(JNIEnv *env, jclass type, jstring input_,
                                                    jstring output_, jint q) {
    const char *input = env->GetStringUTFChars(input_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);

    int result = generate_image_thumbnail(env, input, output, q);

    env->ReleaseStringUTFChars(input_, input);
    env->ReleaseStringUTFChars(output_, output);

    return result;


}

// 压缩图片
jint generate_image_thumbnail(JNIEnv *pEnv, const char *input, const char *output, jint quality) {
    if (input == NULL || output == NULL) {
        LOGE("input file or output file is null");
        return 0;
    }
    FILE *f = fopen(input, "rb+");
    long size = 0;
    if (f == NULL) {
        LOGE("file open failed!");
        return 0;
    } else {
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        LOGD("read file size:%ld", size);

    }
    int w = 0, h = 0;
    unsigned char *buff = NULL;
    buff = (unsigned char *) malloc(size);
    if (buff) {
        memset(buff, 0, size);
    }
    fseek(f, 0, SEEK_SET);
    if (fread(buff, 1, size, f) != size) {
        fclose(f);
        free(buff);
        LOGE("file read 4 byte failed!");
        return 0;
    }
    fclose(f);

    unsigned char *data = NULL;
    int ext = checkFileType(buff);
    switch (ext) {
        case JPEG_FILE_TYPE: // jpeg
            data = jpegcompress::read_JPEG_file(input, &w, &h);
            break;
        case PNG_FILE_TYPE:  // png

            LOGD(" file type is png! ");

            break;
        case BMP_FILE_TYPE: //  bmp
            data = jpegcompress::read_JPEG_file(input, &w, &h);
            break;
        case GIF_FILE_TYPE: //  gif
            LOGD(" file type is gif! ");
            break;
        default:
            data = jpegcompress::read_JPEG_file(input, &w, &h);
            break;
    }
    if (data == NULL) {
        printf("ReadJpeg Failed\n");
        free(buff);
        return 0;
    }
    jpegcompress::write_JPEG_file(data, w, h, quality, output);

    free(data);
    free(buff);
    return 1;
}

/*
　　通过文件头标识判断图片格式，
　　jpg： FF, D8
　　bmp： 42 4D
　　gif： 47 49 46 38
　　png： 89 50 4E 47
*/
int checkFileType(const unsigned char *buf) {
    if (buf[0] == 0xFF && buf[1] == 0xd8 && buf[2] == 0xFF) {
        return JPEG_FILE_TYPE;
    }
    if (buf[0] == 0x42 && buf[1] == 0x4d) {
        return BMP_FILE_TYPE;
    }
    if (buf[0] == 0x47 && buf[1] == 0x49 && buf[2] == 0x46 && buf[3] == 0x38) {
        return GIF_FILE_TYPE;
    }
    if (buf[0] == 0x89 && buf[1] == 0x50 && buf[2] == 0x4e && buf[3] == 0x47) {
        return PNG_FILE_TYPE;
    } else
        return 0;
}
