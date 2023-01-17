package com.hoolai.engine;

import java.io.File;
import java.io.FileInputStream;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.util.Log;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;

public class HLMusic implements OnBufferingUpdateListener, OnCompletionListener,
MediaPlayer.OnPreparedListener ,OnErrorListener{
	// ===========================================================
	// Constants
	// ===========================================================

	private static final String TAG = HLMusic.class.getSimpleName();

	// ===========================================================
	// Fields
	// ===========================================================

	private final Context mContext;
	private MediaPlayer mBackgroundMediaPlayer;
	private float mLeftVolume;
	private float mRightVolume;
	private boolean mPaused;
	private String mCurrentPath;
	private boolean isNewCreateMp = false;
    

	// ===========================================================
	// Constructors
	// ===========================================================

	public HLMusic(final Context pContext) {
		this.mContext = pContext;

		this.initData();
	}

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	// ===========================================================
	// Methods
	// ===========================================================

	public void preloadBackgroundMusic(final String pPath) {
		/*Log.i(HLMusic.TAG, "preloadBackgroundMusic==dPath:" + pPath);
		if ((this.mCurrentPath == null) || (!this.mCurrentPath.equals(pPath))) {
			// preload new background music

			// release old resource and create a new one
			Log.i(HLMusic.TAG, "preloadBackgroundMusic_AAA");
			if (this.mBackgroundMediaPlayer != null) {
				this.mBackgroundMediaPlayer.release();
			}
			Log.i(HLMusic.TAG, "preloadBackgroundMusic_BBB");
			this.mBackgroundMediaPlayer = this.createMediaplayer(pPath);
			Log.i(HLMusic.TAG, "preloadBackgroundMusic_CCC");

			// record the path
			this.mCurrentPath = pPath;
			Log.i(HLMusic.TAG, "preloadBackgroundMusic_DDD");
		}*/
	}

	

	public void playBackgroundMusic(final String pPath, final boolean isLoop) {
	
		if (this.mCurrentPath == null) {
			// it is the first time to play background music or end() was called
			this.mBackgroundMediaPlayer = this.createMediaplayer(pPath);
			this.mCurrentPath = pPath;
			isNewCreateMp = true;
		} else {
			if (!this.mCurrentPath.equals(pPath)) {
				// play new background music

				// release old resource and create a new one
				if (this.mBackgroundMediaPlayer != null) {
					this.mBackgroundMediaPlayer.release();
				}
				this.mBackgroundMediaPlayer = this.createMediaplayer(pPath);
				isNewCreateMp = true;
				// record the path
				this.mCurrentPath = pPath;
			} else {
				isNewCreateMp = false;
			}
		}

		if (this.mBackgroundMediaPlayer == null) {
			Log.e(HLMusic.TAG,
					"playBackgroundMusic: background media player is null");
		} else {
			// if the music is playing or paused, stop it

			try {
			
				if(!this.mBackgroundMediaPlayer.isPlaying()){
					this.mBackgroundMediaPlayer.setLooping(isLoop);
						
				if(isNewCreateMp){				
						this.mBackgroundMediaPlayer.prepareAsync();
						this.mBackgroundMediaPlayer.setOnPreparedListener(this);
					
				} else {	
						if (this.mBackgroundMediaPlayer != null) {
							this.mBackgroundMediaPlayer.release();
						}
						this.mBackgroundMediaPlayer = this.createMediaplayer(pPath);
						this.mBackgroundMediaPlayer.setLooping(isLoop);
						this.mBackgroundMediaPlayer.prepareAsync();
						this.mBackgroundMediaPlayer.setOnPreparedListener(this);
						
				}	
					
			}
			
				this.mPaused = false;
			} catch (final Exception e) {
				Log.e(HLMusic.TAG, "playBackgroundMusic: error state");
			}
		}
	}

	public void stopBackgroundMusic() {
		if (this.mBackgroundMediaPlayer != null) {
			this.mBackgroundMediaPlayer.stop();

			// should set the state, if not, the following sequence will be
			// error
			// play -> pause -> stop -> resume
			this.mPaused = false;
		}
	}

	public void pauseBackgroundMusic() {
		if (this.mBackgroundMediaPlayer != null
				&& this.mBackgroundMediaPlayer.isPlaying()) {
			this.mBackgroundMediaPlayer.pause();
			this.mPaused = true;
		
		}
	}

	public void resumeBackgroundMusic() {
		if (this.mBackgroundMediaPlayer != null && this.mPaused) {
			this.mBackgroundMediaPlayer.start();
			this.mPaused = false;

		}
	}

	public void rewindBackgroundMusic() {
		if (this.mBackgroundMediaPlayer != null) {
			this.mBackgroundMediaPlayer.stop();

			try {
				this.mBackgroundMediaPlayer.prepare();
				this.mBackgroundMediaPlayer.seekTo(0);
				this.mBackgroundMediaPlayer.start();
				
				this.mPaused = false;
				
			} catch (final Exception e) {
				Log.e(HLMusic.TAG, "rewindBackgroundMusic: error state");
			}
		}
	}

	public boolean isBackgroundMusicPlaying() {
		boolean ret = false;

		if (this.mBackgroundMediaPlayer == null) {
			ret = false;
		} else {
			ret = this.mBackgroundMediaPlayer.isPlaying();
		}

		return ret;
	}

	public void end() {
		if (this.mBackgroundMediaPlayer != null) {
			this.mBackgroundMediaPlayer.release();
		}

		this.initData();
	}

	public float getBackgroundVolume() {
		if (this.mBackgroundMediaPlayer != null) {
			return (this.mLeftVolume + this.mRightVolume) / 2;
		} else {
			return 0.0f;
		}
	}

	public void setBackgroundVolume(float pVolume) {
		if (pVolume < 0.0f) {
			pVolume = 0.0f;
		}

		if (pVolume > 1.0f) {
			pVolume = 1.0f;
		}

		this.mLeftVolume = this.mRightVolume = pVolume;
		if (this.mBackgroundMediaPlayer != null) {
			this.mBackgroundMediaPlayer.setVolume(this.mLeftVolume,
					this.mRightVolume);
		}
	}

	private void initData() {
		this.mLeftVolume = 0.5f;
		this.mRightVolume = 0.5f;
		this.mBackgroundMediaPlayer = null;
		this.mPaused = false;
		this.mCurrentPath = null;
	}

	/**
	 * create mediaplayer for music
	 * 
	 * @param pPath
	 *            the pPath relative to assets
	 * @return
	 */
	private MediaPlayer createMediaplayer(final String pPath) {
		Log.i(HLMusic.TAG, "createMediaplayer: " + pPath);
		MediaPlayer mediaPlayer = null;
		try {
				mediaPlayer = new MediaPlayer();
				mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
		        mediaPlayer.setOnBufferingUpdateListener(this);
		        mediaPlayer.setOnCompletionListener(this);  	
				
			if (pPath.startsWith("/")) {
				final FileInputStream fis = new FileInputStream(pPath);
				mediaPlayer.setDataSource(fis.getFD());
				fis.close();
			} else {
				File file = new File(HLHelper.getWritablePath() + pPath);
				if (file.exists()) {
					final FileInputStream fis = new FileInputStream(file);
					mediaPlayer.setDataSource(fis.getFD());
					fis.close();
				} else {
					final AssetFileDescriptor assetFileDescritor = this.mContext
							.getAssets().openFd(pPath);
					mediaPlayer.setDataSource(
							assetFileDescritor.getFileDescriptor(),
							assetFileDescritor.getStartOffset(),
							assetFileDescritor.getLength());
				}
			}

			
			mediaPlayer.setVolume(this.mLeftVolume, this.mRightVolume);
			
		} catch (final Exception e) {
			mediaPlayer = null;
			Log.e(HLMusic.TAG, "error: " + e.getMessage(), e);
		}

		Log.i(HLMusic.TAG, "createMediaplayer: bbbbb");
		return mediaPlayer;
		
	}

	@Override
	public boolean onError(MediaPlayer arg0, int arg1, int arg2) {
		// TODO Auto-generated method stub
	    Log.e(HLMusic.TAG, "onError=arg0:" + arg0 + "==arg1:" + arg1 + "==arg2:" + arg2);
		return false;
	}

	@Override
	public void onPrepared(MediaPlayer arg0) {
		// TODO Auto-generated method stub
		Log.i(HLMusic.TAG,"onPrepared==mediaPlayer_arg0:" + arg0);
		if(arg0 == this.mBackgroundMediaPlayer){	
			this.mBackgroundMediaPlayer.start();
			this.mPaused = false;
		}
		
	}

	@Override
	public void onCompletion(MediaPlayer arg0) {
		// TODO Auto-generated method stub
		Log.e(HLMusic.TAG, "onCompletion");
	}

	@Override
	public void onBufferingUpdate(MediaPlayer arg0, int arg1) {
		// TODO Auto-generated method stub
		Log.i(HLMusic.TAG, "onBufferingUpdate");
	}

	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================
}
