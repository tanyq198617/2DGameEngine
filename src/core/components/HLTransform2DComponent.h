//
//  Transform2D.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-22.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLTRANSFORM2DCOMPONENT_H__
#define __HLTRANSFORM2DCOMPONENT_H__

#include "HLGeometry.h"
#include "kazmath/mat4.h"

#include "HLEntitySystem.h"

#include <list>

NS_HL_BEGIN

class HLTransform2DComponent : public HLComponent
{
    friend class HLSceneManager;
    friend class HLEntityManager;
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLTransform2DComponent, HLPoint, position)
    PROPERTY_DECLARE(HLTransform2DComponent, float, vertexZ)
    PROPERTY_DECLARE(HLTransform2DComponent, int, zOrder)
    PROPERTY_DECLARE(HLTransform2DComponent, HLPoint, anchorPoint)
    PROPERTY_DECLARE(HLTransform2DComponent, HLSize, size)
    PROPERTY_DECLARE(HLTransform2DComponent, float, scale)
    PROPERTY_DECLARE(HLTransform2DComponent, float, scaleX)
    PROPERTY_DECLARE(HLTransform2DComponent, float, scaleY)
    PROPERTY_DECLARE(HLTransform2DComponent, float, skewX)
    PROPERTY_DECLARE(HLTransform2DComponent, float, skewY)
    PROPERTY_DECLARE(HLTransform2DComponent, float, rotation)
    PROPERTY_READONLY_DECLARE(HLTransform2DComponent, kmMat4*, transform)
    PROPERTY_DECLARE(HLTransform2DComponent, HLEntity*, parent)
    PROPERTY_READONLY_DECLARE(HLTransform2DComponent, std::list<HLEntity*>, children)
    
private:
    bool _isTransformGLDirty;
    kmMat4* _nodeToWorldTransform;
    kmMat4* _worldToNodeTransform;
    std::list<HLEntity*> _childrenToAdd;
    bool _lockChildren;
    void processAddChildren();
    
public:
    HLTransform2DComponent():m_position(0,0),m_vertexZ(0),m_zOrder(0),m_anchorPoint(0,0),m_size(0,0),m_scaleX(1),m_scaleY(1),m_skewX(0),m_skewY(0),m_rotation(0),_isTransformGLDirty(true),m_parent(NULL),_lockChildren(false)
    {
        m_transform = new kmMat4();
        _nodeToWorldTransform = new kmMat4();
        _worldToNodeTransform = new kmMat4();
    }
    
    ~HLTransform2DComponent()
    {
        delete m_transform;
        delete _nodeToWorldTransform;
        delete _worldToNodeTransform;
    }
    
    void addChild(HLEntity* child, int zOrder);
    void addChild(HLEntity* child);
    void removeChild(HLEntity* child, bool cleanup = false);
    void removeAllChildren(bool cleanup = false);
    void removeFromParent(bool cleanup = false);
    
    std::list<HLEntity*>& getChildrenRef() {
        return m_children;
    }
    
    kmMat4* nodeToWorldTransform();
    kmMat4* worldToNodeTransform();
    HLPoint convertToNodeSpace(const HLPoint &point);
    HLPoint convertToWorldSpace(const HLPoint &point);
    
    
    FAMILYID
};

NS_HL_END

#endif
