//
//  HLNodeGraph3DComponent.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-23.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLNodeGraph3DComponent.h"

#if GAME_PROJECTION == PROJECTION_3D

#include <vector>
#include "HLUtil.h"
#include "vec3.h"

NS_HL_BEGIN

//const FamilyId HLNodeGraph3DComponent::familyId = 2;
INIT_FAMILYID(HLNodeGraph3DComponent)

PROPERTIES_INITIAL(HLNodeGraph3DComponent)
REGISTER_PROPERTY(HLNodeGraph3DComponent, HLEntity*, parent)
REGISTER_PROPERTY(HLNodeGraph3DComponent, std::vector<HLEntity*>, children)

void HLNodeGraph3DComponent::addChild(HLEntity* thiz, HLEntity* child)
{
    HLASSERT(child, "Argument must be non-nil");
    HLASSERT(!(child->hasProperty("parent") && 
               child->getProperty<HLEntity*>("parent")!=NULL), 
             "Child already have parent.");
    
    m_children.push_back(child);
    
    // TODO post notification
}

void HLNodeGraph3DComponent::removeChild(HLEntity* child)
{
    if (!child) 
    {
        return;
    }
    if (child->hasProperty("parent")) 
    {
        child->setProperty("parent", NULL);
    }
    // TODO post notification
    
    for (std::vector<HLEntity*>::iterator itr = m_children.begin(); itr != m_children.end(); ) 
    {
        if (child == *itr) 
        {
            itr = m_children.erase(itr);
        }
        else
        {
            itr++;
        }
    }
}

void HLNodeGraph3DComponent::removeFromParent(HLEntity* thiz)
{
    HLNodeGraph3DComponent *comp = m_parent->getComponent<HLNodeGraph3DComponent>();
    HLASSERT(comp, "Parent should have nodegraph component");
    comp->removeChild(thiz);
}

HLEntity* HLNodeGraph3DComponent::get_parent()
{
    return m_parent;
}

void HLNodeGraph3DComponent::set_parent(HLEntity *entity)
{
    m_parent = entity;
}

std::vector<HLEntity*> HLNodeGraph3DComponent::get_children()
{
    return m_children;
}

void HLNodeGraph3DComponent::set_children(std::vector<HLEntity*> children)
{
    HLASSERT(0, "Use add child instead!");
}

kmMat4* HLNodeGraph3DComponent::nodeToWorldTransform(HLEntity* thiz)
{
    if (thiz->hasProperty("transform")) 
    {
        kmMat4* transform = thiz->getProperty<kmMat4*>("transform");
        kmMat4Assign(_nodeToWorldTransform, transform);
    }
    else 
    {
        kmMat4Identity(_nodeToWorldTransform);
    }
    HLEntity* parent = m_parent;
    while (parent != NULL) 
    {
        if (parent->hasComponent<HLNodeGraph3DComponent>()) 
        {
            kmMat4Multiply(_nodeToWorldTransform, _nodeToWorldTransform, 
                           parent->getComponent<HLNodeGraph3DComponent>()->nodeToWorldTransform(parent));
            parent = parent->getProperty<HLEntity*>("parent");
        }
        else 
        {
            parent = NULL;
        }
        
    }
    return _nodeToWorldTransform;
}

kmMat4* HLNodeGraph3DComponent::worldToNodeTransform(HLEntity* thiz)
{
    kmMat4Inverse(_worldToNodeTransform, nodeToWorldTransform(thiz));
    return _worldToNodeTransform;
}

HLPoint3D HLNodeGraph3DComponent::convertToNodeSpace(HLEntity* thiz, const HLPoint3D &point)
{
    kmMat4* transform = worldToNodeTransform(thiz);
    kmVec3 p;
    p.x = point.x;
    p.y = point.y;
    p.z = point.z;
    kmVec3Transform(&p, &p, transform);
    return HLPoint3D(p.x, p.y, p.z);
}

HLPoint3D HLNodeGraph3DComponent::convertToWorldSpace(HLEntity* thiz, const HLPoint3D &point)
{
    kmMat4* transform = nodeToWorldTransform(thiz);
    kmVec3 p;
    p.x = point.x;
    p.y = point.y;
    p.z = point.z;
    kmVec3Transform(&p, &p, transform);
    return HLPoint3D(p.x, p.y, p.z);
}

NS_HL_END

#endif
