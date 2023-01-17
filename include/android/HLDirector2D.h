//
//  HLDirector2D.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-5-23.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLDIRECTOR2D_H__
#define __HLDIRECTOR2D_H__

#include "GameEngineConfig.h"

#if GAME_PROJECTION == PROJECTION_3D

#else

#include "HLGeometry.h"
#include <vector>
#include "HLSceneManager.h"
#include "HLGL.h"
#include "HLSingleton.h"
#if defined(TARGET_WIN32)
#include "hltime.h"
#else
#include <sys/time.h>
#endif

NS_HL_BEGIN

class HLDirector2D: public HLSingleton<HLDirector2D>
{
    friend class HLSingleton<HLDirector2D>;
private:
#ifdef TARGET_IOS
    float mContentScaleFactor;
#endif
    GLint mViewPortX;
    GLint mViewPortY;
    GLint mViewPortWidth;
    GLint mViewPortHeight;
    bool mInvalid;
    bool mShowStats;
    std::vector<HLSceneManager*> mSceneMgrStack;
    std::vector<HLSceneManager*> mSceneToDestroy;
    HLSceneManager* mNextSceneMgr;
    HLSceneManager* mCurrentSceneMgr;
    HLSize mDesignWinSize;
    
    HLSize mWinSize;
    HLSize mWinSizeInPixels;
    
    struct timeval mLastUpdate;
    float mDeltaTime;
    bool mNextDeltaTimeZero;
    
    bool mPause;
    
    HLDirector2D();
    
    void registerComponents();
    
    void drawScene();
    
    void setProjection();
    
    void calculateDeltaTime()
    {
        struct timeval now;
        
        gettimeofday(&now,  0);
        
        // new delta time
        if (mNextDeltaTimeZero)
        {
            mDeltaTime = 0;
            mNextDeltaTimeZero = false;
//#ifdef DEBUG
//        // If we are debugging our code, prevent big delta time
//        if(_deltaTime > 0.2f)
//        {
//            _deltaTime = 1 / 60.0f;
//        }
//#endif

        }
        else
        {
            mDeltaTime = (now.tv_sec - mLastUpdate.tv_sec) + (now.tv_usec - mLastUpdate.tv_usec) / 1000000.0f;
            mDeltaTime = mDeltaTime<0?0:mDeltaTime;
        }
        
        
        mLastUpdate = now;
    }
    
    void showStats();
    
public:
    CMultiDelegate0 onRenderBegin;
    CMultiDelegate0 onRenderEnd;
    
    void setDesignWinSize(const HLSize &size)
    {
        mDesignWinSize = size;
        setProjection();
    }
    
    void setDesignWinWidth(float width)
    {
        setDesignWinSize(HLSize(width, 0));
    }
    
    void setDesignWinHeight(float height)
    {
        setDesignWinSize(HLSize(0, height));
    }
    
    HLPoint convertToGLPoint(const HLPoint &p) const;
    HLPoint convertToScreenPoint(const HLPoint &p) const;
    
    float getWinSizeScale() const;
    
    void pause();
    void resume();
    
    void setDisplayStats(bool b)
    {
        mShowStats = b;
    }
    
    void mainLoop(void);
    
    HLSceneManager* getCurrentSceneManager()
    {
        return mCurrentSceneMgr;
    }
    
    void runWithSceneManager(HLSceneManager *scene)
    {
        HLASSERT(scene, "Running scene should not be null");
        HLASSERT(!mCurrentSceneMgr, "Runing runWithSceneManager method twice");
        pushSceneManager(scene);
    }
    
    void setContentScaleFactor(float scaleFactor)
    {
#ifdef TARGET_IOS
        mContentScaleFactor = scaleFactor;
#else
#endif
    }
    
	float getContentScaleFactor(void)
    {
#ifdef TARGET_IOS
        return mContentScaleFactor;
#else
        return 1;
#endif
    }
    
    void setAnimationInterval(double dValue);
    
    void startAnimation()
    {
        mInvalid = false;
//        gettimeofday(&mLastUpdate,  0);
    }
    
    void stopAnimation()
    {
        mInvalid = true;
    }
    
    HLSize getWinSize() const
    {
//        if (mDesignWinSize.width != 0 && mDesignWinSize.height != 0)
//        {
//            return mDesignWinSize;
//        }
//        return mWinSizeInPixels*getWinSizeScale();
        return mWinSize;
    }
    
    const HLSize& getWinSizeInPixels() const
    {
        return mWinSizeInPixels;
    }
    
    void reshapeProjection(const HLSize& winSize)
    {
        mWinSizeInPixels = winSize;
        mWinSize = winSize;
        setProjection();
        setGLDefaultValues();
    }
    
    void setViewPort()
    {
        glViewport(mViewPortX, mViewPortY, mViewPortWidth, mViewPortHeight);
    }
    
    void setGLDefaultValues(void)
    {
        hlglEnable(GL_BLEND);
        hlglBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        hlglDisable(GL_DEPTH_TEST);
        
        // set other opengl default values
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    void pushSceneManager(HLSceneManager *scene);
    
    void replaceSceneManager(HLSceneManager *scene);
    
    void popAndReplaceSceneManager(HLSceneManager *scene);
    
    void popSceneManager();
    
    void setNextSceneManager();
    
};

NS_HL_END

#endif

#endif
