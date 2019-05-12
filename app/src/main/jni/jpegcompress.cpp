//
// Created by dds on 2019/5/10.
//

#include <jni.h>
#include <string>
#include <setjmp.h>
#include "jpeglib.h"
#include "jpegcompress.h"
#include <android/log.h>

#define TAG    "dds_native_image"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void) my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

void jpegcompress::write_JPEG_file(uint8_t *data, int w, int h, jint q, const char *path) {

    //3.1、创建jpeg压缩对象
    jpeg_compress_struct jcs;
    //错误回调
    jpeg_error_mgr error;
    jcs.err = jpeg_std_error(&error);
    //创建压缩对象
    jpeg_create_compress(&jcs);

    //3.2、指定存储文件  write binary
    FILE *f = fopen(path, "wb");
    jpeg_stdio_dest(&jcs, f);
    //3.3、设置压缩参数
    jcs.image_width = (JDIMENSION) (w);
    jcs.image_height = (JDIMENSION) h;
    //bgr
    jcs.input_components = 3;
    jcs.in_color_space = JCS_RGB;
    jpeg_set_defaults(&jcs);
    //开启哈夫曼功能
    jcs.optimize_coding = true;
    jpeg_set_quality(&jcs, q, 1);
    //4 开始压缩
    jpeg_start_compress(&jcs, 1);
    // 5 循环写入每一行数据
    int row_stride = w * 3;//一行的字节数
    JSAMPROW row[1];
    while (jcs.next_scanline < jcs.image_height) {
        //取一行数据
        uint8_t *pixels = data + jcs.next_scanline * row_stride;
        row[0] = pixels;
        jpeg_write_scanlines(&jcs, row, 1);
    }
    //6 压缩完成
    jpeg_finish_compress(&jcs);
    //7 释放jpeg对象
    fclose(f);
    jpeg_destroy_compress(&jcs);
}

unsigned char *jpegcompress::read_JPEG_file(const char *path, int *width, int *height) {
    FILE *infile;
    if ((infile = fopen(path, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", path);
        return 0;
    }
    struct jpeg_decompress_struct info;
    struct my_error_mgr my_err;
    info.err = jpeg_std_error(&my_err.pub);
    my_err.pub.error_exit = my_error_exit;

    if (setjmp(my_err.setjmp_buffer)) {
        jpeg_destroy_decompress(&info);
        fclose(infile);
        return NULL;
    }
    jpeg_create_decompress(&info);
    jpeg_stdio_src(&info, infile);

    int ret_Read_Head = jpeg_read_header(&info, TRUE); //int

    if (ret_Read_Head != JPEG_HEADER_OK) {
        LOGD("error ret_Read_Head != JPEG");
        fclose(infile);
        jpeg_destroy_decompress(&info);
        return NULL;
    }
    (void) jpeg_start_decompress(&info);
    int w = *width = info.output_width;
    int h = *height = info.output_height;
    int numChannels = info.num_components; // 3 = RGB, 4 = RGBA
    int dataSize = w * h * numChannels;
    unsigned char *data = (unsigned char *) malloc(dataSize);
    if (!data) {
        LOGD("malloc data = null");
        fclose(infile);
        jpeg_destroy_decompress(&info);
        return NULL;
    }
    unsigned char *row_ptr;
    while (info.output_scanline < h) {
        row_ptr = data + info.output_scanline * w * numChannels;
        jpeg_read_scanlines(&info, &row_ptr, 1);
    }
    jpeg_finish_decompress(&info);
    fclose(infile);
    return data;
}


