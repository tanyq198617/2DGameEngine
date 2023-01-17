package com.hoolai.engine;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.opengl.GLSurfaceView.EGLConfigChooser;
import android.os.Build;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

import com.hoolai.engine.HLHelper.HLHelperListener;

public abstract class HLActivity extends Activity implements HLHelperListener {
	// ===========================================================
	// Constants
	// ===========================================================

	private static final String TAG = HLActivity.class.getSimpleName();

	private HLHandler mHandler;

	// ===========================================================
	// Fields
	// ===========================================================

	protected HLGLSurfaceView mGLSurfaceView;
	private static Context sContext = null;

	public static Context getContext() {
		return sContext;
	}

	// ===========================================================
	// Constructors
	// ===========================================================

	@Override
	protected void onCreate(final Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Log.w("HLActivity--onCreate",
				"_______________________________________________________________________________________________");
		sContext = this;
		this.mHandler = new HLHandler(this);

		this.init();

		HLHelper.init(this, this);
	}

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	@Override
	public void onWindowFocusChanged(final boolean hasWindowFocus) {
		super.onWindowFocusChanged(hasWindowFocus);
		System.out.println("onWindowFocusChanged="+hasWindowFocus);
//		if (hasWindowFocus) {
//			this.mGLSurfaceView.onResume();
//		} else {
//			this.mGLSurfaceView.onPause();
//		}
	}

	@Override
	public void runOnGLThread(final Runnable pRunnable) {
		this.mGLSurfaceView.queueEvent(pRunnable);
	}

	public void runOnJavaMainThread(final Runnable pRunnable) {
		mHandler.post(pRunnable);
	}

	public void showEditTextDialog(final String pTitle, final String pContent,
			final int pInputMode, final int pInputFlag, final int pReturnType,
			final int pMaxLength, final boolean multiLine, final String digitsFilter) {
		Message msg = new Message();
		msg.what = HLHandler.HANDLER_SHOW_EDITBOX_DIALOG;
		msg.obj = new HLHandler.EditBoxMessage(pTitle, pContent, pInputMode,
				pInputFlag, pReturnType, pMaxLength, multiLine, digitsFilter);
		this.mHandler.sendMessage(msg);
	}

	// ===========================================================
	// Methods
	// ===========================================================
	public void init() {

		// FrameLayout
		ViewGroup.LayoutParams framelayout_params = new ViewGroup.LayoutParams(
				ViewGroup.LayoutParams.FILL_PARENT,
				ViewGroup.LayoutParams.FILL_PARENT);
		FrameLayout framelayout = new FrameLayout(this);
		framelayout.setLayoutParams(framelayout_params);

		// HLEditText layout
		ViewGroup.LayoutParams edittext_layout_params = new ViewGroup.LayoutParams(
				ViewGroup.LayoutParams.FILL_PARENT,
				ViewGroup.LayoutParams.WRAP_CONTENT);
		// HLEditText edittext = new HLEditText(this);
		// edittext.setLayoutParams(edittext_layout_params);
		//
		// // ...add to FrameLayout
		// framelayout.addView(edittext);

		// Cocos2dxGLSurfaceView
		this.mGLSurfaceView = this.onCreateView();

		// ...add to FrameLayout
		framelayout.addView(this.mGLSurfaceView);

		// Switch to supported OpenGL (ARGB888) mode on emulator
		// if (isAndroidEmulator())
		this.mGLSurfaceView.setEGLConfigChooser(new ConfigChooser());

		this.mGLSurfaceView.setHLRenderer(new HLRenderer());

		// Set framelayout as the content view
		setContentView(framelayout);
	}

	public HLGLSurfaceView onCreateView() {
		return new HLGLSurfaceView(this);
	}

	private final static boolean isAndroidEmulator() {
		String model = Build.MODEL;
		Log.d(TAG, "model=" + model);
		String product = Build.PRODUCT;
		Log.d(TAG, "product=" + product);
		boolean isEmulator = false;
		if (product != null) {
			isEmulator = product.equals("sdk") || product.contains("_sdk")
					|| product.contains("sdk_");
		}
		Log.d(TAG, "isEmulator=" + isEmulator);
		return isEmulator;
	}

	private static class ConfigChooser implements EGLConfigChooser {
		private int mDepthSize = 4;
		private int mStencilSize = 4;
		private int[] mValue = new int[1];

		private static int EGL_OPENGL_ES2_BIT = 4;
		private static int[] s_configAttribs2 = { EGL10.EGL_RED_SIZE, 4,
				EGL10.EGL_GREEN_SIZE, 4, EGL10.EGL_BLUE_SIZE, 4,
				EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL10.EGL_NONE };

		@Override
		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
			int[] num_config = new int[1];
			egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

			int numConfigs = num_config[0];

			if (numConfigs <= 0) {
				throw new IllegalArgumentException(
						"No configs match configSpec");
			}

			EGLConfig[] configs = new EGLConfig[numConfigs];
			egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs,
					num_config);

			/*
			 * Now return the "best" one
			 */
			return chooseConfig(egl, display, configs);
		}

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
				EGLConfig[] configs) {
			for (EGLConfig config : configs) {
				int d = findConfigAttrib(egl, display, config,
						EGL10.EGL_DEPTH_SIZE, 0);
				int s = findConfigAttrib(egl, display, config,
						EGL10.EGL_STENCIL_SIZE, 0);

				// We need at least mDepthSize and mStencilSize bits
				if (d < mDepthSize || s < mStencilSize)
					continue;

				// We want an *exact* match for red/green/blue/alpha
				int r = findConfigAttrib(egl, display, config,
						EGL10.EGL_RED_SIZE, 0);
				int g = findConfigAttrib(egl, display, config,
						EGL10.EGL_GREEN_SIZE, 0);
				int b = findConfigAttrib(egl, display, config,
						EGL10.EGL_BLUE_SIZE, 0);
				int a = findConfigAttrib(egl, display, config,
						EGL10.EGL_ALPHA_SIZE, 0);

				if (r == 8 && g == 8 && b == 8 && a == 8)
					return config;
				if (r == 5 && g == 6 && b == 5 && a == 0)
					return config;
			}
			return null;
		}

		private int findConfigAttrib(EGL10 egl, EGLDisplay display,
				EGLConfig config, int attribute, int defaultValue) {

			if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
				return mValue[0];
			}
			return defaultValue;
		}
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		System.out.println("HLActivity=onDestroy");
		HLHelper.end();
		// Log.v("HLActivity", "onDestroy");
	}
	@Override
	protected void onStop() {
		// TODO Auto-generated method stub
		System.out.println("HLActivity=OnStop");
//		this.mGLSurfaceView.onPause();
		super.onStop();
	}
	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		//this.mGLSurfaceView.onPause();
		System.out.println("HLActivity=onpause");
		HLHelper.pauseAllEffects();
		HLHelper.pauseBackgroundMusic();
		// Log.v("HLActivity", "onPause");
	}

	@Override
	protected void onPostResume() {
		// TODO Auto-generated method stub
		super.onPostResume();
		HLHelper.resumeBackgroundMusic();
		HLHelper.resumeAllEffects();
		// Log.v("HLActivity", "onPostResume");
	}

	@Override
	protected void onRestart() {
		// TODO Auto-generated method stub
		System.out.println("onRestart==HLActivity");
		super.onRestart();
		// Log.v("HLActivity", "onRestart");
	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		System.out.println("HlActivity==onResume");
		super.onResume();
		//this.mGLSurfaceView.onResume();
		// HLHelper.resumeBackgroundMusic();
		// HLHelper.resumeAllEffects();
		// Log.v("HLActivity", "onResume");
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		// TODO Auto-generated method stub
		super.onActivityResult(requestCode, resultCode, data);
		Log.i("HLActivity - onActivityResult ", "result:" + resultCode + ":");
	}

	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================
}
