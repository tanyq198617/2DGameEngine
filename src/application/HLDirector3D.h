//
//  HLDirector3D.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-8-13.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __HLDIRECTOR3D_H__
#define __HLDIRECTOR3D_H__

#include "GameEngineConfig.h"

#if GAME_PROJECTION == PROJECTION_2D

#else

#include "HLGeometry.h"
#include <vector>
#include <sys/time.h>
#include "HLScene.h"
#include "HLGL.h"

NS_HL_BEGIN

class HLDirector3D
{
private:
    float _contentScaleFactor;
    bool _invalid;
    std::vector<HLScene*> _sceneStack;
    HLScene* _nextScene;
    HLScene* _currentScene;
    
    HLSize _winSize;
    HLSize _winSizeInPixels;
    
    struct timeval _lastUpdate;
    float _deltaTime;
    bool _nextDeltaTimeZero;
    
    HLDirector3D();
    
    void registerComponents();
    
    void drawScene();
    
    void setProjection();
    
    void pushScene(HLScene *scene)
    {
        HLASSERT(scene, "scene should not be null");
        _sceneStack.push_back(scene);
        _nextScene = scene;
    }
    
    void replaceScene(HLScene *scene)
    {
        HLASSERT(scene, "scene should not be null");
        HLASSERT(_currentScene, "running scene should not be null");
        delete _sceneStack.back();
        _sceneStack.pop_back();
        pushScene(scene);
    }
    
    void popScene(HLScene *scene)
    {
        HLASSERT(_currentScene, "running scene should not be null");
        delete _sceneStack.back();
        _sceneStack.pop_back();
        if (_sceneStack.size() == 0) {
            // end game
            exit(0);
        }
        _nextScene = _sceneStack.back();
    }
    
    void calculateDeltaTime()
    {
        struct timeval now;
        
        gettimeofday(&now,  0);
        
        // new delta time
        if (_nextDeltaTimeZero)
        {
            _deltaTime = 0;
            _nextDeltaTimeZero = false;
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
            _deltaTime = (now.tv_sec - _lastUpdate.tv_sec) + (now.tv_usec - _lastUpdate.tv_usec) / 1000000.0f;
            _deltaTime = _deltaTime<0?0:_deltaTime;
        }
        
        
        _lastUpdate = now;
    }
    
    void showStats();
    
public:
    
    void mainLoop(void);
    
    HLScene* getRunningScene()
    {
        return _currentScene;
    }
    
    void runWithScene(HLScene *scene)
    {
        HLASSERT(scene, "running scene should not be null");
        HLASSERT(!_currentScene, "current scene should be null");
        pushScene(scene);
    }
    
    void setContentScaleFactor(float scaleFactor)
    {
        _contentScaleFactor = scaleFactor;
    }
    
	float getContentScaleFactor(void)
    {
        return _contentScaleFactor;
    }
    
    static HLDirector3D* sharedDirector();
    
    void setAnimationInterval(double dValue);
    
    void startAnimation()
    {
        _invalid = false;
        gettimeofday(&_lastUpdate,  0);
    }
    
    void stopAnimation()
    {
        _invalid = true;
    }
    
    HLSize getWinSize()
    {
        return _winSize;
    }
    
    HLSize getWinSizeInPixels()
    {
        return _winSizeInPixels;
    }
    
    void reshapeProjection(HLSize& winSize)
    {
        _winSize.width = winSize.width/_contentScaleFactor;
        _winSize.height = winSize.height/_contentScaleFactor;
        _winSizeInPixels = winSize;
        setProjection();
        setGLDefaultValues();
    }
    
    void setGLDefaultValues(void)
    {
        hlglEnable( GL_DEPTH_TEST );
        hlglEnable( GL_CULL_FACE  );
//        hlglEnable( GL_BLEND  );
//        hlglBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        hlglDisable( GL_DITHER );
        glDepthMask( GL_TRUE );
        glDepthFunc( GL_LESS );
        glDepthRangef( 0.0f, 1.0f );
        glClearDepthf( 1.0f );
        glCullFace ( GL_BACK );
        glFrontFace( GL_CCW  );
        glClearStencil( 0 );
        glStencilMask( 0xFFFFFFFF );
        
        // set other opengl default values
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
    }
    
    kmMat4 * getOrthoMatrix();
};

NS_HL_END

#endif

#endif
