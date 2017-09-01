#include <jni.h>

#include <math.h>
#include <string.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <stdio.h>
#include "jpeg/jpeglib.h"
#include <setjmp.h>

#define LOG_TAG "jni"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


typedef uint8_t BYTE;

#define true 1
#define false 0

struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void) my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    (*cinfo->err->output_message)(cinfo);

    LOGE("jpeg_message_table[%d]:%s", myerr->pub.msg_code,
         myerr->pub.jpeg_message_table[myerr->pub.msg_code]);
    LOGI("dds");

    longjmp(myerr->setjmp_buffer, 1);
}

//图片压缩方法
int generateJPEG(BYTE *data, int w, int h, int quality, const char *outfilename,
                 jboolean optimize) {
    int nComponent = 3;
    struct jpeg_compress_struct jcs;
    struct my_error_mgr jem;

    jcs.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = my_error_exit;
    if (setjmp(jem.setjmp_buffer)) {
        return 0;
    }
    //为JPEG对象分配空间并初始化
    jpeg_create_compress(&jcs);
    //获取文件信息
    FILE *f = fopen(outfilename, "wb");
    if (f == NULL) {
        return 0;
    }
    //指定压缩数据源
    jpeg_stdio_dest(&jcs, f);
    jcs.image_width = w;
    jcs.image_height = h;
    if (optimize) {
        LOGE("optimize==ture");
    } else {
        LOGE("optimize==false");
    }

    jcs.arith_code = false;
    jcs.input_components = nComponent;
    if (nComponent == 1)
        jcs.in_color_space = JCS_GRAYSCALE;
    else
        jcs.in_color_space = JCS_RGB;

    jpeg_set_defaults(&jcs);
    jcs.optimize_coding = optimize;
    //为压缩设定参数，包括图像大小，颜色空间
    jpeg_set_quality(&jcs, quality, true);
    //开始压缩
    jpeg_start_compress(&jcs, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride;
    row_stride = jcs.image_width * nComponent;
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer[0] = &data[jcs.next_scanline * row_stride];
        //写入数据
        jpeg_write_scanlines(&jcs, row_pointer, 1);
    }

    if (jcs.optimize_coding) {
        LOGE("optimize==ture");
    } else {
        LOGE("optimize==false");
    }
    //压缩完毕
    jpeg_finish_compress(&jcs);
    //释放资源
    jpeg_destroy_compress(&jcs);
    fclose(f);

    return 1;
}

//将java string转换为char*
char *jstringToString(JNIEnv *env, jbyteArray barr) {
    char *rtn = NULL;
    jsize alen = (*env)->GetArrayLength(env, barr);
    jbyte *ba = (*env)->GetByteArrayElements(env, barr, 0);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    (*env)->ReleaseByteArrayElements(env, barr, ba, 0);
    return rtn;
}

//图片压缩方法
//jni方法入口
jstring Java_com_android_ndk_LibJpegUtil_compressBitmap(JNIEnv *env, jclass jclazz,
                                                    jobject bitmapColor, int w,
                                                    int h, int quality,
                                                    jbyteArray fileNameStr, jboolean optimize) {
    AndroidBitmapInfo infoColor;
    BYTE *pixelsColor;
    int ret;
    BYTE *data;
    BYTE *tmpdata;
    char *fileName = jstringToString(env, fileNameStr);
    if ((ret = AndroidBitmap_getInfo(env, bitmapColor, &infoColor)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return (*env)->NewStringUTF(env, "0");;
    }
    if ((ret = AndroidBitmap_lockPixels(env, bitmapColor, (void **) &pixelsColor)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    BYTE r, g, b;
    data = NULL;
    data = malloc(w * h * 3);
    tmpdata = data;
    int j = 0, i = 0;
    int color;
    //将bitmap转换为rgb数据
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            color = *((int *) pixelsColor);
            r = ((color & 0x00FF0000) >> 16);
            g = ((color & 0x0000FF00) >> 8);
            b = color & 0x000000FF;
            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data = data + 3;
            pixelsColor += 4;

        }

    }
    AndroidBitmap_unlockPixels(env, bitmapColor);
    //进行压缩
    int resultCode = generateJPEG(tmpdata, w, h, quality, fileName, optimize);
    free(tmpdata);
    if (resultCode == 0) {
        jstring result = (*env)->NewStringUTF(env, "0");
        return result;
    }
    return (*env)->NewStringUTF(env, "1"); //success
}


