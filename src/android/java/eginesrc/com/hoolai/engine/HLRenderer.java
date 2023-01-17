package com.hoolai.engine;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.pm.ActivityInfo;
import android.opengl.GLSurfaceView;

public class HLRenderer implements GLSurfaceView.Renderer {

	public static interface HLRendererInitCallback {
		void onRendererInit();
	}

	// ===========================================================
	// Constants
	// ===========================================================

	private final static long NANOSECONDSPERSECOND = 1000000000L;
	private final static long NANOSECONDSPERMICROSECOND = 1000000;

	private static long sAnimationInterval = (long) (1.0 / 60 * HLRenderer.NANOSECONDSPERSECOND);

	// ===========================================================
	// Fields
	// ===========================================================

	private long mLastTickInNanoSeconds;
	private int mScreenWidth;
	private int mScreenHeight;
	private boolean inited = false;

	// ===========================================================
	// Constructors
	// ===========================================================

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	public HLRendererInitCallback initCallback = null;

	public static void setAnimationInterval(final double pAnimationInterval) {
		HLRenderer.sAnimationInterval = (long) (pAnimationInterval * HLRenderer.NANOSECONDSPERSECOND);
	}

	public void setScreenWidthAndHeight(final int pSurfaceWidth,
			final int pSurfaceHeight) {
		int orientation = ((HLActivity) HLActivity.getContext())
				.getRequestedOrientation();
		if (orientation == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE) {
			if (pSurfaceWidth < pSurfaceHeight) {
				this.mScreenWidth = pSurfaceHeight;
				this.mScreenHeight = pSurfaceWidth;
			} else {
				this.mScreenWidth = pSurfaceWidth;
				this.mScreenHeight = pSurfaceHeight;
			}
		} else if (orientation == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT) {
			if (pSurfaceWidth > pSurfaceHeight) {
				this.mScreenWidth = pSurfaceHeight;
				this.mScreenHeight = pSurfaceWidth;
			} else {
				this.mScreenWidth = pSurfaceWidth;
				this.mScreenHeight = pSurfaceHeight;
			}
		} else {
			this.mScreenWidth = pSurfaceWidth;
			this.mScreenHeight = pSurfaceHeight;
		}
	}

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	@Override
	public void onSurfaceCreated(final GL10 pGL10, final EGLConfig pEGLConfig) {
		if (initCallback != null)
			initCallback.onRendererInit();
		HLRenderer.nativeInit(this.mScreenWidth, this.mScreenHeight);
		this.mLastTickInNanoSeconds = System.nanoTime();
		inited = true;
	}

	@Override
	public void onSurfaceChanged(final GL10 pGL10, final int pWidth,
			final int pHeight) {
	}

	@Override
	public void onDrawFrame(final GL10 gl) {
		/*
		 * FPS controlling algorithm is not accurate, and it will slow down FPS
		 * on some devices. So comment FPS controlling code.
		 */

		/*
		 * final long nowInNanoSeconds = System.nanoTime(); final long interval
		 * = nowInNanoSeconds - this.mLastTickInNanoSeconds;
		 */

		// should render a frame when onDrawFrame() is called or there is a
		// "ghost"
		HLRenderer.nativeRender();

		/*
		 * // fps controlling if (interval < HLRenderer.sAnimationInterval) {
		 * try { // because we render it before, so we should sleep twice time
		 * interval Thread.sleep((HLRenderer.sAnimationInterval - interval) /
		 * HLRenderer.NANOSECONDSPERMICROSECOND); } catch (final Exception e) {
		 * } }
		 * 
		 * this.mLastTickInNanoSeconds = nowInNanoSeconds;
		 */
	}

	// ===========================================================
	// Methods
	// ===========================================================

	private static native void nativeTouchesBegin(final int[] pIDs,
			final float[] pXs, final float[] pYs);

	private static native void nativeTouchesEnd(final int[] pIDs,
			final float[] pXs, final float[] pYs);

	private static native void nativeTouchesMove(final int[] pIDs,
			final float[] pXs, final float[] pYs);

	private static native void nativeTouchesCancel(final int[] pIDs,
			final float[] pXs, final float[] pYs);

	private static native boolean nativeKeyDown(final int pKeyCode);

	private static native void nativeRender();

	private static native void nativeInit(final int pWidth, final int pHeight);

	private static native void nativeOnPause();

	private static native void nativeOnResume();

	public void handleActionDown(final int[] pIDs, final float[] pXs,
			final float[] pYs) {
		HLRenderer.nativeTouchesBegin(pIDs, pXs, pYs);
	}

	public void handleActionUp(final int[] pIDs, final float[] pXs,
			final float[] pYs) {
		HLRenderer.nativeTouchesEnd(pIDs, pXs, pYs);
	}

	public void handleActionCancel(final int[] pIDs, final float[] pXs,
			final float[] pYs) {
		HLRenderer.nativeTouchesCancel(pIDs, pXs, pYs);
	}

	public void handleActionMove(final int[] pIDs, final float[] pXs,
			final float[] pYs) {
		HLRenderer.nativeTouchesMove(pIDs, pXs, pYs);
	}

	public void handleKeyDown(final int pKeyCode) {
		HLRenderer.nativeKeyDown(pKeyCode);
	}

	public void handleOnPause() {
		System.out.println("handleOnPause");
		System.out.println(inited);
		if (inited)
			HLRenderer.nativeOnPause();
	}

	public void handleOnResume() {
		System.out.println("handleOnResume");
		System.out.println(inited);
		if (inited)
			HLRenderer.nativeOnResume();
	}

	private static native void nativeInsertText(final String pText);

	private static native void nativeDeleteBackward();

	private static native String nativeGetContentText();
	
	private static native void textInputResignActive();

	public void handleInsertText(final String pText) {
		HLRenderer.nativeInsertText(pText);
	}

	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================
}
