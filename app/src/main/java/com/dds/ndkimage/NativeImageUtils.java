package com.dds.ndkimage;

import android.graphics.Bitmap;

/**
 * Created by dds on 2019/5/9.
 * android_shuai@163.com
 */
public class NativeImageUtils {


    // 将Bitmap压缩后存到文件
    public static native void compressBitmap(Bitmap b, int q, String p);

    static {
        System.loadLibrary("native-image");
    }
}
