//
//  HLCameraComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 12-8-16.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLCameraComponent.h"

#if GAME_PROJECTION == PROJECTION_3D

#include "HLScene.h"
#include "HLDirector3D.h"

NS_HL_BEGIN

INIT_FAMILYID(HLCameraComponent)

PROPERTIES_INITIAL(HLCameraComponent)
REGISTER_PROPERTY(HLCameraComponent, HLPoint3D, center)
REGISTER_PROPERTY(HLCameraComponent, HLPoint3D, up)
REGISTER_PROPERTY(HLCameraComponent, bool, isActive)

void HLCameraComponent::set_center(HLPoint3D center)
{
    m_center = center;
    if (m_isActive) 
    {
        
    }
}

HLPoint3D HLCameraComponent::get_center()
{
    return m_center;
}

void HLCameraComponent::set_up(HLPoint3D up)
{
    m_up = up;
    if (m_isActive) 
    {
        
    }
}

HLPoint3D HLCameraComponent::get_up()
{
    return m_up;
}

void HLCameraComponent::set_isActive(bool b)
{
    HLDirector3D::sharedDirector()->getRunningScene()->setActiveCamera(entity, b);
}

bool HLCameraComponent::get_isActive()
{
    return HLDirector3D::sharedDirector()->getRunningScene()->activeCamera == entity;
}

NS_HL_END

#endif
