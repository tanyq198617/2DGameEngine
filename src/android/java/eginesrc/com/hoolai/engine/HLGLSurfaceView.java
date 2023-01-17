package com.hoolai.engine;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class HLGLSurfaceView extends GLSurfaceView {
	// ===========================================================
	// Constants
	// ===========================================================

	// ===========================================================
	// Fields
	// ===========================================================

	private static HLGLSurfaceView mHLGLSurfaceView;
	
	public static boolean mMultiTouchEnabled = false;

	private HLRenderer mHLRenderer;
	private float touch_gap = 5;

	public HLRenderer getRenderer() {
		return mHLRenderer;
	}

	// ===========================================================
	// Constructors
	// ===========================================================

	public HLGLSurfaceView(final Context context) {
		super(context);

		this.initView();
	}

	public HLGLSurfaceView(final Context context, final AttributeSet attrs) {
		super(context, attrs);

		this.initView();
	}

	protected void initView() {
		this.setEGLContextClientVersion(2);
		this.setFocusableInTouchMode(true);
		touch_gap = HLHelper.dip2px(HLActivity.getContext(), 8);
		System.out.println("touch_gap==" + touch_gap);
		HLGLSurfaceView.mHLGLSurfaceView = this;
	}

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	public static HLGLSurfaceView getInstance() {
		return mHLGLSurfaceView;
	}

	public void setHLRenderer(final HLRenderer renderer) {
		this.mHLRenderer = renderer;
		this.setRenderer(this.mHLRenderer);
	}

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	@Override
	public void onResume() {
		super.onResume();

		this.queueEvent(new Runnable() {
			@Override
			public void run() {
				HLGLSurfaceView.this.mHLRenderer.handleOnResume();
			}
		});
	}

	@Override
	public void onPause() {
		this.queueEvent(new Runnable() {
			@Override
			public void run() {
				HLGLSurfaceView.this.mHLRenderer.handleOnPause();
			}
		});

		// super.onPause();
	}
	
	private class Touch
	{
		public int touch_id;
		public float start_x;
		public float start_y;
		public boolean touch_moved = false;
	}

	private Map<Integer, Touch> touches = new HashMap<Integer, Touch>();
	
	private boolean multitouch_enabled = mMultiTouchEnabled;

	@Override
	public boolean onTouchEvent(final MotionEvent pMotionEvent) {
		// these data are used in ACTION_MOVE and ACTION_CANCEL
		final int pointerNumber = pMotionEvent.getPointerCount();
		final int[] ids = new int[pointerNumber];
		final float[] xs = new float[pointerNumber];
		final float[] ys = new float[pointerNumber];

		for (int i = 0; i < pointerNumber; i++) {
			ids[i] = pMotionEvent.getPointerId(i);
			xs[i] = pMotionEvent.getX(i);
			ys[i] = pMotionEvent.getY(i);
		}

		switch (pMotionEvent.getAction() & MotionEvent.ACTION_MASK) {
		case MotionEvent.ACTION_POINTER_DOWN:
			if (multitouch_enabled)
			{
				int index = pMotionEvent.getActionIndex();
				final int[] ids1 = new int[1];
				final float[] xs1 = new float[1];
				final float[] ys1 = new float[1];
				Touch t = new Touch();
				ids1[0] = t.touch_id = ids[index];
				xs1[0] = t.start_x = xs[index];
				ys1[0] = t.start_y = ys[index];
				touches.put(t.touch_id, t);
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						HLGLSurfaceView.this.mHLRenderer.handleActionDown(ids1, xs1,
								ys1);
					}
				});
			}
				break;
		case MotionEvent.ACTION_DOWN:
			multitouch_enabled = mMultiTouchEnabled;
			touches.clear();
			Touch t = new Touch();
			t.touch_id = ids[0];
			t.start_x = xs[0];
			t.start_y = ys[0];
			touches.put(t.touch_id, t);
			if (multitouch_enabled)
			{
				for (int i = 0; i < pointerNumber; ++i)
				{
					Touch t1 = new Touch();
					t1.touch_id = ids[i];
					t1.start_x = xs[i];
					t1.start_y = ys[i];
					touches.put(t1.touch_id, t1);
				}
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						HLGLSurfaceView.this.mHLRenderer.handleActionDown(ids, xs,
								ys);
					}
				});
			}
			else
			{
				final int[] ids1 = new int[1];
				final float[] xs1 = new float[1];
				final float[] ys1 = new float[1];
				ids1[0] = ids[0];
				xs1[0] = xs[0];
				ys1[0] = ys[0];
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						HLGLSurfaceView.this.mHLRenderer.handleActionDown(ids1, xs1,
								ys1);
					}
				});
			}
			break;

		case MotionEvent.ACTION_MOVE:
			if (multitouch_enabled)
			{
				ArrayList<Integer> indeces = new ArrayList<Integer>();
				for (int i = 0; i < pointerNumber; ++i)
				{
					Touch touch = touches.get(ids[i]);
					if (touch.touch_moved
							|| (Math.abs(touch.start_x - xs[i]) > touch_gap || Math
									.abs(touch.start_y - ys[i]) > touch_gap)) {
						touch.touch_moved = true;
						indeces.add(i);
					}
				}
				final int[] ids1 = new int[indeces.size()];
				final float[] xs1 = new float[indeces.size()];
				final float[] ys1 = new float[indeces.size()];
				for (int i = 0; i < indeces.size(); ++i)
				{
					ids1[i] = ids[indeces.get(i)];
					xs1[i] = xs[indeces.get(i)];
					ys1[i] = ys[indeces.get(i)];
				}
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						HLGLSurfaceView.this.mHLRenderer.handleActionMove(ids1,
								xs1, ys1);
					}
				});
			}
			else
			{
				if (touches.isEmpty())
				{
					break;
				}
				Touch touch = touches.entrySet().iterator().next().getValue();
				final int[] ids1 = new int[1];
				final float[] xs1 = new float[1];
				final float[] ys1 = new float[1];
				int index = pMotionEvent.findPointerIndex(touch.touch_id);
				if (index == -1)
				{
					break;
				}
				ids1[0] = ids[index];
				xs1[0] = xs[index];
				ys1[0] = ys[index];
				if (touch.touch_moved
						|| (Math.abs(touch.start_x - xs[0]) > touch_gap || Math
								.abs(touch.start_y - ys[0]) > touch_gap)) {
					touch.touch_moved = true;
					this.queueEvent(new Runnable() {
						@Override
						public void run() {
							HLGLSurfaceView.this.mHLRenderer.handleActionMove(ids1,
									xs1, ys1);
						}
					});
				}
			}
			break;

		case MotionEvent.ACTION_POINTER_UP:
			if (multitouch_enabled)
			{
				int index = pMotionEvent.getActionIndex();
				touches.remove(ids[index]);
				final int[] ids1 = new int[1];
				final float[] xs1 = new float[1];
				final float[] ys1 = new float[1];
				ids1[0] = ids[index];
				xs1[0] = xs[index];
				ys1[0] = ys[index];
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						HLGLSurfaceView.this.mHLRenderer
								.handleActionUp(ids1, xs1, ys1);
					}
				});
			}
			else
			{
				if (touches.isEmpty())
				{
					break;
				}
				Touch touch = touches.entrySet().iterator().next().getValue();
				int index = pMotionEvent.getActionIndex();
				if (touch.touch_id == ids[index])
				{
					final int[] ids1 = new int[1];
					final float[] xs1 = new float[1];
					final float[] ys1 = new float[1];
					ids1[0] = ids[index];
					xs1[0] = xs[index];
					ys1[0] = ys[index];
					touches.clear();
					this.queueEvent(new Runnable() {
						@Override
						public void run() {
							HLGLSurfaceView.this.mHLRenderer
									.handleActionUp(ids1, xs1, ys1);
						}
					});
				}
			}
				break;

		case MotionEvent.ACTION_UP:
			if (multitouch_enabled)
			{
				for (int i = 0; i < pointerNumber; ++i)
				{
					touches.remove(ids[i]);
				}
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						HLGLSurfaceView.this.mHLRenderer
								.handleActionUp(ids, xs, ys);
					}
				});
			}
			else
			{
				if (touches.isEmpty())
				{
					break;
				}
				Touch touch = touches.entrySet().iterator().next().getValue();
				final int[] ids1 = new int[1];
				final float[] xs1 = new float[1];
				final float[] ys1 = new float[1];
				int index = pMotionEvent.findPointerIndex(touch.touch_id);
				if (index == -1)
				{
					break;
				}
				ids1[0] = ids[index];
				xs1[0] = xs[index];
				ys1[0] = ys[index];
				touches.clear();
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						HLGLSurfaceView.this.mHLRenderer
								.handleActionUp(ids1, xs1, ys1);
					}
				});
			}
			break;

		case MotionEvent.ACTION_CANCEL:
			if (multitouch_enabled)
			{
				for (int i = 0; i < pointerNumber; ++i)
				{
					touches.remove(ids[i]);
				}
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						HLGLSurfaceView.this.mHLRenderer
								.handleActionCancel(ids, xs, ys);
					}
				});
			}
			else
			{
				if (touches.isEmpty())
				{
					break;
				}
				Touch touch = touches.entrySet().iterator().next().getValue();
				final int[] ids1 = new int[1];
				final float[] xs1 = new float[1];
				final float[] ys1 = new float[1];
				int index = pMotionEvent.findPointerIndex(touch.touch_id);
				if (index == -1)
				{
					break;
				}
				ids1[0] = ids[index];
				xs1[0] = xs[index];
				ys1[0] = ys[index];
				touches.clear();
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						HLGLSurfaceView.this.mHLRenderer
								.handleActionCancel(ids1, xs1, ys1);
					}
				});
			}
			break;
		}

		return true;
	}

	/*
	 * This function is called before HLRenderer.nativeInit(), so the width and
	 * height is correct.
	 */
	@Override
	protected void onSizeChanged(final int pNewSurfaceWidth,
			final int pNewSurfaceHeight, final int pOldSurfaceWidth,
			final int pOldSurfaceHeight) {
		if (!this.isInEditMode()) {
			this.mHLRenderer.setScreenWidthAndHeight(pNewSurfaceWidth,
					pNewSurfaceHeight);
		}
	}

	@Override
	public boolean onKeyDown(final int pKeyCode, final KeyEvent pKeyEvent) {
		// switch (pKeyCode) {
		// case KeyEvent.KEYCODE_BACK:
		// // case KeyEvent.KEYCODE_MENU:
		// // this.queueEvent(new Runnable() {
		// // @Override
		// // public void run() {
		// //// HLGLSurfaceView.this.mHLRenderer.handleKeyDown(pKeyCode);
		// // }
		// // });
		// android.os.Process.killProcess(android.os.Process.myPid());
		// return true;
		// default:
		// return super.onKeyDown(pKeyCode, pKeyEvent);
		// }
		return super.onKeyDown(pKeyCode, pKeyEvent);
	}
}
