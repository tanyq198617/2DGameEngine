//
//  HLCamera2DComponent.h
//  HoolaiEngine
//
//  Created by tyq on 2/7/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLCAMERA2DCOMPONENT_H__
#define __HLCAMERA2DCOMPONENT_H__

#include "GameEngineConfig.h"

#if GAME_PROJECTION == PROJECTION_2D

#include "HLEntitySystem.h"

NS_HL_BEGIN

class HLCamera2DComponent : public HLComponent
{
public:
    HLCamera2DComponent();
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLCamera2DComponent, bool, active)
    PROPERTY_DECLARE_IMPLEMENT(HLCamera2DComponent, bool, animation)
    PROPERTY_DECLARE_IMPLEMENT(HLCamera2DComponent, HLRect, cameraRect)
    
    virtual void onDeactive();
    
    FAMILYID
};

NS_HL_END

#endif

#endif
