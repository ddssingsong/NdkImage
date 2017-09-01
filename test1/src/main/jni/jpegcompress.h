#define true 1
#define false 0

/*
 *返回图片的宽度(w_Dest),
 *返回图片的高度(h_Dest),
 *返回图片的位深(bit_depth),
 *源图片的RGB数据(src),
 *源图片的宽度(w_Src),
 *源图片的高度(h_Src)
 */
unsigned char* do_Stretch_Linear(int w_Dest, int h_Dest, int bit_depth,
		unsigned char *src, int w_Src, int h_Src);
/*
 * 读取Jpeg图片的数据并返回，如果出错，返回NULL
 */
unsigned char* ReadJpeg(const char* path, int* width, int* height);

unsigned char* ReadJpeg2(const char* path, int* width, int* height);

int write_JPEG_file(const char * filename, unsigned char* image_buffer,
		int quality, int image_height, int image_width);

int write_JPEG_file_android(unsigned char* data, int w, int h, int quality,
		const char* outfilename, jboolean optimize);
