#include <string.h>
#include <math.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <jni.h>
#include <stdio.h>
#include "jpegcompress.h"
#include "pngcompress.h"

#define TAG    "dds"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#define true 1
#define false 0

typedef uint8_t BYTE;

char* jstrinTostring(JNIEnv* env, jbyteArray barr) {
	char* rtn = NULL;
	jsize alen = (*env)->GetArrayLength(env, barr);
	jbyte* ba = (*env)->GetByteArrayElements(env, barr, 0);
	if (alen > 0) {
		rtn = (char*) malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	(*env)->ReleaseByteArrayElements(env, barr, ba, 0);
	return rtn;
}
/**
 * 返回文件大小
 */
long fileSize(const char* inputFile) {
	FILE* f = fopen(inputFile, "rb");
	long s = 0;
	if (f == NULL) {
		return s;
	} else {
		fseek(f, 0, SEEK_END);
		s = ftell(f);
		fclose(f);
		return s;
	}
}
//最大公约数
int proportion(int m, int n) {
	if (m < n) {
		int temp = m;
		m = n;
		n = temp;
	}
	while (m % n != 0) {
		int temp = m % n;
		m = n;
		n = temp;
	}
	return n;
}
//获取文件扩展名
char* substr(const char*str) {
	char *ptr, c = '.';
	int pos;
	ptr = strrchr(str, c);
	pos = ptr - str + 1;
	unsigned n = strlen(str) - pos;
	static char stbuf[256];
	strncpy(stbuf, str + pos, n);
	stbuf[n] = 0;
	return stbuf;
}

/**
 * 检查扩展名是否支持
 */
int checkExt(const char *fileName) {
	char* ext = substr(fileName);
	if (strcasecmp(ext, "jpg") == 0) {
		return 1;
	} else if (strcasecmp(ext, "jpeg") == 0) {
		return 1;
	} else if (strcasecmp(ext, "bmp") == 0) {
		return 1;
	} else if (strcasecmp(ext, "png") == 0) {
		return 2;
	} else {
		return -1;
	}
}

/**
 * 缩放规则
 * 1,1/4，清晰度为65的图
 * 2,1/8，清晰度为50的图
 * 3，质量为50的图
 * 4，超小图115*115，质量为10
 */
int imageRule(int* tb_w, int *tb_h, int* q, int w, int h, int quality,
		long size) {
	int rx = w > h ? w : h;
	int ry = w > h ? h : w;
	int rz = proportion(rx, ry);
	switch (quality) {
	case 1:
		if (size > 1024 * 200) {
			if (w > h ? w / h : h / w < 4) {
				*tb_w = w > h ? 1280 : ((int) (1280 / (float) (1.0 * h / w)));
				*tb_h = h > w ? 1280 : ((int) (1280 / (float) (1.0 * w / h)));
				*q = 65;
			} else {
				*tb_w = w;
				*tb_h = h;
				*q = 65;
				return 0;
			}
		} else {
			*tb_w = w;
			*tb_h = h;
			*q = 65;
		}
		break;
	case 2:
		if (size > 1024 * 50) {
			*tb_w = w > h ? 480 : ((int) (480 / (float) (1.0 * h / w)));
			*tb_h = h > w ? 480 : ((int) (480 / (float) (1.0 * w / h)));
			*q = 50;
		} else {
			*tb_w = w;
			*tb_h = h;
			*q = 65;
		}
		break;
	case 3:
		if (size > 1024 * 50) {
			*tb_w = 400;
			*tb_h = 400;
			*q = 50;
		} else {
			*tb_w = w;
			*tb_h = h;
			*q = 65;
		}
		break;
	case 4:
		*tb_w = 115;
		*tb_h = 115;
		*q = 10;
		break;
	}
	return 1;
}

//生成图片的缩略图（图片的一个缩小版本）
int generate_image_thumbnail(const char* inputFile, const char* outputFile,
		jboolean optimize, int quality) {
	if (inputFile == NULL || outputFile == NULL)
		return 0;

	//读取图片像素数组
	int w = 0, h = 0, tb_w = 0, tb_h = 0, q = 0;
	//图片文件大小
	long size = fileSize(inputFile);
	unsigned char* buff = NULL;
	int ext = checkExt(inputFile);
	switch (ext) {
	case 1: // JPG 图片
		buff = ReadJpeg(inputFile, &w, &h);
		break;
	case 2: // PNG 图片
		png_to_jpeg(inputFile, outputFile, 100);
		buff = ReadJpeg(outputFile, &w, &h);
		remove(outputFile);
		break;
	default:
		buff = ReadJpeg(inputFile, &w, &h);
		break;
	}

	if (buff == NULL) {
		printf("ReadJpeg Failed\n");
		return 0;
	}
	if (imageRule(&tb_w, &tb_h, &q, w, h, quality, size) != 0) {
		//缩放图片，缩放后的大小为(tb_w,tb_h)
		buff = do_Stretch_Linear(tb_w, tb_h, 24, buff, w, h);
	}

	//将缩放后的像素数组保存到jpeg文件
//	write_JPEG_file(outputFile, img_buf, 65, tb_h, tb_w);
	write_JPEG_file_android(buff, tb_w, tb_h, q, outputFile, optimize);
	free(buff);
	return 1;
}

long Java_com_android_ndk_ImageNativeUtil_zoomcompress(JNIEnv* env,
		jobject thiz, jbyteArray input, jbyteArray output, jboolean optimize,
		int quality) {
	char * inputfile = jstrinTostring(env, input);
	char * outputfile = jstrinTostring(env, output);
	return generate_image_thumbnail(inputfile, outputfile, optimize, quality);
}

long Java_com_android_ndk_ImageNativeUtil_compressBitmap(JNIEnv *env,
		jclass jclazz, jobject bitmapColor, int w, int h, int quality,
		jbyteArray fileNameStr, jboolean optimize) {

	AndroidBitmapInfo infoColor;
	BYTE *pixelsColor;
	int ret;
	BYTE *data;
	BYTE *tmpdata;
	char *fileName = jstrinTostring(env, fileNameStr);
	//读取图片失败
	if ((ret = AndroidBitmap_getInfo(env, bitmapColor, &infoColor)) < 0) {
		LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
		return false;
	}

	if ((ret = AndroidBitmap_lockPixels(env, bitmapColor,
			(void **) &pixelsColor)) < 0) {
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
	//开始进行压缩
	int resultCode = write_JPEG_file_android(tmpdata, w, h, quality, fileName, optimize);
	free(tmpdata);
	if (resultCode == 0) {
		return false;
	}
	return true;

}



