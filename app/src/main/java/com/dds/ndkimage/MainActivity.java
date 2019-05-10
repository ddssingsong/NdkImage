package com.dds.ndkimage;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
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

public class MainActivity extends AppCompatActivity {


    private static final int PICK_IMAGE = 1;
    private static final int PICK_IMAGE_KITKAT = 2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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
            // 读取图片的方向
            int degree = readPictureDegree(path);
            Log.e("dds_test", path + "get degree:" + degree);
            File file = new File(Environment.getExternalStorageDirectory(), "ssss.jpg");
            NativeImageUtils.compressBitmap(bitmap, 50, file.getAbsolutePath());
            Log.e("dds_test", "set degree:" + degree);
            // 设置图片的方向
            setPictureDegreeZero(file.getAbsolutePath(), degree);
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
}
