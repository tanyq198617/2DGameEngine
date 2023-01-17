//
//  HLColorComponent.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-1.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLColorComponent.h"
#include "HLUtil.h"
#include "HLBatchComponent.h"


NS_HL_BEGIN

//const FamilyId HLColorComponent::familyId = 5;
INIT_FAMILYID(HLColorComponent)

PROPERTIES_INITIAL(HLColorComponent)
REGISTER_PROPERTY(HLColorComponent, color4B, color)

void HLColorComponent::set_color(color4B color)
{
    m_color = color;
    
    if (mEntity->hasProperty("useBatch"))
    {
        if (mEntity->getProperty<bool>("useBatch"))
        {
            mEntity->getProperty<HLEntity*>("parent")->getComponent<HLBatchComponent>()->updateQuad(mEntity);
        }
    }
}

color4B HLColorComponent::get_color()
{
    return m_color;
}

NS_HL_END
