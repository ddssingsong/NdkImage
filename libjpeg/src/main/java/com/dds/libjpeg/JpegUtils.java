package com.dds.libjpeg;

import android.graphics.Bitmap;

/**
 * Created by dds on 2019/5/7.
 * android_shuai@163.com
 */
public class JpegUtils {

    public static native long compressBitmap(Bitmap bit, int w, int h, int quality, byte[] path, boolean optimize);
}
