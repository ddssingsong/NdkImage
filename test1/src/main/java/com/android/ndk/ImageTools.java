package com.android.ndk;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.ExifInterface;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

/**
 * 操作图片的工具类
 *
 */
public class ImageTools {
    // 临时文件夹
    @SuppressLint("SdCardPath")
	private final static String temp = "/sdcard/test/temp/";

    /**
     * 图片质量
     * Big--大图质量，按照720p图片压缩标准进行压缩
     * Thum--缩略图质量，按照480p图片压缩标准进行压缩
     * Portrait--头像压缩标准，按照才剪完以后的图片进行头像压缩的预留枚举值
     *
     */
    public enum Quality {
        BIG(1), THUM(2), PORTRAIT(3), SMALL(4);

        private int q;

        private Quality(int q) {
            this.q = q;
        }

        public int getQuality() {
            return q;
        }
    }

    /**
     * 压缩图片获取BitMap数据
     *
     * @param oldimage 原图
     * @param q        图片质量枚举
     * @return
     */
    public static Bitmap getCommpressImage(String inputFilepath, Quality q) {
        File dirFile = new File(temp);
        if (!dirFile.exists()) {
            dirFile.mkdirs();
        }
        File jpegTrueFile = new File(dirFile, new java.util.Date().getTime() + ".jpg");
        ImageNativeUtil.compressBitmap(inputFilepath, jpegTrueFile.getAbsolutePath(), true, q);
        Bitmap nImage = BitmapFactory.decodeFile(jpegTrueFile.getAbsolutePath());
        jpegTrueFile.delete();
        return nImage;
    }

    /**
     * 压缩图片获取字节流
     *
     * @param oldimage
     * @param q
     * @return
     */
    public static byte[] getCommpressImage2Byte(String inputFilepath, Quality q) {
        File dirFile = new File(temp);
        if (!dirFile.exists()) {
            dirFile.mkdirs();
        }
        File jpegTrueFile = new File(dirFile, new java.util.Date().getTime() + ".jpg");
        ImageNativeUtil.compressBitmap(inputFilepath, jpegTrueFile.getAbsolutePath(), true, q);
        //增加方向exif
        setExif(inputFilepath, jpegTrueFile.getAbsolutePath());

        byte[] newf = getBytesFromFile(jpegTrueFile);
        jpegTrueFile.delete();
        return newf;
    }

    /**
     * 存储压缩图片
     *
     * @param oldimage 原图
     * @param q        图片质量枚举
     * @param filename 存储文件名
     * @return
     */
    public static void saveCommpressImage(String inputFilepath, Quality q, String outputFilepath) {
        ImageNativeUtil.compressBitmap(inputFilepath, outputFilepath, false, q);
        //增加方向exif
        setExif(inputFilepath, outputFilepath);
    }

    /**
     * 文件转化为字节数组
     *
     * @param file
     * @return
     */
    private static byte[] getBytesFromFile(File file) {
        byte[] ret = null;
        try {
            if (file == null) {
                // log.error("helper:the file is null!");
                return null;
            }
            FileInputStream in = new FileInputStream(file);
            ByteArrayOutputStream out = new ByteArrayOutputStream(4096);
            byte[] b = new byte[4096];
            int n;
            while ((n = in.read(b)) != -1) {
                out.write(b, 0, n);
            }
            in.close();
            out.close();
            ret = out.toByteArray();
        } catch (IOException e) {
            // log.error("helper:get bytes from file process error!");
            e.printStackTrace();
        }
        return ret;
    }

    /**
     * 加入方向exif
     *
     * @param input
     * @param output
     */
    public static void setExif(String input, String output) {
        try {
            ExifInterface outexif = new ExifInterface(output);
            outexif.setAttribute("UserComment", "dds");
            outexif.saveAttributes();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
