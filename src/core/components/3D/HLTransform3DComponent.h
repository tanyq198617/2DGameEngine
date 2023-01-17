//
//  HLTransform3DComponent.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-8-13.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __HLTRANSFORM3DCOMPONENT_H__
#define __HLTRANSFORM3DCOMPONENT_H__

#include "GameEngineConfig.h"

#if GAME_PROJECTION == PROJECTION_3D

#include "mat4.h"

#include "HLEntitySystem.h"

NS_HL_BEGIN

class HLTransform3DComponent : public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLTransform3DComponent, HLPoint3D, position)
    PROPERTY_DECLARE(HLTransform3DComponent, float, scale)
    PROPERTY_DECLARE(HLTransform3DComponent, float, scaleX)
    PROPERTY_DECLARE(HLTransform3DComponent, float, scaleY)
    PROPERTY_DECLARE(HLTransform3DComponent, float, scaleZ)
    PROPERTY_DECLARE(HLTransform3DComponent, float, rotationX)
    PROPERTY_DECLARE(HLTransform3DComponent, float, rotationY)
    PROPERTY_DECLARE(HLTransform3DComponent, float, rotationZ)
    PROPERTY_READONLY_DECLARE(HLTransform3DComponent, kmMat4*, transform)
    
private:
    bool _isTransformGLDirty;
    HLPoint3D _positionInPixels;
    
public:
    HLTransform3DComponent():m_position(0,0, 0),m_scaleX(1),m_scaleY(1),m_scaleZ(1),_isTransformGLDirty(true),_positionInPixels(0,0,0),m_rotationX(0),m_rotationY(0),m_rotationZ(0)
    {
        m_transform = new kmMat4();
    }
    
    ~HLTransform3DComponent()
    {
        delete m_transform;
    }
    
public:
    static const FamilyId familyId;
};

NS_HL_END

#endif

#endif
