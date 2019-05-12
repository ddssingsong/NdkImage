//
// Created by dds on 2019/5/10.
//

#ifndef NDKIMAGE_JPEGCOMPRESS_H
#define NDKIMAGE_JPEGCOMPRESS_H


#include <jni.h>

class jpegcompress {
private:

public:
    static unsigned char *read_JPEG_file(const char *path, int *width, int *height);

    static void write_JPEG_file(uint8_t *data, int w, int h, jint q, const char *path);
};

#endif //NDKIMAGE_JPEGCOMPRESS_H
