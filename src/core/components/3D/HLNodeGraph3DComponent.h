//
//  HLNodeGraphComponent.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-23.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __HLNODEGRAPH3DCOMPONENT_H__
#define __HLNODEGRAPH3DCOMPONENT_H__

#include "GameEngineConfig.h"

#if GAME_PROJECTION == PROJECTION_3D

#include "HLEntitySystem.h"
#include "kazmath/mat4.h"

NS_HL_BEGIN

class HLNodeGraph3DComponent:public HLComponent 
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLNodeGraph3DComponent, HLEntity*, parent)
    PROPERTY_DECLARE(HLNodeGraph3DComponent, std::vector<HLEntity*>, children)
    
private:
    kmMat4* _nodeToWorldTransform;
    kmMat4* _worldToNodeTransform;
    
public:
    
    HLNodeGraph3DComponent():m_parent(NULL)
    {
        _nodeToWorldTransform = new kmMat4();
        _worldToNodeTransform = new kmMat4();
    }
    
    virtual ~HLNodeGraph3DComponent()
    {
        delete _nodeToWorldTransform;
        delete _worldToNodeTransform;
    }
    
    // listen to entity deletion message and remove it from parent if needed
    
    void addChild(HLEntity* thiz, HLEntity* child);
    void removeChild(HLEntity* child);
    void removeFromParent(HLEntity* thiz);
    
    kmMat4* nodeToWorldTransform(HLEntity* thiz);
    kmMat4* worldToNodeTransform(HLEntity* thiz);
    HLPoint3D convertToNodeSpace(HLEntity* thiz, const HLPoint3D &point);
    HLPoint3D convertToWorldSpace(HLEntity* thiz, const HLPoint3D &point);
public:
    static const FamilyId familyId;
};

NS_HL_END

#endif

#endif
