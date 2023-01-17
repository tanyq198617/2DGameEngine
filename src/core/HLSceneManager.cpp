//
//  HLSCene.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-24.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLSceneManager.h"
#include "HLGLMatrixStack.h"
#include "HLTransform2DComponent.h"
#include "HLClippingComponent.h"
#include "HLScissorComponent.h"
#include "HLDrawingPrimitives.h"

#if GAME_PROJECTION == PROJECTION_3D
#include "HLCameraComponent.h"
#include "HLNodeGraph3DComponent.h"
#include "kazmath/GL/mat4stack.h"
#else
#include "HLDirector2D.h"
#endif

#include "HLSceneView.h"

#if USE_JS_SCRIPTING
#include "JSSceneManager.h"
#endif

NS_HL_BEGIN

HLSceneManager::~HLSceneManager()
{
#if USE_JS_SCRIPTING
    JSSceneManager::removeJSObject(this);
#endif
    delete mEntityMgr;
}

void HLSceneManager::updateEntity(HLEntity* entity, float deltaTime)
{
//    if (!entity->isVisible())
//    {
//        return;
//    }
    
    HLTransform2DComponent* transComp = entity->getComponent<HLTransform2DComponent>();
    
    std::list<HLComponent*>::const_iterator itr;
    if (transComp)
    {
        if (!transComp->mEntity) // should be destroyed
        {
            return;
        }
        transComp->_lockChildren = true;
        std::list<HLEntity*>&& children = transComp->get_children();
        if (children.size() > 0)
        {
            std::list<HLEntity*>::iterator it = children.begin();
            while (it != children.end())
            {
                if (*it == NULL)
                {
                    it = children.erase(it);
                    continue;
                }
                if ((*it)->getComponent<HLTransform2DComponent>()->get_zOrder() < 0)
                {
                    updateEntity(*it, deltaTime);
                }
                else
                {
                    break;
                }
                ++it;
            }
            if (!transComp->mEntity) // should be destroyed
            {
                return;
            }
            entity->onUpdate(deltaTime);
            if (!transComp->mEntity) // should be destroyed
            {
                return;
            }
            while (it != children.end())
            {
                if (*it == NULL)
                {
                    it = children.erase(it);
                }
                else
                {
                    updateEntity(*it, deltaTime);
                    ++it;
                }
            }
        }
        else
        {
            entity->onUpdate(deltaTime);
        }
        transComp->_lockChildren = false;
        if (transComp->mEntity) // may be deactivated during update
        {
            transComp->processAddChildren();
        }
    }
    else
    {
        entity->onUpdate(deltaTime);
    }
}

HLEntity* HLSceneManager::hitTest(const HLPoint& point)
{
#if defined(TARGET_MACOS)
    OpenThreads::ScopedReadLock lock(mMutex);
#endif
    for (std::list<HLEntity*>::reverse_iterator itr = entities.rbegin(); itr != entities.rend(); itr++)
    {
        kmMat4 t;
        kmMat4Identity(&t);
        if (mSceneView)
        {
            kmMat4Multiply(&t, &t, mSceneView->nodeToWorldTransform());
        }
        kmMat4Multiply(&t, &t, &mCameraMatrix);
        if (*itr)
        {
            if (*itr != NULL)
            {
                HLEntity * res = (*itr)->hitTest(&t, point);
                if (res)
                    return res;
            }
        }
    }
    return NULL;
}

GLint g_sStencilBits = -1;

void HLSceneManager::drawEntity(HLEntity* entity)
{
    if (!entity->isVisible())
    {
        return;
    }
    
    HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
    
    HLScissorComponent* scissorComp = entity->getComponent<HLScissorComponent>();
    bool scissorEnabled = hlglIsEnabled(GL_SCISSOR_TEST);
    GLint scissorBox[4];
    if (scissorComp && scissorComp->get_scissorBox() != HLRectZero)
    {
        if (scissorEnabled)
        {
            hlglGetScissorBox(scissorBox);
        }
        else
        {
            hlglEnable(GL_SCISSOR_TEST);
        }
        
        HLRect scissorBox1 = scissorComp->get_scissorBox();
        
        float scale = HLDirector2D::getSingleton()->getWinSizeScale();
        HLSize winSizeInPixel = HLDirector2D::getSingleton()->getWinSizeInPixels();
        HLPoint loc = HLDirector2D::getSingleton()->convertToScreenPoint(scissorBox1.origin)*CONTENT_SCALE_FACTOR;
        loc.y = winSizeInPixel.height - loc.y;
        
        if (scissorEnabled)
        {
            float left = MAX(loc.x, scissorBox[0]);
            float bottom = MAX(loc.y, scissorBox[1]);
            float right = MIN(loc.x+scissorBox1.size.width/scale, scissorBox[0]+scissorBox[2]);
            float top = MIN(loc.y+scissorBox1.size.height/scale, scissorBox[1]+scissorBox[3]);
            if (right > left && top > bottom)
            {
                hlglScissor(roundf(left), roundf(bottom), roundf((right - left)), roundf((top - bottom)));
            }
            else
            {
                matrixStack->popMatrix();
                return;
            }
        }
        else
        {
            hlglScissor(roundf(loc.x), roundf(loc.y), scissorBox1.size.width/scale, scissorBox1.size.height/scale);
        }
    }
    
    bool drawstencil = false;
    // store the current stencil layer (position in the stencil buffer),
    // this will allow nesting up to n CCClippingNode,
    // where n is the number of bits of the stencil buffer.
    static GLint layer = -1;
    // manually save the stencil state
    GLboolean currentStencilEnabled = GL_FALSE;
    GLuint currentStencilWriteMask = ~0;
    GLenum currentStencilFunc = GL_ALWAYS;
    GLint currentStencilRef = 0;
    GLuint currentStencilValueMask = ~0;
    GLenum currentStencilFail = GL_KEEP;
    GLenum currentStencilPassDepthFail = GL_KEEP;
    GLenum currentStencilPassDepthPass = GL_KEEP;
    
    do
    {
        if (g_sStencilBits < 1)
        {
            break;
        }
        
        HLClippingComponent* comp = entity->getComponent<HLClippingComponent>();
        if (!comp)
        {
            break;
        }
        
        if (!comp->get_stencil() || !comp->get_stencil()->isVisible())
        {
            break;
        }
        
        // all the _stencilBits are in use?
        if (layer + 1 == g_sStencilBits)
        {
            static bool once = true;
            if (once)
            {
                HLLOG("Nesting more than %d stencils is not supported. Everything will be drawn without stencil for this node and its childs.", g_sStencilBits);
                once = false;
            }
            break;
        }
        
        // increment the current layer
        ++layer;
        
        // mask of the current layer (ie: for layer 3: 00000100)
        GLint mask_layer = 0x1 << layer;
        // mask of all layers less than the current (ie: for layer 3: 00000011)
        GLint mask_layer_l = mask_layer - 1;
        // mask of all layers less than or equal to the current (ie: for layer 3: 00000111)
        GLint mask_layer_le = mask_layer | mask_layer_l;
        
        currentStencilEnabled = glIsEnabled(GL_STENCIL_TEST);
        glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint *)&currentStencilWriteMask);
        glGetIntegerv(GL_STENCIL_FUNC, (GLint *)&currentStencilFunc);
        glGetIntegerv(GL_STENCIL_REF, &currentStencilRef);
        glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint *)&currentStencilValueMask);
        glGetIntegerv(GL_STENCIL_FAIL, (GLint *)&currentStencilFail);
        glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint *)&currentStencilPassDepthFail);
        glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint *)&currentStencilPassDepthPass);
        
        // enable stencil use
        glEnable(GL_STENCIL_TEST);
        CHECK_GL_ERROR();
        
        // all bits on the stencil buffer are readonly, except the current layer bit,
        // this means that operation like glClear or glStencilOp will be masked with this value
        glStencilMask(mask_layer);
        
        // manually save the depth test state
        //GLboolean currentDepthTestEnabled = GL_TRUE;
        GLboolean currentDepthWriteMask = GL_TRUE;
        //currentDepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
        glGetBooleanv(GL_DEPTH_WRITEMASK, &currentDepthWriteMask);
        
        // disable depth test while drawing the stencil
        //glDisable(GL_DEPTH_TEST);
        // disable update to the depth buffer while drawing the stencil,
        // as the stencil is not meant to be rendered in the real scene,
        // it should never prevent something else to be drawn,
        // only disabling depth buffer update should do
        glDepthMask(GL_FALSE);
        
        ///////////////////////////////////
        // CLEAR STENCIL BUFFER
        
        // manually clear the stencil buffer by drawing a fullscreen rectangle on it
        // setup the stencil test func like this:
        // for each pixel in the fullscreen rectangle
        //     never draw it into the frame buffer
        //     if not in inverted mode: set the current layer value to 0 in the stencil buffer
        //     if in inverted mode: set the current layer value to 1 in the stencil buffer
        glStencilFunc(GL_NEVER, mask_layer, mask_layer);
        glStencilOp(!comp->get_inverted() ? GL_ZERO : GL_REPLACE, GL_KEEP, GL_KEEP);
        
        matrixStack->pushMatrix();
        matrixStack->loadIdentity();
        // draw a fullscreen solid rectangle to clear the stencil buffer
        drawColor4F(1, 1, 1, 1);
        HLSize size = HLDirector2D::getSingleton()->getWinSize();
        drawSolidRect(HLPointZero, HLPoint(size.width, size.height));
        matrixStack->popMatrix();
        
        ///////////////////////////////////
        // DRAW CLIPPING STENCIL
        
        // setup the stencil test func like this:
        // for each pixel in the stencil node
        //     never draw it into the frame buffer
        //     if not in inverted mode: set the current layer value to 1 in the stencil buffer
        //     if in inverted mode: set the current layer value to 0 in the stencil buffer
        glStencilFunc(GL_NEVER, mask_layer, mask_layer);
        glStencilOp(!comp->get_inverted() ? GL_REPLACE : GL_ZERO, GL_KEEP, GL_KEEP);
        
        // draw the stencil node as if it was one of our child
        matrixStack->pushMatrix();
        HLTransform2DComponent* transComp = entity->getComponent<HLTransform2DComponent>();
        if (transComp)
        {
            kmMat4* transform = transComp->get_transform();
            matrixStack->multMatrix(transform);
        }
        drawEntity(comp->get_stencil());
        matrixStack->popMatrix();
        
        // restore the depth test state
        glDepthMask(currentDepthWriteMask);
        
        glStencilFunc(GL_EQUAL, mask_layer_le, mask_layer_le);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        
        drawstencil = true;
    } while(0);
    
    HLTransform2DComponent* transComp = entity->getComponent<HLTransform2DComponent>();
    std::list<HLComponent*>::const_iterator itr;
    matrixStack->pushMatrix();
    if (transComp) 
    {
        kmMat4* transform = transComp->get_transform();
        matrixStack->multMatrix(transform);
    }
    if (transComp) 
    {
        std::list<HLEntity*>&& children = transComp->get_children();
        if (children.size() > 0) 
        {
            std::list<HLEntity*>::iterator it = children.begin();
            while (it != children.end())
            {
                if (*it == NULL)
                {
                    it = children.erase(it);
                    continue;
                }
                if ((*it)->getComponent<HLTransform2DComponent>()->get_zOrder() < 0)
                {
                    drawEntity(*it);
                } 
                else 
                {
                    break;
                }
                ++it;
            }
            entity->onDraw();
            while (it != children.end())
            {
                if (*it == NULL)
                {
                    it = children.erase(it);
                }
                else
                {
                    drawEntity(*it);
                    ++it;
                }
            }
        } 
        else 
        {
            entity->onDraw();
        }
    } 
    else 
    {
        entity->onDraw();
    }
    matrixStack->popMatrix();
    
    if (drawstencil)
    {
        // CLEANUP
        // manually restore the stencil state
        glStencilFunc(currentStencilFunc, currentStencilRef, currentStencilValueMask);
        glStencilOp(currentStencilFail, currentStencilPassDepthFail, currentStencilPassDepthPass);
        glStencilMask(currentStencilWriteMask);
        if (!currentStencilEnabled)
        {
            glDisable(GL_STENCIL_TEST);
        }
        
        // we are done using this layer, decrement
        layer--;
    }
    
    if (scissorComp)
    {
        if (scissorEnabled)
        {
            hlglScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
        }
        else
        {
            hlglDisable(GL_SCISSOR_TEST);
        }
    }
}

void HLSceneManager::update(float deltaTime)
{
    std::list<HLEntity*>::iterator itr = entities.begin();
    while (itr != entities.end())
    {
        if (*itr == NULL)
        {
            itr = entities.erase(itr);
            continue;
        }
        if ((*itr)->isEnabled())
        {
            updateEntity(*itr, deltaTime);
        }
        ++itr;
    }
    
    mEntityMgr->destroyEntities();
    
#if GAME_PROJECTION == PROJECTION_3D
    if (activeCamera&&updateCamera)
    {
        updateCamera = false;
        kmGLMatrixMode(KM_GL_MODELVIEW);
        kmGLLoadIdentity();
        kmVec3 eye, center, up;
        kmMat4 matrixLookup;
        HLPoint3D p, c, u;
        u = activeCamera->getProperty<HLPoint3D>("up");
        c = activeCamera->getProperty<HLPoint3D>("center");
        if (activeCamera->hasProperty("position"))
        {
            p = activeCamera->getProperty<HLPoint3D>("position");
        }
        if (activeCamera->hasComponent<HLNodeGraph3DComponent>())
        {
            HLNodeGraph3DComponent *comp = activeCamera->getComponent<HLNodeGraph3DComponent>();
            p = comp->convertToWorldSpace(activeCamera, p);
            c = comp->convertToWorldSpace(activeCamera, c);
            u = comp->convertToWorldSpace(activeCamera, u);
        }
        u = u.normalize();
        kmVec3Fill( &eye, p.x, p.y, p.z );
        kmVec3Fill( &center, c.x, c.y, c.z );
        kmVec3Fill( &up, u.x, u.y, u.z);
        kmMat4LookAt(&matrixLookup, &eye, &center, &up);
        kmGLMultMatrix(&matrixLookup);
    }
    else
    {
        kmGLMatrixMode(KM_GL_MODELVIEW);
        kmGLLoadMatrix(&cameraMatrix);
    }
    
    kmGLGetMatrix(KM_GL_MODELVIEW, &cameraMatrix);
#else
    if (mCameraChangeProgress < 1.f)
    {
        mCameraChangeProgress += deltaTime*2;
        if (mCameraChangeProgress > 1.f)
        {
            mCameraChangeProgress = 1.f;
        }
    }
    updateCameraMatrix();
#endif
}

#if GAME_PROJECTION == PROJECTION_3D
extern "C" {
    extern km_mat4_stack modelview_matrix_stack;
}
#endif

void HLSceneManager::draw()
{
    HLGLMatrixStack::matrixStack()->multMatrix(&mCameraMatrix);
    
    std::list<HLEntity*>::iterator itr = entities.begin();
    while (itr != entities.end())
    {
        if (*itr == NULL)
        {
            itr = entities.erase(itr);
        }
        else
        {
            if ((*itr)->isEnabled()&&(*itr)->isVisible()) {
                drawEntity(*itr);
            }
            ++itr;
        }
    }
}

//#if GAME_PROJECTION == PROJECTION_3D
void HLSceneManager::setActiveCamera(HLEntity* camera, bool isActive, bool slow)
{
    if (isActive) 
    {
        if (mActiveCamera && mActiveCamera != camera)
        {
            mOldCameraMatrix = mCameraMatrix;
            if (slow) {
                mCameraChangeProgress = 0.f;
            } else {
                mCameraChangeProgress = 1.f;
            }
        }
        mActiveCamera = camera;
        updateCameraMatrix();
    }
    else 
    {
        if (mActiveCamera == camera) 
        {
            mActiveCamera = NULL;
        }
    }
}

//void HLSceneManager::setActiveCamera(HLEntity* camera)
//{
//    mActiveCamera = camera;
//}

void HLSceneManager::updateCameraMatrix()
{
    kmMat4Identity(&mCameraMatrix);
    if (mActiveCamera)
    {
        if (mActiveCamera->hasComponent<HLTransform2DComponent>())
        {
            HLSize winSize = HLDirector2D::getSingleton()->getWinSize();
            //            HLPoint pos = mActiveCamera->getProperty<HLPoint>("position");
            HLPoint pos = mActiveCamera->getComponent<HLTransform2DComponent>()->convertToWorldSpace(HLPointZero);
            HLRect cameraRect = mActiveCamera->getProperty<HLRect>("cameraRect");
            if (cameraRect != HLRectZero)
            {
                pos.x = MAX(pos.x, cameraRect.origin.x);
                pos.x = MIN(pos.x, cameraRect.origin.x+cameraRect.size.width);
                pos.y = MAX(pos.y, cameraRect.origin.y);
                pos.y = MIN(pos.y, cameraRect.origin.y+cameraRect.size.height);
            }
            kmMat4Translation(&mCameraMatrix, winSize.width/2-pos.x, winSize.height/2-pos.y, 0);
            if (mCameraChangeProgress < 1)
            {
                if (mCameraChangeProgress == 0.f && kmMat4AreEqual(&mOldCameraMatrix, &mCameraMatrix))
                {
                    mCameraChangeProgress = 1.f;
                    return;
                }
                for (int i = 0; i < 16; ++i)
                {
                    mCameraMatrix.mat[i] = mOldCameraMatrix.mat[i] + (mCameraMatrix.mat[i] - mOldCameraMatrix.mat[i]) * mCameraChangeProgress;
                }
            }
        }
    }
}

kmMat4* HLSceneManager::currentCameraMatrix()
{
    return &mCameraMatrix;
}

//#endif

NS_HL_END
