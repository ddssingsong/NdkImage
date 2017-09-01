package com.android.ndk;

import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.support.annotation.RequiresApi;
import android.support.v4.content.CursorLoader;

import java.io.File;


public class AlbumUtil {
	
	
	//根据返回的Uri获取到图片的路径
	
	public static String getRealPathFromUri(Context context, Uri uri) {
		int sdkVersion = Build.VERSION.SDK_INT;
		if (sdkVersion < 11) {
			return getRealPathFromUri_BelowApi11(context, uri);
		}
		if (sdkVersion < 19) {
			return getRealPathFromUri_Api11To18(context, uri);
		}
		return getRealPathFromUri_AboveApi19(context, uri);
	}

	/**
	 * 适配api19以上,根据uri获取图片的绝对路径
	 */
	@RequiresApi(api = Build.VERSION_CODES.KITKAT)
	private static String getRealPathFromUri_AboveApi19(Context context, Uri uri) {
		if (DocumentsContract.isDocumentUri(context, uri)) {

			if (isExternalStorageDocument(uri)) {
				String docId = DocumentsContract.getDocumentId(uri);
				String[] split = docId.split(":");
				String type = split[0];
				if ("primary".equalsIgnoreCase(type)) {
					return Environment.getExternalStorageDirectory() + File.separator + split[1];
				}
			} else if (isDownloadsDocument(uri)) {
				String wholeID = DocumentsContract.getDocumentId(uri);
				Uri contentUri = ContentUris.withAppendedId(Uri.parse("content://downloads/public_downloads"),
						Long.valueOf(wholeID));
				return getDataColumn(context, contentUri, null, null);
			} else if (isMediaDocument(uri)) {
				String docId = DocumentsContract.getDocumentId(uri);
				String[] split = docId.split(":");
				String type = split[0];
				Uri contentUri = null;
				if ("image".equals(type)) {
					contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
				} else if ("video".equals(type)) {
					contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
				} else if ("audio".equals(type)) {
					contentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
				}
				String selection = MediaStore.Images.Media._ID + "=?";
				String[] selectionArgs = new String[] { split[1] };
				return getDataColumn(context, contentUri, selection, selectionArgs);
			}
		} else if ("content".equalsIgnoreCase(uri.getScheme())) {
			// Return the remote address
			if (isGooglePhotosUri(uri))
				return uri.getLastPathSegment();
			return getDataColumn(context, uri, null, null);
		}
		// File
		else if ("file".equalsIgnoreCase(uri.getScheme())) {
			return uri.getPath();
		}

		return null;

	}

	public static String getDataColumn(Context context, Uri uri, String selection, String[] selectionArgs) {
		Cursor cursor = null;
		String column = MediaStore.Images.Media.DATA;
		String[] projection = { column };
		try {
			cursor = context.getContentResolver().query(uri, projection, selection, selectionArgs, null);
			if (cursor != null && cursor.moveToFirst()) {
				int index = cursor.getColumnIndexOrThrow(column);
				return cursor.getString(index);
			}
		} finally {
			if (cursor != null)
				cursor.close();
		}
		return null;
	}

	/**
	 * 适配api11-api18,根据uri获取图片的绝对路径
	 */
	private static String getRealPathFromUri_Api11To18(Context context, Uri uri) {
		String filePath = null;
		String[] projection = { MediaStore.Images.Media.DATA };
		CursorLoader loader = new CursorLoader(context, uri, projection, null, null, null);
		Cursor cursor = loader.loadInBackground();
		if (cursor != null) {
			cursor.moveToFirst();
			filePath = cursor.getString(cursor.getColumnIndex(projection[0]));
			cursor.close();
		}
		return filePath;
	}

	/**
	 * （已测可用）适配api11以下(不包括api11),根据uri获取图片的绝对路径
	 */
	private static String getRealPathFromUri_BelowApi11(Context context, Uri uri) {

		String filePath = null;
		String[] projection = { MediaStore.Images.Media.DATA };
		Cursor cursor = context.getContentResolver().query(uri, projection, null, null, null);
		if (cursor != null) {
			cursor.moveToFirst();
			filePath = cursor.getString(cursor.getColumnIndex(projection[0]));
			cursor.close();
		}
		return filePath;
	}

	
	
	public static boolean isExternalStorageDocument(Uri uri) {
		return "com.android.externalstorage.documents".equals(uri.getAuthority());
	}

	public static boolean isDownloadsDocument(Uri uri) {
		return "com.android.providers.downloads.documents".equals(uri.getAuthority());
	}

	public static boolean isMediaDocument(Uri uri) {
		return "com.android.providers.media.documents".equals(uri.getAuthority());
	}

	public static boolean isGooglePhotosUri(Uri uri) {
		return "com.google.android.apps.photos.content".equals(uri.getAuthority());
	}
}
