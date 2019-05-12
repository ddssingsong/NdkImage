package com.dds.ndkimage;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import java.io.File;
import java.io.IOException;
import java.math.BigDecimal;

public class MainActivity extends AppCompatActivity {


    private static final int PICK_IMAGE = 1;
    private static final int PICK_IMAGE_KITKAT = 2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (!PermissionUtil.isNeedRequestPermission(this)) {

        }
    }


    public void pickImage(View view) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT) {
            Intent intent = new Intent(Intent.ACTION_PICK,
                    android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
            startActivityForResult(intent, PICK_IMAGE);
        } else {
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType("image/*");
            startActivityForResult(intent, PICK_IMAGE_KITKAT);
        }


    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        //获取图片路径
        if (resultCode == Activity.RESULT_OK) {
            switch (requestCode) {
                case PICK_IMAGE:
                    if (data != null) {
                        Uri uri = data.getData();
                        compressBitmap(uri);
                    }
                    break;
                case PICK_IMAGE_KITKAT:
                    if (data != null) {
                        Uri uri = ensureUriPermition(this, data);
                        compressBitmap(uri);
                    }

                    break;
            }

        }
    }

    @SuppressWarnings("ResourceType")
    public static Uri ensureUriPermition(Context context, Intent intent) {
        Uri uri = intent.getData();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            final int takeFlags = intent.getFlags()
                    & (Intent.FLAG_GRANT_READ_URI_PERMISSION
                    | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
            context.getContentResolver().takePersistableUriPermission(uri, takeFlags);
        }
        return uri;


    }

    private void compressBitmap(Uri uri) {
        Bitmap bitmap = null;
        try {
            bitmap = MediaStore.Images.Media.getBitmap(getContentResolver(), uri);
            String path = Uri2PathUtil.getRealPathFromUri(this, uri);
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inJustDecodeBounds = true;
            BitmapFactory.decodeFile(path, options);
            String mimeType = options.outMimeType;

            Log.d("dds_test", "图片类型：" + mimeType);
            Log.d("dds_test", "压缩前大小：" + getFormatSize(new File(path).length()));

            // 读取图片的方向
            int degree = readPictureDegree(path);
            Log.d("dds_test", "path:" + path);
            Log.d("dds_test", "\n 图片方向:" + degree);
            File file = new File(Environment.getExternalStorageDirectory(), "ssss.jpg");
            int result = NativeImageUtils.zoomCompress(path, file.getAbsolutePath(), 65);
            if (result > 0) {
                Log.d("dds_test", "压缩完大小：" + getFormatSize(file.length()));
                // 设置图片的方向
                setPictureDegreeZero(file.getAbsolutePath(), degree);

                sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + file.getAbsolutePath())));
            } else {
                Log.e("dds_test", "压缩失败！");
            }

        } catch (Exception e) {
            e.printStackTrace();
        }


    }


    // 读取文件的方向
    public static int readPictureDegree(String path) {
        int degree = 0;
        try {
            ExifInterface exifInterface = new ExifInterface(path);
            int orientation = exifInterface.getAttributeInt(
                    ExifInterface.TAG_ORIENTATION,
                    ExifInterface.ORIENTATION_NORMAL);
            switch (orientation) {
                case ExifInterface.ORIENTATION_ROTATE_90:
                    degree = 90;
                    break;
                case ExifInterface.ORIENTATION_ROTATE_180:
                    degree = 180;
                    break;
                case ExifInterface.ORIENTATION_ROTATE_270:
                    degree = 270;
                    break;
            }
        } catch (IOException e) {
            Log.e("dds_test", e.toString());
            e.printStackTrace();
        }
        return degree;
    }

    // 设置文件的方向
    public static void setPictureDegreeZero(String path, int degree) {
        try {
            Log.d("dds_test", "设置压缩完图片的方向:" + degree);
            ExifInterface exifInterface = new ExifInterface(path);
            if (degree == 90) {
                exifInterface.setAttribute(ExifInterface.TAG_ORIENTATION, "6");
            } else if (degree == 180) {
                exifInterface.setAttribute(ExifInterface.TAG_ORIENTATION, "3");
            } else if (degree == 270) {
                exifInterface.setAttribute(ExifInterface.TAG_ORIENTATION, "8");
            } else {
                exifInterface.setAttribute(ExifInterface.TAG_ORIENTATION, "1");
            }
            exifInterface.saveAttributes();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    // 格式化单位
    private static String getFormatSize(double size) {
        double kiloByte = size / 1024;
        if (kiloByte < 1) {
            return size + "Byte";
        }
        double megaByte = kiloByte / 1024;
        if (megaByte < 1) {
            BigDecimal result1 = new BigDecimal(Double.toString(kiloByte));
            return result1.setScale(2, BigDecimal.ROUND_HALF_UP).toPlainString() + "KB";
        }
        double gigaByte = megaByte / 1024;
        if (gigaByte < 1) {
            BigDecimal result2 = new BigDecimal(Double.toString(megaByte));
            return result2.setScale(2, BigDecimal.ROUND_HALF_UP).toPlainString() + "MB";
        }
        double teraBytes = gigaByte / 1024;
        if (teraBytes < 1) {
            BigDecimal result3 = new BigDecimal(Double.toString(gigaByte));
            return result3.setScale(2, BigDecimal.ROUND_HALF_UP).toPlainString() + "GB";
        }
        BigDecimal result4 = new BigDecimal(teraBytes);
        return result4.setScale(2, BigDecimal.ROUND_HALF_UP).toPlainString() + "TB";
    }
}
