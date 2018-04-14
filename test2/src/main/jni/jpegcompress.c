#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "jpegcompress.h"
#include <android/log.h>
#include <jpeg/jpeglib.h>

#define TAG    "dds_ndk"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void) my_error_exit(j_common_ptr cinfo) {
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	(*cinfo->err->output_message)(cinfo);

	longjmp(myerr->setjmp_buffer, 1);
}

//读取Jpeg图片的数据并返回，如果出错，返回NULL
unsigned char* ReadJpeg(const char* path, int* width, int* height) {
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		return NULL;
	}

	struct jpeg_decompress_struct info;
	struct my_error_mgr my_err;
	info.err = jpeg_std_error(&my_err.pub);
	my_err.pub.error_exit = my_error_exit;

	if (setjmp(my_err.setjmp_buffer)) {
		jpeg_destroy_decompress(&info);
		fclose(file);
		return NULL;
	}

	jpeg_create_decompress(&info);
	jpeg_stdio_src(&info, file);

	int ret_Read_Head = jpeg_read_header(&info, 1); //int

	if (ret_Read_Head != JPEG_HEADER_OK) {
		LOGD("error ret_Read_Head != JPEG");
		fclose(file);
		jpeg_destroy_decompress(&info);
		return NULL;
	}

	(void) jpeg_start_decompress(&info);
	int w = *width = info.output_width;
	int h = *height = info.output_height;
	int numChannels = info.num_components; // 3 = RGB, 4 = RGBA
	unsigned long dataSize = w * h * numChannels;
	unsigned char *data = (unsigned char *) malloc(dataSize);
	if (!data){
		LOGD("malloc data = null");
		return NULL;
	}
	unsigned char* rowptr;
	while (info.output_scanline < h) {
		rowptr = data + info.output_scanline * w * numChannels;
		jpeg_read_scanlines(&info, &rowptr, 1);
	}
	jpeg_finish_decompress(&info);
	fclose(file);
	return data;
}


//读取Jpeg图片的数据并返回，如果出错，返回NULL
unsigned char* ReadJpeg2(const char* path, int* width, int* height) {
	FILE *file = fopen(path, "rb");
    	if (file == NULL) {
    		return NULL;
    	}
    	struct jpeg_decompress_struct info;
    	struct my_error_mgr my_err;
    	info.err = jpeg_std_error(&my_err.pub);
    	my_err.pub.error_exit = my_error_exit;
    	if (setjmp(my_err.setjmp_buffer)) {
    		printf("Error occured\n");
    		jpeg_destroy_decompress(&info);
    		fclose(file);
    		return NULL;
    	}
    	jpeg_create_decompress(&info); //fills info structure
    	jpeg_stdio_src(&info, file);        //void
    	int ret_Read_Head = jpeg_read_header(&info, 1); //int
    	if (ret_Read_Head != JPEG_HEADER_OK) {
    		printf("jpeg_read_header failed\n");
    		fclose(file);
    		jpeg_destroy_decompress(&info);
    		return NULL;
    	}
    	(void) jpeg_start_decompress(&info);
    	int w = *width = info.output_width;
    	int h = *height = info.output_height;
    	int numChannels = info.num_components; // 3 = RGB, 4 = RGBA
    	unsigned long dataSize = w * h * numChannels;
    	unsigned char *data = (unsigned char *) malloc(dataSize);
    	if (!data)
    		return NULL;
    	unsigned char* rowptr;
    	while (info.output_scanline < h) {
    		rowptr = data + info.output_scanline * w * numChannels;
    		jpeg_read_scanlines(&info, &rowptr, h);
    	}
    int w_Dest=1280;
    int h_Dest=720;
    int w_Src=w;
    int h_Src=h;
    int bit_depth=24;
    int sw = w_Src - 1, sh = h_Src - 1, dw = w_Dest - 1, dh = h_Dest - 1;
    int i = 0, j = 0, k = 0;

    unsigned char *src=data;
	int B, N, x, y;
	int nPixelSize = bit_depth / 8;
	unsigned char *pLinePrev, *pLineNext;
	unsigned char *pDest = (unsigned char *) malloc(
			w_Dest * h_Dest * bit_depth / 8);
	unsigned char *tmp;
	unsigned char *pA, *pB, *pC, *pD;
	for (i = 0; i <= dh; ++i) {
		tmp = pDest + i * w_Dest * nPixelSize;
		y = i * sh / dh;
		N = dh - i * sh % dh;
		y++;
		pLinePrev = src + y * w_Src * nPixelSize;
		//LOGD("!!!! y = %d",y);
		if(N == dh){
		//LOGD("$$$$ y = %d",y);
		pLineNext=pLinePrev;
		}else{
		//LOGD("#### y = %d",y);
		pLineNext=pLinePrev;
		}
		//pLineNext = (N == dh) ? pLinePrev : src + y * w_Src * nPixelSize;
		for (j = 0; j <= dw; ++j) {
			x = j * sw / dw * nPixelSize;
			B = dw - j * sw % dw;
			pA = pLinePrev + x;
			pB = pA + nPixelSize;
			pC = pLineNext + x;
			pD = pC + nPixelSize;
			if (B == dw) {
				pB = pA;
				pD = pC;
			}
			for (k = 0; k < nPixelSize; ++k) {
				*tmp++ = (unsigned char) (int) ((B * N
						* (*pA++ - *pB - *pC + *pD) + dw * N * *pB++
						+ dh * B * *pC++ + (dw * dh - dh * B - dw * N) * *pD++
						+ dw * dh / 2) / (dw * dh));
			}
		}
	}
	jpeg_finish_decompress(&info);
	fclose(file);
	return pDest;
}

/*
 *返回图片的宽度(w_Dest),
 *返回图片的高度(h_Dest),
 *返回图片的位深(bit_depth),
 *源图片的RGB数据(src),
 *源图片的宽度(w_Src),
 *源图片的高度(h_Src)
 */
unsigned char* do_Stretch_Linear(int w_Dest, int h_Dest, int bit_depth,
		unsigned char *src, int w_Src, int h_Src) {
	int sw = w_Src - 1, sh = h_Src - 1, dw = w_Dest - 1, dh = h_Dest - 1;
	int B, N, x, y;
	int nPixelSize = bit_depth / 8;
	unsigned char *pLinePrev, *pLineNext;
	unsigned char *pDest = (unsigned char *) malloc(
			w_Dest * h_Dest * bit_depth / 8);
	unsigned char *tmp;
	unsigned char *pA, *pB, *pC, *pD;
	int i = 0, j = 0, k = 0;
	for (i = 0; i <= dh; ++i) {
		tmp = pDest + i * w_Dest * nPixelSize;
		y = i * sh / dh;
		N = dh - i * sh % dh;
		pLinePrev = src + (y++) * w_Src * nPixelSize;
		pLineNext = (N == dh) ? pLinePrev : src + y * w_Src * nPixelSize;
		for (j = 0; j <= dw; ++j) {
			x = j * sw / dw * nPixelSize;
			B = dw - j * sw % dw;
			pA = pLinePrev + x;
			pB = pA + nPixelSize;
			pC = pLineNext + x;
			pD = pC + nPixelSize;
			if (B == dw) {
				pB = pA;
				pD = pC;
			}

			for (k = 0; k < nPixelSize; ++k) {
				*tmp++ = (unsigned char) (int) ((B * N
						* (*pA++ - *pB - *pC + *pD) + dw * N * *pB++
						+ dh * B * *pC++ + (dw * dh - dh * B - dw * N) * *pD++
						+ dw * dh / 2) / (dw * dh));
			}
		}
	}
	LOGD("do_Stretch_Linear success!");
	return pDest;
}

int write_JPEG_file(const char * filename, unsigned char* image_buffer,
		int quality, int image_height, int image_width) {
	if (filename == NULL || image_buffer == NULL)
		return 0;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;
	JSAMPROW row_pointer[1];
	int row_stride;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(filename, "wb")) == NULL) {
		return 0;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = image_width;
	cinfo.image_height = image_height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE );

	jpeg_start_compress(&cinfo, TRUE);

	row_stride = image_width * 3;

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	jpeg_destroy_compress(&cinfo);

	return 1;
}

int write_JPEG_file_android(unsigned char* data, int w, int h, int quality,
		const char* outfilename, jboolean optimize) {
	int nComponent = 3;
	struct jpeg_compress_struct jcs;
	struct my_error_mgr jem;
	jcs.err = jpeg_std_error(&jem.pub);
	jem.pub.error_exit = my_error_exit;
	if (setjmp(jem.setjmp_buffer)) {
		return 0;
	}
	jpeg_create_compress(&jcs);
	FILE* f = fopen(outfilename, "wb");
	if (f == NULL) {
		return 0;
	}
	jpeg_stdio_dest(&jcs, f);
	jcs.image_width = w;
	jcs.image_height = h;
	jcs.arith_code = false;
	jcs.input_components = nComponent;
	if (nComponent == 1){
		//灰度图
		jcs.in_color_space = JCS_GRAYSCALE;
	} else{
		//彩色图
		jcs.in_color_space = JCS_RGB;
	}
	jpeg_set_defaults(&jcs);
	jcs.optimize_coding = optimize;
	jpeg_set_quality(&jcs, quality, true);
	jpeg_start_compress(&jcs, TRUE);
	JSAMPROW row_pointer[1];
	int row_stride;
	row_stride = jcs.image_width * nComponent;
	while (jcs.next_scanline < jcs.image_height) {
		row_pointer[0] = &data[jcs.next_scanline * row_stride];
		jpeg_write_scanlines(&jcs, row_pointer, 1);
	}
	jpeg_finish_compress(&jcs);
	jpeg_destroy_compress(&jcs);
	fclose(f);
	LOGD("write_JPEG_file_android success!");
	return 1;
}
