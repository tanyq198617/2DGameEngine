//
//  HLDirector3D.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-8-13.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLDirector3D.h"

#if GAME_PROJECTION == PROJECTION_2D

#else

#include "HLApplication.h"
#include "HLAutoreleasePool.h"
#include "kazmath/GL/matrix.h"
#include "HLFileUtil.h"
#include "HLClassRegister.h"
#include "HLTransform2DComponent.h"
#include "HLNodeGraphComponent.h"
#include "HLSpriteComponent.h"
#include "HLTextTTFComponent.h"
#include "HLColorComponent.h"
#include "HLVerticesComponent.h"
#include "HLAnimateSystem.h"
#include "HLTransform3DComponent.h"
#include "HLWavefrontObjComponent.h"
#include "HLCameraComponent.h"
#include "HLNodeGraph3DComponent.h"

NS_HL_BEGIN

kmMat4 orthoMatrix;

std::vector<ThreadScheduleObject> pendingScheduleArray;
pthread_mutex_t scheduleSelectorMutex;

HLDirector3D::HLDirector3D()
{
    _nextDeltaTimeZero = true;
    registerComponents();
}

void HLDirector3D::registerComponents()
{
    REGISTER_CLASS(HLTransform2DComponent);
    REGISTER_CLASS(HLNodeGraphComponent);
    REGISTER_CLASS(HLSpriteComponent);
    REGISTER_CLASS(HLTextTTFComponent);
    REGISTER_CLASS(HLColorComponent);
    REGISTER_CLASS(HLVerticesComponent);
    REGISTER_CLASS(HLTransform3DComponent);
    REGISTER_CLASS(HLWavefrontObjComponent);
    REGISTER_CLASS(HLNodeGraph3DComponent);
    REGISTER_CLASS(HLCameraComponent);
}

HLDirector3D* HLDirector3D::sharedDirector()
{
    static HLDirector3D director;
    return &director;
}

void HLDirector3D::setProjection()
{
    float zeye = (960 / 1.1566f);
    
    glViewport(0, 0, _winSizeInPixels.width, _winSizeInPixels.height);
    kmGLMatrixMode(KM_GL_PROJECTION);
    kmGLLoadIdentity();
    kmMat4 matrixPerspective, matrixLookup;
    kmMat4PerspectiveProjection( &matrixPerspective, 45.0f, (GLfloat)_winSizeInPixels.width/_winSizeInPixels.height, 0.1f, zeye*2);
    kmGLMultMatrix(&matrixPerspective);
    
    kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLLoadIdentity();
    kmVec3 eye, center, up;
    kmVec3Fill( &eye, 0, 0, zeye );
    kmVec3Fill( &center, 0.0f, 0.0f, 0.0f );
    kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
    kmMat4LookAt(&matrixLookup, &eye, &center, &up);
    kmGLMultMatrix(&matrixLookup);
    
    kmMat4OrthographicProjection(&orthoMatrix, 0, _winSizeInPixels.width, 0, _winSizeInPixels.height, -1024, 1024);
}

void HLDirector3D::setAnimationInterval(double dValue)
{
    HLApplication::sharedApplication().setAnimationInterval(dValue);
}

void HLDirector3D::mainLoop()
{
    if(!pthread_mutex_trylock(&scheduleSelectorMutex))
    {
        std::vector<ThreadScheduleObject>::iterator itr;
        for (itr = pendingScheduleArray.begin(); itr!=pendingScheduleArray.end(); itr++) {
            ThreadScheduleObject ob = *itr;
            (ob.target->*(ob.selector))(ob.object);
            ob.target->release();
            ob.object->release();
        }
        pendingScheduleArray.clear();
        pthread_mutex_unlock(&scheduleSelectorMutex);
    }
    
    // update
    if (!_invalid) {
        drawScene();
        HLAutoreleasePool::currentPool()->purgePool();
    }
}

void HLDirector3D::drawScene()
{
    calculateDeltaTime();
    
    HLTweenSystem::update(_deltaTime);
    HLAnimateSystem::update(_deltaTime);
    
    if (_nextScene) { // TODO transition
        _currentScene = _nextScene;
        _nextScene = NULL;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    kmGLPushMatrix();
    if (_currentScene) {
        _currentScene->update(_deltaTime);
        _currentScene->draw();
    }
    showStats();
    kmGLPopMatrix();
    
    HLApplication::sharedApplication().swapBuffers();
}

void HLDirector3D::showStats()
{
    static unsigned int frames = 0;
    static float accDt = 0;
    frames++;
    accDt += _deltaTime;
    
    static HLEntity* fpsLabel = NULL;
    if (accDt >= 0.1) {
        char fpschar[10];
        sprintf(fpschar, "%.1f", frames/accDt);
        if (!fpsLabel) {
            fpsLabel = HLEntityManager::sharedEntityManager()->createEntity("director_fpslabel");
            fpsLabel->addComponent<HLTextTTFComponent>();
            fpsLabel->setProperty("textSize", 17);
        }
        fpsLabel->setProperty("text", std::string(fpschar));
        frames = 0;
        accDt = 0;
    }
    
    if (fpsLabel) {
        kmGLLoadIdentity();
        fpsLabel->getComponent<HLTextTTFComponent>()->draw(fpsLabel);
    }
    
}

kmMat4 * HLDirector3D::getOrthoMatrix()
{
    return &orthoMatrix;
}

NS_HL_END

#endif
