//
//  Transform2D.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-22.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLTransform2DComponent.h"
#include "GameEngineConfig.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "HLClassRegister.h"
#include "HLUtil.h"
#include "HLSceneManager.h"
#include "HLBatchComponent.h"
#include "HLSpriteComponent.h"


NS_HL_BEGIN

//const FamilyId HLTransform2DComponent::familyId = 1;
INIT_FAMILYID(HLTransform2DComponent)

PROPERTIES_INITIAL(HLTransform2DComponent)
REGISTER_PROPERTY(HLTransform2DComponent, HLPoint, position)
REGISTER_PROPERTY(HLTransform2DComponent, float, vertexZ)
REGISTER_PROPERTY(HLTransform2DComponent, int, zOrder)
REGISTER_PROPERTY(HLTransform2DComponent, HLPoint, anchorPoint)
REGISTER_PROPERTY(HLTransform2DComponent, HLSize, size)
REGISTER_PROPERTY(HLTransform2DComponent, float, scale)
REGISTER_PROPERTY(HLTransform2DComponent, float, scaleX)
REGISTER_PROPERTY(HLTransform2DComponent, float, scaleY)
REGISTER_PROPERTY(HLTransform2DComponent, float, skewX)
REGISTER_PROPERTY(HLTransform2DComponent, float, skewY)
REGISTER_PROPERTY(HLTransform2DComponent, float, rotation)
REGISTER_PROPERTY_READONLY(HLTransform2DComponent, kmMat4*, transform)
REGISTER_PROPERTY(HLTransform2DComponent, HLEntity*, parent)
REGISTER_PROPERTY_READONLY(HLTransform2DComponent, std::list<HLEntity*>, children)


HLPoint HLTransform2DComponent::get_position()
{
    return m_position;
}


void HLTransform2DComponent::set_position(HLPoint pos)
{
    if (m_position != pos) 
    {
        m_position = pos;
        _isTransformGLDirty = true;
        
        mEntity->onInternalEvent("position_changed", NULL);
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_position_changed", mEntity);
        }
    }
}


int HLTransform2DComponent::get_zOrder()
{
    return m_zOrder;
}


void HLTransform2DComponent::set_zOrder(int zOrder)
{
    if (m_zOrder != zOrder)
    {
        m_zOrder = zOrder;
        if (m_parent)
        {
            HLTransform2DComponent* comp = m_parent->getComponent<HLTransform2DComponent>();
            comp->removeChild(mEntity, false);
            comp->addChild(mEntity, m_zOrder);
        }
    }
}


float HLTransform2DComponent::get_vertexZ()
{
    return m_zOrder;
}


void HLTransform2DComponent::set_vertexZ(float z)
{
    if (m_vertexZ != z)
    {
        m_vertexZ = z;
        _isTransformGLDirty = true;
    }
}


HLPoint HLTransform2DComponent::get_anchorPoint()
{
    return m_anchorPoint;
}


void HLTransform2DComponent::set_anchorPoint(HLPoint anchorPoint)
{
    if (m_anchorPoint != anchorPoint) 
    {
        m_anchorPoint = anchorPoint;
        _isTransformGLDirty = true;
    }
}


HLSize HLTransform2DComponent::get_size()
{
    return m_size;
}


void HLTransform2DComponent::set_size(HLSize size)
{
    if (m_size != size) 
    {
        m_size = size;
        
        _isTransformGLDirty = true;
        
        mEntity->onInternalEvent("size_changed", NULL);
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_size_changed", mEntity);
        }
    }
}


float HLTransform2DComponent::get_scale()
{
    HLASSERT(m_scaleX == m_scaleY, "get_scale, ScaleX != ScaleY. Don't know which one to return");
    return m_scaleX;
}


void HLTransform2DComponent::set_scale(float scale)
{
    if (m_scaleX != scale) 
    {
        m_scaleX = m_scaleY = scale;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_scale_changed", mEntity);
        }
    }
}


float HLTransform2DComponent::get_scaleX()
{
    return m_scaleX;
}


void HLTransform2DComponent::set_scaleX(float scaleX)
{
    if (m_scaleX != scaleX) 
    {
        m_scaleX = scaleX;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_scalex_changed", mEntity);
        }
    }
}


float HLTransform2DComponent::get_scaleY()
{
    return m_scaleY;
}


void HLTransform2DComponent::set_scaleY(float scaleY)
{
    if (m_scaleY != scaleY) 
    {
        m_scaleY = scaleY;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_scaley_changed", mEntity);
        }
    }
}


float HLTransform2DComponent::get_skewX()
{
    return m_skewX;
}


void HLTransform2DComponent::set_skewX(float skewX)
{
    if (m_skewX != skewX) 
    {
         m_skewX = skewX;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_skewx_changed", mEntity);
        }
    }
}


float HLTransform2DComponent::get_skewY()
{
    return m_skewY;
}


void HLTransform2DComponent::set_skewY(float skewY)
{
    if (m_skewY != skewY) 
    {
        m_skewY = skewY;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_skewy_changed", mEntity);
        }
    }
}


float HLTransform2DComponent::get_rotation()
{
    return m_rotation;
}


void HLTransform2DComponent::set_rotation(float rotation)
{
    if (m_rotation != rotation) 
    {
        m_rotation = rotation;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_rotation_changed", mEntity);
        }
    }
}


kmMat4* HLTransform2DComponent::get_transform()
{
    if (_isTransformGLDirty) 
    {
        kmMat4Identity(m_transform);
        kmMat4 tmpMat;
        if (m_position != HLPointZero) 
        {
            kmMat4Translation(m_transform, m_position.x, m_position.y, 0);
        }
        if (m_rotation != 0) 
        {
            kmMat4RotationZ(&tmpMat, -DEGREES_TO_RADIANS(m_rotation));
            kmMat4Multiply(m_transform, m_transform, &tmpMat);
        }
        if (m_skewX != 0 || m_skewY != 0)
        {
            memset(tmpMat.mat, 0, sizeof(float) * 16);
            
            tmpMat.mat[0] = tmpMat.mat[5] = tmpMat.mat[10] = tmpMat.mat[15] = 1.0f;
            tmpMat.mat[1] = tanf(DEGREES_TO_RADIANS(m_skewY));
            tmpMat.mat[4] = tanf(DEGREES_TO_RADIANS(m_skewX));
            kmMat4Multiply(m_transform, m_transform, &tmpMat);
        }
        if (!(m_scaleX == 1 && m_scaleY == 1)) 
        {
            kmMat4Scaling(&tmpMat, m_scaleX, m_scaleY, 1);
            kmMat4Multiply(m_transform, m_transform, &tmpMat);
        }
        if (m_anchorPoint != HLPointZero || m_vertexZ)
        {
            kmMat4Translation(&tmpMat, -m_anchorPoint.x, -m_anchorPoint.y, m_vertexZ);
            kmMat4Multiply(m_transform, m_transform, &tmpMat);
        }
        _isTransformGLDirty = false;

        
//        float x = m_position.x;
//        float y = m_position.y;
//        
//        float c = 1, s = 0;
//        if (m_rotation)
//        {
//            float radians = -DEGREES_TO_RADIANS(m_rotation);
//            c = cosf(radians);
//            s = sinf(radians);
//        }
//        
//        bool needsSkewMatrix = ( m_skewX || m_skewY );
//        
//        if( !needsSkewMatrix && m_anchorPoint != HLPointZero )
//        {
//            x += c * -m_anchorPoint.x * m_scaleX + -s * -m_anchorPoint.y * m_scaleY;
//            y += s * -m_anchorPoint.x * m_scaleX +  c * -m_anchorPoint.y * m_scaleY;
//        }
//        
//        m_transform->mat[0] = c * m_scaleX;
//        m_transform->mat[1] = s * m_scaleX;
//        m_transform->mat[4] = -s * m_scaleY;
//        m_transform->mat[5] = c * m_scaleY;
//        m_transform->mat[12] = x;
//        m_transform->mat[13] = y;
//        m_transform->mat[14] = m_vertexZ;
//        
//        if (needsSkewMatrix)
//        {
//            kmMat4 tmpMatrix;
//            kmMat4Identity(&tmpMatrix);
//            tmpMatrix.mat[1] = tanf(DEGREES_TO_RADIANS(m_skewY));
//            tmpMatrix.mat[4] = tanf(DEGREES_TO_RADIANS(m_skewX));
//            kmMat4Multiply(m_transform, &tmpMatrix, m_transform);
//            if (m_anchorPoint != HLPointZero)
//            {
//                kmMat4Translation(&tmpMatrix, -m_anchorPoint.x, -m_anchorPoint.y, 0);
//                kmMat4Multiply(m_transform, m_transform, &tmpMatrix);
//            }
//        }
//
//        _isTransformGLDirty = false;
        
    }
    return m_transform;
}

//
//void HLTransform2DComponent::set_transform(kmMat4* transform)
//{
//    // cannot set transform directly
//    HLASSERT(0, "Cannot set transform directly!");
//}


void HLTransform2DComponent::processAddChildren()
{
    if (_lockChildren)
    {
        return;
    }
    for (std::list<HLEntity*>::iterator itr = _childrenToAdd.begin(); itr != _childrenToAdd.end(); ++itr)
    {
        int zOrder = 0;
        HLTransform2DComponent* comp = (*itr)->getComponent<HLTransform2DComponent>();
        if (comp)
        {
            zOrder = comp->get_zOrder();
        }
        addChild(*itr, zOrder);
    }
    _childrenToAdd.clear();
}

void HLTransform2DComponent::addChild(HLEntity* child)
{
    HLASSERT(child, "Argument must be non-nil");
    HLASSERT(child->hasProperty("parent"), "Child should have parent property");
    HLASSERT(child->getProperty<HLEntity*>("parent")==NULL,
             "Child already have parent.");
    addChild(child, child->getProperty<int>("zOrder"));
}

void HLTransform2DComponent::addChild(HLEntity* child, int zOrder)
{
    HLASSERT(mEntity, "component not activated");
    HLASSERT(child, "Argument must be non-nil");
    HLASSERT(child->hasProperty("parent"), "Child should have parent property");
    HLASSERT(child->getProperty<HLEntity*>("parent")==NULL,
             "Child already have parent.");
    if (mEntity->hasProperty("useBatch"))
    {
        if (mEntity->getProperty<bool>("useBatch"))
        {
            printf("\nWarning: Children will not use batch.\n");
        }
    }
    
    HLTransform2DComponent* transComp = child->getComponent<HLTransform2DComponent>();
    
    if (_lockChildren)
    {
        transComp->set_zOrder(zOrder);
        _childrenToAdd.push_back(child);
        return;
    }
    
    if (!transComp)
    {
        zOrder = 0;
    }
    else
    {
        transComp->set_zOrder(zOrder);
        transComp->set_parent(mEntity);
    }
    
    if (m_children.size()==0)
    {
        m_children.push_back(child);
    }
    else
    {
        HLEntity* back = m_children.back();
        if (back && (!back->hasComponent<HLTransform2DComponent>() || back->getProperty<int>("zOrder") <= zOrder))
        {
            m_children.push_back(child);
        }
        else
        {
            std::list<HLEntity*>::iterator itr = m_children.begin();
            while (itr != m_children.end())
            {
                if (*itr)
                {
                    if ((*itr)->hasComponent<HLTransform2DComponent>())
                    {
                        if ((*itr)->getProperty<int>("zOrder") > zOrder)
                        {
                            m_children.insert(itr, child);
                            break;
                        }
                    }
                    ++itr;
                }
                else
                {
                    itr = m_children.erase(itr);
                }
            }
            if (itr == m_children.end())
            {
                m_children.push_back(child);
            }
        }
    }
    
    if (mEntity->hasComponent<HLBatchComponent>())
    {
        mEntity->getComponent<HLBatchComponent>()->addChild(child);
    }
    
    // TODO post notification
}

void HLTransform2DComponent::removeChild(HLEntity* child, bool cleanup)
{
    if (!child)
    {
        return;
    }
    // TODO post notification
    
    if (mEntity->hasComponent<HLBatchComponent>())
    {
        mEntity->getComponent<HLBatchComponent>()->removeChild(child, cleanup);
    }
    

    if (child->hasComponent<HLTransform2DComponent>())
    {
        child->setProperty<HLEntity*>("parent", NULL);
    }
    for (std::list<HLEntity*>::iterator itr = m_children.begin(); itr != m_children.end(); )
    {
        if (child == *itr)
        {
            *itr = NULL;
            break;
        }
        else
        {
            itr++;
        }
    }

    if (cleanup)
    {
        mEntity->getEntityManager()->destroyEntity(child);
    }
}

void HLTransform2DComponent::removeFromParent(bool cleanup)
{
    if (!m_parent)
    {
        return;
    }
    HLTransform2DComponent *comp = m_parent->getComponent<HLTransform2DComponent>();
    HLASSERT(comp, "Parent should have nodegraph component");
    comp->removeChild(mEntity, cleanup);
}

void HLTransform2DComponent::removeAllChildren(bool cleanup)
{
    if (cleanup)
    {
        for (std::list<HLEntity*>::iterator itr = m_children.begin(); itr != m_children.end(); itr++)
        {
            if (*itr)
            {
                mEntity->getEntityManager()->destroyEntity(*itr);
//                 mEntity->getEntityManager()->addEntityToDestroy(*itr);
            }
        }
    }
    if (mEntity->hasComponent<HLBatchComponent>())
    {
        mEntity->getComponent<HLBatchComponent>()->removeAllChildren(cleanup);
    }
    m_children.clear();
}

HLEntity* HLTransform2DComponent::get_parent()
{
    return m_parent;
}

void HLTransform2DComponent::set_parent(HLEntity *entity)
{
    m_parent = entity;
}

std::list<HLEntity*> HLTransform2DComponent::get_children()
{
    return m_children;
}

kmMat4* HLTransform2DComponent::nodeToWorldTransform()
{
    kmMat4* transform = get_transform();
    kmMat4Assign(_nodeToWorldTransform, transform);

    HLEntity* parent = m_parent;
    if (parent != NULL)
    {
        if (parent->hasComponent<HLTransform2DComponent>())
        {
            kmMat4Multiply(_nodeToWorldTransform, parent->getComponent<HLTransform2DComponent>()->nodeToWorldTransform(),_nodeToWorldTransform);
            parent = parent->getProperty<HLEntity*>("parent");
        }
    }
    else
    {
        HLSceneManager* sceneMgr = mEntity->getEntityManager()->getSceneManager();
        kmMat4Multiply(_nodeToWorldTransform, _nodeToWorldTransform, sceneMgr->currentCameraMatrix());
    }
    return _nodeToWorldTransform;
}

kmMat4* HLTransform2DComponent::worldToNodeTransform()
{
    kmMat4Inverse(_worldToNodeTransform, nodeToWorldTransform());
    return _worldToNodeTransform;
}

HLPoint HLTransform2DComponent::convertToNodeSpace(const HLPoint &point)
{
    kmMat4* transform = worldToNodeTransform();
    return HLPoint((float)((double)transform->mat[0]*point.x+(double)transform->mat[4]*point.y+transform->mat[12]),
                   (float)((double)transform->mat[1]*point.x+(double)transform->mat[5]*point.y+transform->mat[13]));
}

HLPoint HLTransform2DComponent::convertToWorldSpace(const HLPoint &point)
{
    kmMat4* transform = nodeToWorldTransform();
    return HLPoint((float)((double)transform->mat[0]*point.x+(double)transform->mat[4]*point.y+transform->mat[12]),
                   (float)((double)transform->mat[1]*point.x+(double)transform->mat[5]*point.y+transform->mat[13]));
}

NS_HL_END
