#include "png.h"
#include "pngcompress.h"
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "jpeglib.h"
#include <assert.h>
#define tmin(a, b) ((a)>(b) ? (b):(a))
#define tmax(a,b)  ((a) > (b)?(a):(b))

#include <android/log.h>
#define TAG    "dds"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
//!1不要将这三个宏同时打开需要互斥

//#define USE_PREMULTIPLY_APLHA (0)//使用预乘
#define USE_RGBA (1)//使用 r g b a
//#define USE_ABGR (0)//使用 a b g r

//预乘 aplha
#define CC_RGB_PREMULTIPLY_APLHA(vr, vg, vb, va) \
 (unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
 ((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
 ((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
 ((unsigned)(unsigned char)(va) << 24))

//r g b a
#define CC_RGB_PREMULTIPLY_APLHA_RGBA(vr, vg, vb, va)\
 ( (unsigned)(vr))|\
    ( (unsigned)(vg) << 8)|\
  ( (unsigned)(vb) << 16)|\
  ((unsigned)(va) << 24)

//a b g r
//使用该宏的时候 write png时需要调用函数 png_set_swap_alpha
#define CC_RGB_PREMULTIPLY_APLHA_ABGR(vr, vg, vb, va)\
 ( (unsigned)(vr) << 8)|\
 ( (unsigned)(vg) << 16)|\
 ( (unsigned)(vb) << 24)|\
 ((unsigned)(va))

//读取png图片，并返回宽高，若出错则返回NULL
unsigned char* ReadPng(const char* path, int* width, int* height) {
	FILE* file = fopen(path, "rb");
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0,
			0);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));
	png_init_io(png_ptr, file);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	int m_width = *width = png_get_image_width(png_ptr, info_ptr);
	int m_height = *height = png_get_image_height(png_ptr, info_ptr);
	int color_type = png_get_color_type(png_ptr, info_ptr);
	int bytesPerComponent = 3, i = 0, j = 0, p = 0;
	if (color_type & PNG_COLOR_MASK_ALPHA) {
		bytesPerComponent = 4;
		p = 1;
	}
	int size = m_height * m_width * bytesPerComponent;
	unsigned char *pImateRawData = (unsigned char *) malloc(size);
	png_bytep* rowPointers = png_get_rows(png_ptr, info_ptr);
	int bytesPerRow = m_width * bytesPerComponent;
	if (p == 1) {
		unsigned int *tmp = (unsigned int *) pImateRawData;
		for (i = 0; i < m_height; i++) {
			for (j = 0; j < bytesPerRow; j += 4) {
#if USE_PREMULTIPLY_APLHA
				*tmp++ = CC_RGB_PREMULTIPLY_APLHA( rowPointers[i][j], rowPointers[i][j + 1],
						rowPointers[i][j + 2], rowPointers[i][j + 3] );
#elif USE_RGBA
				*tmp++ = CC_RGB_PREMULTIPLY_APLHA_RGBA(rowPointers[i][j],
						rowPointers[i][j + 1], rowPointers[i][j + 2],
						rowPointers[i][j + 3]);
#elif USE_ABGR
				*tmp++ = CC_RGB_PREMULTIPLY_APLHA_ABGR( rowPointers[i][j], rowPointers[i][j + 1],
						rowPointers[i][j + 2], rowPointers[i][j + 3] );
#endif
			}
		}
	} else {
		for (j = 0; j < m_height; ++j) {
			memcpy(pImateRawData + j * bytesPerRow, rowPointers[j],
					bytesPerRow);
		}
	}
	return pImateRawData;
}
int png_to_jpeg(const char *pngfile, const char *jpegfile, int jpegquality) {
	FILE *fpin = fopen(pngfile, "rb");
	if (!fpin) {
		return 1;
	}

	unsigned char header[8];
	fread(header, 1, 8, fpin);
	if (png_sig_cmp(header, 0, 8)) {
			LOGD("2");
		//fprintf(stderr, "this is not a PNG file\n");
		return 2;
	}

	int ret = 0;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0,
			0);
	assert(png_ptr);

	png_infop info_ptr = png_create_info_struct(png_ptr);
	assert(info_ptr);

	png_infop end_info = png_create_info_struct(png_ptr);
	assert(end_info);

	if (setjmp(png_jmpbuf(png_ptr))) {
		//fprintf(stderr, "failed.\n");
		ret = 3;
		goto error_png;
	}

	png_init_io(png_ptr, fpin);
	png_set_sig_bytes(png_ptr, 8);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0);
	png_bytep * row_pointers = png_get_rows(png_ptr, info_ptr);

	png_uint_32 width, height;
	int bit_depth, color_type;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0,
			0, 0);

	if (color_type != PNG_COLOR_TYPE_RGB_ALPHA && color_type != PNG_COLOR_TYPE_RGB) {
		//fprintf(stderr, "input PNG must be RGB+Alpha\n");
		ret = 4;
		goto error_png;
	}
	if (bit_depth != 8) {
		//fprintf(stderr, "input bit depth must be 8bit!\n");
		ret = 5;
		goto error_png;
	}

	//printf("png is %ldx%ld\n", width, height);
	int channels = png_get_channels(png_ptr, info_ptr);
	if (channels != 4 && channels != 3) {
		//fprintf(stderr, "channels must be 4.\n");
		ret = 6;
		goto error_png;
	}

	/* now write jpeg */
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW jrow_pointer[1];
	FILE *outfp;

	outfp = fopen(jpegfile, "wb");
	if (!outfp) {
		//perror(jpegfile);
		ret = 7;
		goto error_png;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfp);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, jpegquality, 1);
	jpeg_start_compress(&cinfo, 1);

	unsigned char *row = malloc(width * 3);
	while (cinfo.next_scanline < cinfo.image_height) {
		int x,y;
		jrow_pointer[0] = row;
		unsigned char *source = row_pointers[cinfo.next_scanline];
		switch(color_type){
            case PNG_COLOR_TYPE_RGB_ALPHA:
                for (x = 0; x < width; ++x) {
                			row[x * 3 + 0] = source[0];
                			row[x * 3 + 1] = source[1];
                			row[x * 3 + 2] = source[2];
                			source += 4;
                     }
                break;

            case PNG_COLOR_TYPE_RGB:
                for (x = 0; x < width; ++x) {
                            row[x * 3 + 0] = source[0];
                            row[x * 3 + 1] = source[1];
                            row[x * 3 + 2] = source[2];
                            source += 3;
                }
                break;
		}
		jpeg_write_scanlines(&cinfo, jrow_pointer, 1);
	}

	error_jpeg: if (row)
		free(row);

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	if (outfp)
		fclose(outfp);
	error_png: if (fpin)
		fclose(fpin);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	return ret;
}
