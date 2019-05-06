package com.android.ndk;

import android.graphics.Bitmap;

/**
 * Created by dds on 2017/5/28 0028.
 * I Am the Changer, You had me at Hello World
 */

public class LibJpegUtil {

    static {

    }

    /**
     * 图片压缩
     *
     * @param w
     * @param h
     * @param quality
     * @param filename
     * @param optimize
     */
    public static native void compressBitmap(Bitmap bitmap, int w, int h, int quality, byte[] filename, boolean optimize);
}
