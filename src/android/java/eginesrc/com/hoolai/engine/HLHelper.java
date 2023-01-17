package com.hoolai.engine;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.util.Locale;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.res.AssetManager;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

public class HLHelper {
	// ===========================================================
	// Constants
	// ===========================================================
	private static final String PREFS_NAME = "HLPrefsFile";

	// ===========================================================
	// Fields
	// ===========================================================

	private static AssetManager sAssetManager;
	private static boolean sAccelerometerEnabled;
	private static String sPackageName;
	public static String sFileDirectory;
	private static Context sContext = null;
	private static HLHelperListener sHLHelperListener;
	private static HLMusic sHLMusic;
	private static HLSound sHLSound;

	// ===========================================================
	// Constructors
	// ===========================================================

	public static void init(final Context pContext,
			final HLHelperListener pHLHelperListener) {
		final ApplicationInfo applicationInfo = pContext.getApplicationInfo();

		HLHelper.sContext = pContext;
		HLHelper.sHLHelperListener = pHLHelperListener;

		if (!android.os.Environment.getExternalStorageState().equals(
				android.os.Environment.MEDIA_MOUNTED)) {
			AlertDialog.Builder builder = new AlertDialog.Builder(pContext);
			builder.setMessage("sd卡不可用");

			builder.setTitle("提示");

			builder.setPositiveButton("确定", new OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					dialog.dismiss();
					terminateProcess();
				}
			});

			builder.create().show();
			return;
		}

		HLHelper.sPackageName = applicationInfo.packageName;
		HLHelper.sFileDirectory =sContext.getExternalFilesDir(null).getAbsolutePath() +
				File.separator + "data" + File.separator;
		System.out.println("sFileDirectory==" + sFileDirectory);
		File f = new File(HLHelper.sFileDirectory);
		
		if (!f.exists()) {
			f.mkdirs();
		}

		HLHelper.sAssetManager = pContext.getAssets();

		HLHelper.sHLMusic = new HLMusic(pContext);
		HLHelper.sHLSound = new HLSound(pContext);

		HLHelper.nativeSetAssetManager(sAssetManager);
	}

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	public static HLHelperListener getsHLHelperListener() {
		return sHLHelperListener;
	}

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	// ===========================================================
	// Methods
	// ===========================================================

	private static native void nativeSetAssetManager(AssetManager a);

	private static native void nativeSetEditTextDialogResult(
			final byte[] pBytes, final boolean shouldReturn);

	private static native void nativeDeactiveTextInput();

	public static void setEditTextDialogResult(final String pResult,
			final boolean shouldReturn) {
		try {
			final byte[] bytesUTF8 = pResult.getBytes("UTF8");

			HLHelper.sHLHelperListener.runOnGLThread(new Runnable() {
				@Override
				public void run() {
					HLHelper.nativeSetEditTextDialogResult(bytesUTF8,
							shouldReturn);
				}
			});
		} catch (UnsupportedEncodingException pUnsupportedEncodingException) {
			/* Nothing. */
		}
	}

	public static void deactiveTextInput() {
		HLHelper.sHLHelperListener.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				HLHelper.nativeDeactiveTextInput();
			}
		});
	}

	private static void showEditTextDialog(final String pTitle,
			final String pMessage, final int pInputMode, final int pInputFlag,
			final int pReturnType, final int pMaxLength, final boolean multiLine, final String digitsFilter) {
		HLHelper.sHLHelperListener.showEditTextDialog(pTitle, pMessage,
				pInputMode, pInputFlag, pReturnType, pMaxLength, multiLine, digitsFilter);
	}

	public static String getCocos2dxPackageName() {
		return HLHelper.sPackageName;
	}

	public static String getWritablePath() {
		return HLHelper.sFileDirectory;
	}

	public static String getCurrentLanguage() {
		return Locale.getDefault().getLanguage();
	}

	public static String getDeviceModel() {
		return Build.MODEL;
	}

	public static AssetManager getAssetManager() {
		return HLHelper.sAssetManager;
	}

	public static void terminateProcess() {
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	public static int getDPI() {
		if (sContext != null) {
			DisplayMetrics metrics = new DisplayMetrics();
			WindowManager wm = ((Activity) sContext).getWindowManager();
			if (wm != null) {
				Display d = wm.getDefaultDisplay();
				if (d != null) {
					d.getMetrics(metrics);
					return (int) (metrics.density * 160.0f);
				}
			}
		}
		return -1;
	}

	// ===========================================================
	// Functions for CCUserDefault
	// ===========================================================

	public static boolean getBoolForKey(String key, boolean defaultValue) {
		SharedPreferences settings = ((Activity) sContext)
				.getSharedPreferences(HLHelper.PREFS_NAME, 0);
		return settings.getBoolean(key, defaultValue);
	}

	public static int getIntegerForKey(String key, int defaultValue) {
		SharedPreferences settings = ((Activity) sContext)
				.getSharedPreferences(HLHelper.PREFS_NAME, 0);
		return settings.getInt(key, defaultValue);
	}

	public static float getFloatForKey(String key, float defaultValue) {
		SharedPreferences settings = ((Activity) sContext)
				.getSharedPreferences(HLHelper.PREFS_NAME, 0);
		return settings.getFloat(key, defaultValue);
	}

	public static double getDoubleForKey(String key, double defaultValue) {
		// SharedPreferences doesn't support saving double value
		SharedPreferences settings = ((Activity) sContext)
				.getSharedPreferences(HLHelper.PREFS_NAME, 0);
		return settings.getFloat(key, (float) defaultValue);
	}

	public static String getStringForKey(String key, String defaultValue) {
		SharedPreferences settings = ((Activity) sContext)
				.getSharedPreferences(HLHelper.PREFS_NAME, 0);
		return settings.getString(key, defaultValue);
	}

	public static void setBoolForKey(String key, boolean value) {
		SharedPreferences settings = ((Activity) sContext)
				.getSharedPreferences(HLHelper.PREFS_NAME, 0);
		SharedPreferences.Editor editor = settings.edit();
		editor.putBoolean(key, value);
		editor.commit();
	}

	public static void setIntegerForKey(String key, int value) {
		SharedPreferences settings = ((Activity) sContext)
				.getSharedPreferences(HLHelper.PREFS_NAME, 0);
		SharedPreferences.Editor editor = settings.edit();
		editor.putInt(key, value);
		editor.commit();
	}

	public static void setFloatForKey(String key, float value) {
		SharedPreferences settings = ((Activity) sContext)
				.getSharedPreferences(HLHelper.PREFS_NAME, 0);
		SharedPreferences.Editor editor = settings.edit();
		editor.putFloat(key, value);
		editor.commit();
	}

	public static void setDoubleForKey(String key, double value) {
		// SharedPreferences doesn't support recording double value
		SharedPreferences settings = ((Activity) sContext)
				.getSharedPreferences(HLHelper.PREFS_NAME, 0);
		SharedPreferences.Editor editor = settings.edit();
		editor.putFloat(key, (float) value);
		editor.commit();
	}

	public static void setStringForKey(String key, String value) {
		SharedPreferences settings = ((Activity) sContext)
				.getSharedPreferences(HLHelper.PREFS_NAME, 0);
		SharedPreferences.Editor editor = settings.edit();
		editor.putString(key, value);
		editor.commit();
	}

	// 安装apk
	public static void installApk(String url) {
		File apkfile = new File(url);
		if (!apkfile.exists()) {
			return;
		}
		Intent installIntent = new Intent(Intent.ACTION_VIEW);
		installIntent.setDataAndType(Uri.fromFile(apkfile),
				"application/vnd.android.package-archive");
		sContext.startActivity(installIntent);
	}

	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================

	public static interface HLHelperListener {
		public void showEditTextDialog(final String pTitle,
				final String pMessage, final int pInputMode,
				final int pInputFlag, final int pReturnType,
				final int pMaxLength, final boolean multiLine, final String digitsFilter);

		public void runOnGLThread(final Runnable pRunnable);
	}

	public static void preloadBackgroundMusic(final String pPath) {
		HLHelper.sHLMusic.preloadBackgroundMusic(pPath);
	}

	public static void playBackgroundMusic(final String pPath,
			final boolean isLoop) {
		HLHelper.sHLMusic.playBackgroundMusic(pPath, isLoop);
	}

	public static void resumeBackgroundMusic() {
		HLHelper.sHLMusic.resumeBackgroundMusic();
	}

	public static void pauseBackgroundMusic() {
		HLHelper.sHLMusic.pauseBackgroundMusic();
	}

	public static void stopBackgroundMusic() {
		HLHelper.sHLMusic.stopBackgroundMusic();
	}

	public static void rewindBackgroundMusic() {
		HLHelper.sHLMusic.rewindBackgroundMusic();
	}

	public static boolean isBackgroundMusicPlaying() {
		return HLHelper.sHLMusic.isBackgroundMusicPlaying();
	}

	public static float getBackgroundMusicVolume() {
		return HLHelper.sHLMusic.getBackgroundVolume();
	}

	public static void setBackgroundMusicVolume(final float volume) {
		HLHelper.sHLMusic.setBackgroundVolume(volume);
	}

	public static void preloadEffect(final String path) {
		HLHelper.sHLSound.preloadEffect(path);
	}

	public static int playEffect(final String path, final boolean isLoop) {
		return HLHelper.sHLSound.playEffect(path, isLoop);
	}

	public static void resumeEffect(final int soundId) {
		HLHelper.sHLSound.resumeEffect(soundId);
	}

	public static void pauseEffect(final int soundId) {
		HLHelper.sHLSound.pauseEffect(soundId);
	}

	public static void stopEffect(final int soundId) {
		HLHelper.sHLSound.stopEffect(soundId);
	}

	public static float getEffectsVolume() {
		return HLHelper.sHLSound.getEffectsVolume();
	}

	public static void setEffectsVolume(final float volume) {
		HLHelper.sHLSound.setEffectsVolume(volume);
	}

	public static void unloadEffect(final String path) {
		HLHelper.sHLSound.unloadEffect(path);
	}

	public static void pauseAllEffects() {
		HLHelper.sHLSound.pauseAllEffects();
	}

	public static void resumeAllEffects() {
		HLHelper.sHLSound.resumeAllEffects();
	}

	public static void stopAllEffects() {
		HLHelper.sHLSound.stopAllEffects();
	}

	public static void end() {
		HLHelper.sHLMusic.end();
		HLHelper.sHLSound.end();
	}

	/**
	 * dip转px
	 */
	public static int dip2px(Context context, float dipValue) {
		float scale = context.getResources().getDisplayMetrics().density;
		return (int) (dipValue * scale + 0.5f);
	}

	/**
	 * px转dip
	 */
	public static int px2dip(Context context, float pxValue) {
		float scale = context.getResources().getDisplayMetrics().density;
		return (int) (pxValue / scale + 0.5f);
	}
}
