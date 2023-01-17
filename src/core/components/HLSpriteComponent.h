//
//  HLSpriteComponent.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-28.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLSPRITECOMPONENT_H__
#define __HLSPRITECOMPONENT_H__

#include "HLEntitySystem.h"
#include "HLGeometry.h"

NS_HL_BEGIN

class HLTexture;
class HLRect;
class HLTransform2DComponent;
class HLColorComponent;
class HLShaderComponent;
class HLColorAdvanceComponent;
class HLBlendFuncComponent;

class HLSpriteComponent:public HLComponent
{
    friend class HLBatchComponent;
    
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLSpriteComponent, HLTexture*, texture)
    PROPERTY_DECLARE_IMPLEMENT(HLSpriteComponent, bool, textureRotated)
    PROPERTY_DECLARE_IMPLEMENT(HLSpriteComponent, bool, gray)
    PROPERTY_DECLARE(HLSpriteComponent, HLRect, textureRect)
    PROPERTY_READONLY_DECLARE_IMPLEMENT(HLSpriteComponent, bool, useBatch)
    PROPERTY_DECLARE(HLSpriteComponent, bool, flipX)
    PROPERTY_DECLARE(HLSpriteComponent, bool, flipY)
    PROPERTY_DECLARE(HLSpriteComponent, HLSize, untrimmedSize)
    PROPERTY_DECLARE(HLSpriteComponent, HLPoint, offsetPosFromBL)
    PROPERTY_DECLARE_IMPLEMENT(HLSpriteComponent, float, brightness)
    PROPERTY_DECLARE_IMPLEMENT(HLSpriteComponent, float, alphaTest)
    
public:
    HLSpriteComponent():m_texture(NULL), m_useBatch(false), m_flipX(false), m_flipY(false), m_untrimmedSize(0,0), m_offsetPosFromBL(0,0), m_brightness(1),m_textureRotated(false),m_gray(false),mDirty(true),mTransComp(NULL),mColorComp(NULL),mShaderComp(NULL),mColorAdvComp(NULL),mBlendComp(NULL),m_alphaTest(-1) {}
    
    virtual ~HLSpriteComponent();
    
    HLSpriteComponent(HLTexture* texture, HLRect textureRect): m_texture(texture), m_textureRect(textureRect),
                                                                m_untrimmedSize(0,0), m_offsetPosFromBL(0,0)
    {}
    
    virtual void onDeactive();
    
    virtual void onActive();
    
    virtual void onInternalEvent(const char* event, void* info);
    void updateCoordsAndVerts();
    
    void drawSprite();
    
    FAMILYID
    
private:
    GLfloat mVertices[8];
    GLfloat mCoordinates[8];
    bool mDirty;
    
    HLTransform2DComponent* mTransComp;
    HLColorComponent* mColorComp;
    HLShaderComponent* mShaderComp;
    HLColorAdvanceComponent* mColorAdvComp;
    HLBlendFuncComponent* mBlendComp;
};

NS_HL_END

#endif
