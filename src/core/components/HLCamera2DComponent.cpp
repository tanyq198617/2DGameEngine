//
//  HLCamera2DComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 2/7/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLCamera2DComponent.h"
#include "HLSceneManager.h"

#if GAME_PROJECTION == PROJECTION_2D

NS_HL_BEGIN

INIT_FAMILYID(HLCamera2DComponent)

PROPERTIES_INITIAL(HLCamera2DComponent)
REGISTER_PROPERTY(HLCamera2DComponent, bool, active)
REGISTER_PROPERTY(HLCamera2DComponent, bool, animation)
REGISTER_PROPERTY(HLCamera2DComponent, HLRect, cameraRect)

HLCamera2DComponent::HLCamera2DComponent():m_animation(false) {}

void HLCamera2DComponent::set_active(bool active)
{
    mEntity->getEntityManager()->getSceneManager()->setActiveCamera(mEntity, active, m_animation);
}

bool HLCamera2DComponent::get_active()
{
    return mEntity->getEntityManager()->getSceneManager()->getActiveCamera() == mEntity;
}

void HLCamera2DComponent::onDeactive()
{
    mEntity->getEntityManager()->getSceneManager()->setActiveCamera(mEntity, false);
    
}
NS_HL_END

#endif
