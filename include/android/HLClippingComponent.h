//
//  HLClippingComponent.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-5-10.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLCLIPPINGCOMPONENT_H__
#define __HLCLIPPINGCOMPONENT_H__

#include "HLEntitySystem.h"

NS_HL_BEGIN

class HLClippingComponent : public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE_IMPLEMENT(HLClippingComponent, HLEntity*, stencil)
//    PROPERTY_DECLARE(HLClippingComponent, float, alphaThreshold)
    PROPERTY_DECLARE_IMPLEMENT(HLClippingComponent, bool, inverted)
    
public:
    HLClippingComponent();
    ~HLClippingComponent();
    
public:
    FAMILYID
};

NS_HL_END

#endif /* defined(__HoolaiEngine__HLClippingComponent__) */
