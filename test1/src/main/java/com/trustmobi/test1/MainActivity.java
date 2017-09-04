package com.trustmobi.test1;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.android.ndk.AlbumUtil;
import com.android.ndk.ImageNativeUtil;
import com.android.ndk.ImageTools;

import java.io.File;
import java.math.BigDecimal;

public class MainActivity extends Activity {
    private static final int PICK_IMAGE = 1;
    private static final int PICK_IMAGE_KITKAT = 2;

    private ImageView image;
    private TextView text;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        image = (ImageView) findViewById(R.id.image);
        text = (TextView) findViewById(R.id.text);

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
        // 获取图片路径
        if (resultCode == Activity.RESULT_OK) {
            switch (requestCode) {
                case PICK_IMAGE:
                    if (data != null) {
                        Uri uri = data.getData();
                        uriToBitmap(uri);
                    }
                    break;
                case PICK_IMAGE_KITKAT:
                    if (data != null) {
                        Uri uri = ensureUriPermition(this, data);
                        data.getData();
                        uriToBitmap(uri);
                    }

                    break;
            }

        }
    }

    @SuppressLint("NewApi")
    public static Uri ensureUriPermition(Context context, Intent intent) {
        Uri uri = intent.getData();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            final int takeFlags = intent.getFlags()
                    & (Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
            context.getContentResolver().takePersistableUriPermission(uri, takeFlags);
        }
        return uri;

    }

    File file;

    private void uriToBitmap(final Uri uri) {
        String path = AlbumUtil.getRealPathFromUri(this, uri);
        Log.i("dds", "图片路径" + path);
        file = new File(path);
        Bitmap bitmap = BitmapFactory.decodeFile(file.getAbsolutePath());
        image.setImageBitmap(bitmap);
        text.setText("原图:" + getFormatSize(file.length()));

    }

    public void compress(View view) {
        if (file == null) return;
        new Thread(new Runnable() {

            @Override
            public void run() {
                // 对文件进行操
                final File file2 = new File(Environment.getExternalStorageDirectory(), "ddss.jpg");
                long result = ImageNativeUtil.zoomcompress(file.getAbsolutePath().getBytes(), file2.getAbsolutePath().getBytes(), true, ImageTools.Quality.SMALL.getQuality());
                Log.i("dds", "返回结果" + result);
                runOnUiThread(new Runnable() {
                    public void run() {
                        if (file2.exists()) {
                            Bitmap bitmap = BitmapFactory.decodeFile(file2.getAbsolutePath());
                            image.setImageBitmap(bitmap);
                            text.setText("压缩过的图：" + getFormatSize(file2.length()));
                        }
                    }
                });

            }
        }).start();
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