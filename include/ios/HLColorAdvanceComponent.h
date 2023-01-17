//
//  HLColorAdvanceComponent.h
//  HoolaiEngine
//
//  Created by zhao shuan on 3/4/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLCOLORADVANCECOMPONENT_H__
#define __HLCOLORADVANCECOMPONENT_H__

#include "HLEntitySystem.h"
#include "HLGL.h"
#include "HLUtil.h"

NS_HL_BEGIN

class HLColorAdvanceComponent : public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE_IMPLEMENT(HLColorAdvanceComponent, color4F, colorPercent)
    PROPERTY_DECLARE_IMPLEMENT(HLColorAdvanceComponent, color4F, colorAmount)
    
public:
    HLColorAdvanceComponent() {m_colorPercent.r = m_colorPercent.g = m_colorPercent.b = m_colorPercent.a = 1; m_colorAmount.r = m_colorAmount.g = m_colorAmount.b = m_colorAmount.a = 0;}

    FAMILYID
};

NS_HL_END

#endif
