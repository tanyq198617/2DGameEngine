//
//  HLTransform3DComponent.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-8-13.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLTransform3DComponent.h"

#if GAME_PROJECTION == PROJECTION_3D

#include "HLDirector3D.h"

NS_HL_BEGIN

INIT_FAMILYID(HLTransform3DComponent)

PROPERTIES_INITIAL(HLTransform3DComponent)
REGISTER_PROPERTY(HLTransform3DComponent, HLPoint3D, position)
REGISTER_PROPERTY(HLTransform3DComponent, float, scale)
REGISTER_PROPERTY(HLTransform3DComponent, float, scaleX)
REGISTER_PROPERTY(HLTransform3DComponent, float, scaleY)
REGISTER_PROPERTY(HLTransform3DComponent, float, scaleZ)
REGISTER_PROPERTY(HLTransform3DComponent, float, rotationX)
REGISTER_PROPERTY(HLTransform3DComponent, float, rotationY)
REGISTER_PROPERTY(HLTransform3DComponent, float, rotationZ)
REGISTER_PROPERTY_READONLY(HLTransform3DComponent, kmMat4*, transform)

inline 
HLPoint3D HLTransform3DComponent::get_position()
{
    return m_position;
}

inline
void HLTransform3DComponent::set_position(HLPoint3D pos)
{
    if (m_position != pos) 
    {
        m_position = pos;
        _isTransformGLDirty = true;
    }
}

inline
float HLTransform3DComponent::get_scale()
{
    HLASSERT(m_scaleX == m_scaleY == m_scaleZ, "get_scale, ScaleX , ScaleY and ScaleZ are not equal. Don't know which one to return");
    return m_scaleX;
}

inline
void HLTransform3DComponent::set_scale(float scale)
{
    if (m_scaleX != scale) 
    {
        m_scaleX = m_scaleY = m_scaleZ = scale;
        _isTransformGLDirty = true;
    }
}

inline
float HLTransform3DComponent::get_scaleX()
{
    return m_scaleX;
}

inline
void HLTransform3DComponent::set_scaleX(float scale)
{
    m_scaleX = scale;
}

inline
float HLTransform3DComponent::get_scaleY()
{
    return m_scaleY;
}

inline
void HLTransform3DComponent::set_scaleY(float scale)
{
    m_scaleY = scale;
}

inline
float HLTransform3DComponent::get_scaleZ()
{
    return m_scaleZ;
}

inline
void HLTransform3DComponent::set_scaleZ(float scale)
{
    m_scaleZ = scale;
}

inline
void HLTransform3DComponent::set_rotationX(float rotation)
{
    if (m_rotationX != rotation) 
    {
        m_rotationX = rotation;
        _isTransformGLDirty = true;
    }
}

inline
float HLTransform3DComponent::get_rotationX()
{
    return m_rotationX;
}

inline
void HLTransform3DComponent::set_rotationY(float rotation)
{
    if (m_rotationY != rotation) 
    {
        m_rotationY = rotation;
        _isTransformGLDirty = true;
    }
}

inline
float HLTransform3DComponent::get_rotationY()
{
    return m_rotationY;
}

inline
void HLTransform3DComponent::set_rotationZ(float rotation)
{
    if (m_rotationZ != rotation) 
    {
        m_rotationZ = rotation;
        _isTransformGLDirty = true;
    }
}

inline
float HLTransform3DComponent::get_rotationZ()
{
    return m_rotationZ;
}

inline
kmMat4* HLTransform3DComponent::get_transform()
{
    if (_isTransformGLDirty) 
    {
        kmMat4Identity(m_transform);
        // transform
        m_transform->mat[12] = m_position.x;
        m_transform->mat[13] = m_position.y;
        m_transform->mat[14] = m_position.z;
        // scale
        m_transform->mat[0] = m_scaleX;
        m_transform->mat[5] = m_scaleY;
        m_transform->mat[10] = m_scaleZ;
        
        kmMat4 mat;
        kmMat4RotationX(&mat, -DEGREES_TO_RADIANS(m_rotationX));
        kmMat4Multiply(m_transform, m_transform, &mat);
        kmMat4RotationY(&mat, -DEGREES_TO_RADIANS(m_rotationY));
        kmMat4Multiply(m_transform, m_transform, &mat);
        kmMat4RotationZ(&mat, -DEGREES_TO_RADIANS(m_rotationZ));
        kmMat4Multiply(m_transform, m_transform, &mat);
        
        _isTransformGLDirty = false;
    }
    return m_transform;
}

NS_HL_END

#endif
