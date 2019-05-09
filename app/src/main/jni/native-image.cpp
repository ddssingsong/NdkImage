#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <malloc.h>
#include "jpeglib.h"

void write_JPEG_file(uint8_t *temp, int w, int h, jint q, const char *path);

extern "C"
JNIEXPORT void JNICALL
Java_com_dds_ndkimage_NativeImageUtils_compressBitmap(JNIEnv *env, jclass type, jobject bitmap,
                                                      jint q,
                                                      jstring p_) {
    if (bitmap == NULL) {
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
    write_JPEG_file(temp, w, h, q, path);

    //释放内存
    free(temp);
    AndroidBitmap_unlockPixels(env, bitmap);
    env->ReleaseStringUTFChars(p_, path);
}

void write_JPEG_file(uint8_t *data, int w, int h, jint q, const char *path) {
    //3.1、创建jpeg压缩对象
    jpeg_compress_struct jcs;
    //错误回调
    jpeg_error_mgr error;
    jcs.err = jpeg_std_error(&error);
    //创建压缩对象
    jpeg_create_compress(&jcs);
    //    3.2、指定存储文件  write binary
    FILE *f = fopen(path, "wb");
    jpeg_stdio_dest(&jcs, f);
    //    3.3、设置压缩参数
    jcs.image_width = (JDIMENSION) (w);
    jcs.image_height = (JDIMENSION) h;
    //bgr
    jcs.input_components = 3;
    jcs.in_color_space = JCS_RGB;
    jpeg_set_defaults(&jcs);
    //开启哈夫曼功能
    jcs.optimize_coding = true;
    jpeg_set_quality(&jcs, q, 1);
    //    3.4、开始压缩
    jpeg_start_compress(&jcs, 1);
    //    3.5、循环写入每一行数据
    int row_stride = w * 3;//一行的字节数
    JSAMPROW row[1];
    while (jcs.next_scanline < jcs.image_height) {
        //取一行数据
        uint8_t *pixels = data + jcs.next_scanline * row_stride;
        row[0] = pixels;
        jpeg_write_scanlines(&jcs, row, 1);
    }
    //    3.6、压缩完成
    jpeg_finish_compress(&jcs);
    //    3.7、释放jpeg对象
    fclose(f);
    jpeg_destroy_compress(&jcs);

}
