package com.android.ndk;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;

/**
 * Created by dds on 18-4-12.
 */

public class ImageNativeUtil {

    private static int DEFAULT_QUALITY = 95;

    public static void compressBitmap(Bitmap bit, String fileName, boolean optimize, int level) {
        compressBitmap(bit, DEFAULT_QUALITY, fileName, optimize, level);

    }

    public static void compressBitmap(Bitmap bit, int quality, String fileName, boolean optimize, int level) {
        if (bit.getConfig() != Bitmap.Config.ARGB_8888) {
            Bitmap result;
            result = Bitmap.createBitmap(bit.getWidth() / level, bit.getHeight() / level, Bitmap.Config.ARGB_8888);
            Canvas canvas = new Canvas(result);
            Rect rect = new Rect(0, 0, bit.getWidth() / level, bit.getHeight() / level);
            canvas.drawBitmap(bit, null, rect, null);
            saveBitmap(result, quality, fileName, optimize);
            result.recycle();
        } else {
            saveBitmap(bit, quality, fileName, optimize);
        }

    }

    public static void compressBitmap(String input, String output, boolean optimize, ImageTools.Quality q) {
        zoomcompress(input.getBytes(), output.getBytes(), optimize, q.getQuality());
    }

    private static void saveBitmap(Bitmap bit, int quality, String fileName, boolean optimize) {
        compressBitmap(bit, bit.getWidth(), bit.getHeight(), quality, fileName.getBytes(), optimize);
    }

    public static native long compressBitmap(Bitmap bit, int w, int h, int quality, byte[] fileNameBytes,
                                             boolean optimize);

    public static native long zoomcompress(byte[] input, byte[] output, boolean optimize, int q);


    static {
        System.loadLibrary("ndk-image");
    }
}
