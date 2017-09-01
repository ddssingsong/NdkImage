//读取png图片，并返回宽高，若出错则返回NULL
unsigned char* ReadPng(const char* path, int* width, int* height);
int png_to_jpeg(const char *pngfile, const char *jpegfile, int jpegquality);
