//
//  HLCameraComponent.h
//  HoolaiEngine
//
//  Created by tyq on 12-8-16.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __HLCAMERACOMPONENT_H__
#define __HLCAMERACOMPONENT_H__

#include "GameEngineConfig.h"

#if GAME_PROJECTION == PROJECTION_3D
#include "HLEntitySystem.h"

NS_HL_BEGIN

class HLCameraComponent : public HLComponent
{
    friend class HLSceneManager;
    
	PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLCameraComponent, HLPoint3D, center)
    PROPERTY_DECLARE(HLCameraComponent, HLPoint3D, up)
    PROPERTY_DECLARE(HLCameraComponent, bool, isActive)
    
private:
    HLEntity *entity;
    
public:
    HLCameraComponent():m_center(0,0,0),m_up(0,1,0),m_isActive(false), entity(NULL) {}
    
    virtual void onAddedToEntity(HLEntity* entity)
    {
        HLASSERT(!this->entity, "camera already added to entity");
        this->entity = entity;
    }
    
    virtual void onRemovedFromEntity(HLEntity* entity)
    {
        entity = NULL;
    }
    
public:
    static const FamilyId familyId;
};

NS_HL_END

#endif

#endif
