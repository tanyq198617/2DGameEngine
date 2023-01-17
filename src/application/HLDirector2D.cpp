//
//  HLDirector2D.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-23.
//  Copyright (c) 2012 ICT. All rights reserved.
//


#include "HLDirector2D.h"
#include "HLGUIManager.h"

#if GAME_PROJECTION == PROJECTION_3D

#else

#include "HLApplication.h"
#include "HLAutoreleasePool.h"
#include "HLGLMatrixStack.h"
#include "HLFileUtil.h"
#include "HLClassRegister.h"
#include "HLTransform2DComponent.h"
#include "HLSpriteComponent.h"
#include "HLCamera2DComponent.h"
#include "HLBMFontComponent.h"
#include "HLColorComponent.h"
#include "HLColorAdvanceComponent.h"
#include "HLGUIViewComponent.h"
#include "HLBlendFuncComponent.h"
#include "HLShaderComponent.h"
#include "HLPolygonHitTestComponent.h"
#include "HLClippingComponent.h"
#include "HLScissorComponent.h"
#include "HLSkeletonComponent.h"
#include "HLBatchComponent.h"
#include "HLShaderComponent.h"
#include "HLPrimitiveComponent.h"
#include "HLAnimateSystem.h"
#include "HLLabel.h"
#include "HLTexture.h"

#include "HLTimer.h"

#include <OpenThreads/Mutex>

#include "HLFont.h"

#include "HLGUIManager.h"

#include <pthread.h>


pthread_t mainthread;

extern bool appWillTerminate;

NS_HL_BEGIN

std::vector<ThreadScheduleObject> pendingScheduleArray;
//pthread_mutex_t scheduleSelectorMutex;
OpenThreads::Mutex scheduleSelectorMutex;

extern std::map<std::string, HLFont*> fontinstances;

HLDirector2D::HLDirector2D():
#ifdef TARGET_IOS
mContentScaleFactor(1),
#endif
mShowStats(false),
mCurrentSceneMgr(NULL),
mNextSceneMgr(NULL)
{
    mNextDeltaTimeZero = true;
    registerComponents();
}

void HLDirector2D::registerComponents()
{
    REGISTER_CLASS(HLTransform2DComponent);
    REGISTER_CLASS(HLSpriteComponent);
    REGISTER_CLASS(HLPolygonHitTestComponent);
    REGISTER_CLASS(HLSkeletonComponent);
    REGISTER_CLASS(HLCamera2DComponent);
    REGISTER_CLASS(HLGUIViewComponent);
    REGISTER_CLASS(HLBMFontComponent);
    REGISTER_CLASS(HLColorComponent);
    REGISTER_CLASS(HLColorAdvanceComponent);
    REGISTER_CLASS(HLBlendFuncComponent);
    REGISTER_CLASS(HLSkeletonComponent);
    REGISTER_CLASS(HLBatchComponent);
    REGISTER_CLASS(HLClippingComponent);
    REGISTER_CLASS(HLScissorComponent);
    REGISTER_CLASS(HLShaderComponent);
    REGISTER_CLASS(HLPrimitiveComponent);
}

void HLDirector2D::setProjection()
{
    if (mDesignWinSize == HLSizeZero)
    {
        mViewPortX = mViewPortY = 0;
        mViewPortWidth = mWinSizeInPixels.width;
        mViewPortHeight = mWinSizeInPixels.height;
    }
    else
    {
        float scale;
        if (mDesignWinSize.width != 0 && mDesignWinSize.height != 0) // scale aspect ratio
        {
            scale = MAX(mDesignWinSize.width/mWinSizeInPixels.width, mDesignWinSize.height/mWinSizeInPixels.height);
            mWinSize = mDesignWinSize;
        }
        else if (mDesignWinSize.width == 0) // width fill and heigth scale
        {
            scale = mDesignWinSize.height/mWinSizeInPixels.height;
            mWinSize = mWinSizeInPixels * scale;
        }
        else if (mDesignWinSize.height == 0) // height fill and width scale
        {
            scale = mDesignWinSize.width/mWinSizeInPixels.width;
            mWinSize = mWinSizeInPixels * scale;
        }
        HLSize realsize = mWinSize / scale;
        mViewPortX = (mWinSizeInPixels.width-realsize.width)/2;
        mViewPortY = (mWinSizeInPixels.height-realsize.height)/2;
        mViewPortWidth = realsize.width;
        mViewPortHeight = realsize.height;
    }
    setViewPort();
    HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
    matrixStack->matrixMode(KM_GL_PROJECTION);
    matrixStack->loadIdentity();
    kmMat4 orthoMatrix;
#ifdef NVCARD
    kmMat4OrthographicProjection(&orthoMatrix, -0.01, mWinSize.width-0.01, -0.01, mWinSize.height-0.01, -1024, 1024);
#else
    kmMat4OrthographicProjection(&orthoMatrix, 0, mWinSize.width, 0, mWinSize.height, -1024, 1024);
#endif
    matrixStack->multMatrix(&orthoMatrix);
    matrixStack->matrixMode(KM_GL_MODELVIEW);
    matrixStack->loadIdentity();

//    gui::HLGUIManager::getSingleton()->onWinSizeChanged();
}

void HLDirector2D::setAnimationInterval(double dValue)
{
    HLApplication::sharedApplication()->setAnimationInterval(dValue);
}

void HLDirector2D::mainLoop()
{
    if (appWillTerminate)
        return;
    
    if (mInvalid)
        return;
    
	static bool initialized = false;
	if (!initialized)
	{
		initialized = true;
		mainthread = pthread_self();
	}
    
    if(!scheduleSelectorMutex.trylock())
    {
        std::vector<ThreadScheduleObject>::iterator itr;
        for (itr = pendingScheduleArray.begin(); itr!=pendingScheduleArray.end(); itr++)
        {
            ThreadScheduleObject ob = *itr;
            (ob.target->*(ob.selector))(ob.object);
            ob.target->release();
            if (ob.object)
            {
                ob.object->release();
            }
        }
        pendingScheduleArray.clear();
        scheduleSelectorMutex.unlock();
    }
    
    // update
//    if (!mInvalid)
    {
        drawScene();
        HLAutoreleasePool::currentPool()->purgePool();
        HLTexture::glPurgeTextures();
    }
}

void HLDirector2D::pause()
{
    mPause = true;
}

void HLDirector2D::resume()
{
    if (!mPause)
    {
        return;
    }
    mPause = false;
    calculateDeltaTime();
}

void HLDirector2D::drawScene()
{
    calculateDeltaTime();
    
    if (mNextSceneMgr)
    {
        setNextSceneManager();
    }
    
    if (!mPause)
    {
        HLTimerManager::getSingleton()->update(mDeltaTime);
        HLTweenSystem::update(mDeltaTime);
        HLAnimateSystem::update(mDeltaTime);
        gui::HLView::update(mDeltaTime);
        if (mCurrentSceneMgr)
        {
            mCurrentSceneMgr->update(mDeltaTime);
        }
    }
    
    for (std::map<std::string, HLFont*>::iterator itr = fontinstances.begin(); itr != fontinstances.end(); ++itr)
    {
        itr->second->updateTexture();
    }
    
    onRenderBegin();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
    
    matrixStack->pushMatrix();
    if (mCurrentSceneMgr)
    {
        mCurrentSceneMgr->draw();
    }
    matrixStack->popMatrix();
    
    matrixStack->pushMatrix();
    gui::HLGUIManager::getSingleton()->drawFrame();
    matrixStack->popMatrix();
    onRenderEnd();
    
    if (mShowStats)
    {
        showStats();
    }
    
    HLApplication::sharedApplication()->swapBuffers();
}

void HLDirector2D::showStats()
{
    static unsigned int frames = 0;
    static float accDt = 0;
    frames++;
    accDt += mDeltaTime;
    
    static hoolai::gui::HLLabel* label = NULL;
    if (!label)
    {
        label = new hoolai::gui::HLLabel(NULL);
        label->setTextStroke(true);
        label->setFontSize(12);
        label->setSize(HLSize(100, 15));
    }
    if (accDt >= 0.2)
    {
        char fpschar[10];
        sprintf(fpschar, "%.1f", frames/accDt);
        label->setText(fpschar);
        label->setTextAlignment(hoolai::gui::kTextHAlignmentLeft | hoolai::gui::kTextVAlignmentBottom);
        frames = 0;
        accDt = 0;
    }
    
    label->onDraw();
    
//    if (font)
//    {
//        font->testdraw(fps, 12);
//    }
//    else
//    {
//        font = HLFont::getFont();
//    }
    
//    const char* font = "MSYH.hlf";
//    HLFont::getFont(font)->addText("1234567890. 字体测试");
//    kmGLTranslatef(10, 400, 0);
//    HLFont::getFont(font)->testdraw("字体测试", 12);
//    kmGLTranslatef(0, -20, 0);
//    HLFont::getFont(font)->testdraw("字体测试", 16);
//    kmGLTranslatef(0, -24, 0);
//    HLFont::getFont(font)->testdraw("字体测试", 20);
//    kmGLTranslatef(0, -28, 0);
//    HLFont::getFont(font)->testdraw("字体测试", 24);
//    kmGLTranslatef(0, -32, 0);
//    HLFont::getFont(font)->testdraw("字体测试", 28);
//    kmGLTranslatef(0, -36, 0);
//    HLFont::getFont(font)->testdraw("字体测试", 32);

//    HLFont::getFont("MSYH.hlf")->testdraw(fps.c_str(), 12);
//    kmGLTranslatef(10, 10, 0);
//    HLFont::getFont("simhei.hlf")->testdraw("123", 12);
    
}

HLPoint HLDirector2D::convertToGLPoint(const HLPoint &p) const
{
    if (mDesignWinSize == HLSizeZero)
    {
        return HLPoint(p.x, mWinSizeInPixels.height - p.y);
    }
    else
    {
        float scale = getWinSizeScale();
        HLSize winSize = getWinSize()/scale;
        HLSize winSize1 = mWinSizeInPixels;
        HLPoint res = p;
        res.y = mWinSizeInPixels.height - p.y;
        res.x -= (winSize1.width - winSize.width) / 2;
        res.y -= (winSize1.height - winSize.height) / 2;
        return res*scale;
    }
}

HLPoint HLDirector2D::convertToScreenPoint(const HLPoint &p) const
{
    if (mDesignWinSize == HLSizeZero)
    {
        return HLPoint(p.x, mWinSizeInPixels.height - p.y);
    }
    else
    {
        float scale = getWinSizeScale();
        HLSize winSize = getWinSize()/scale;
        HLSize winSize1 = mWinSizeInPixels;
        HLPoint res = p;
        res /= scale;
        res.x += (winSize1.width - winSize.width) / 2;
        res.y += (winSize1.height - winSize.height) / 2;
        res.y = mWinSizeInPixels.height - res.y;
        return res / CONTENT_SCALE_FACTOR;
    }
}

float HLDirector2D::getWinSizeScale() const
{
    float scale = 1.f;
    if (mDesignWinSize == HLSizeZero)
    {
        return 1.f;
    }
    if (mDesignWinSize.width != 0 && mDesignWinSize.height != 0)
    {
        scale = MAX(mDesignWinSize.width/mWinSizeInPixels.width, mDesignWinSize.height/mWinSizeInPixels.height);
    }
    else if (mDesignWinSize.width == 0) // width fill and heigth scale
    {
        scale = mDesignWinSize.height/mWinSizeInPixels.height;
    }
    else if (mDesignWinSize.height == 0) // height fill and width scale
    {
        scale = mDesignWinSize.width/mWinSizeInPixels.width;
    }
    return scale;
}

void HLDirector2D::pushSceneManager(HLSceneManager *scene)
{
    HLASSERT(scene, "scene should not be null");
    //        if (mSceneMgrStack.begin() != mSceneMgrStack.end())
    //        {
    //            HLSceneManager* mgr = mSceneMgrStack.back();
    //            mgr->onDeactive(mgr);
    //        }
    mSceneMgrStack.push_back(scene);
    mNextSceneMgr = scene;
}

void HLDirector2D::replaceSceneManager(HLSceneManager *scene)
{
    HLASSERT(scene, "scene should not be null");
    HLASSERT(mCurrentSceneMgr, "running scene should not be null");
    //        HLSceneManager *mgr = mSceneMgrStack.back();
    //        mgr->onDestroy(mgr);
    //        delete mgr;
    HLSceneManager* current = mSceneMgrStack.back();
    mSceneMgrStack.pop_back();
    pushSceneManager(scene);
    mSceneToDestroy.push_back(current);
}

void HLDirector2D::popSceneManager()
{
    HLASSERT(mCurrentSceneMgr, "running scene should not be null");
    //        HLSceneManager* mgr = mSceneMgrStack.back();
    //        mgr->onDestroy(mgr);
    //        delete mSceneMgrStack.back();
    //        mSceneMgrStack.pop_back();
    //        mgr = mSceneMgrStack.back();
    //        mgr->onReactive(mgr);
    HLSceneManager* current = mSceneMgrStack.back();
    mSceneMgrStack.pop_back();
    if (mSceneMgrStack.size() == 0)
    {
        // end game
        exit(0);
    }
    mNextSceneMgr = mSceneMgrStack.back();
    //        mCurrentSceneMgr=mSceneMgrStack.back();
    mSceneToDestroy.push_back(current);
}

void HLDirector2D::popAndReplaceSceneManager(HLSceneManager *scene)
{
    HLASSERT(mCurrentSceneMgr, "running scene should not be null");
    mSceneToDestroy.push_back(mSceneMgrStack.back());
    mSceneMgrStack.pop_back();
    if (mSceneMgrStack.size() == 0)
    {
        // end game
        exit(0);
    }
    HLSceneManager* sceneMgr = mSceneMgrStack.back();
    sceneMgr->onDestroy(sceneMgr);
    delete sceneMgr;
    mSceneMgrStack.pop_back();
    pushSceneManager(scene);
}

void HLDirector2D::setNextSceneManager()
{
    if (mCurrentSceneMgr)
    {
        if (std::find(mSceneToDestroy.begin(), mSceneToDestroy.end(), mCurrentSceneMgr) == mSceneToDestroy.end())
        {
            mCurrentSceneMgr->onDeactive(mCurrentSceneMgr);
        }
    }
    
    for (std::vector<HLSceneManager*>::iterator itr = mSceneToDestroy.begin(); itr != mSceneToDestroy.end(); ++itr)
    {
        (*itr)->onDestroy(*itr);
        delete *itr;
    }
    mSceneToDestroy.clear();
    
    mCurrentSceneMgr = mNextSceneMgr;
    mNextSceneMgr = NULL;
    mCurrentSceneMgr->onActive(mCurrentSceneMgr);
}

NS_HL_END

#endif
