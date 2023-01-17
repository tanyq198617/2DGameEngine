//
//  HLColorComponent.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-1.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLCOLORCOMPONENT_H__
#define __HLCOLORCOMPONENT_H__

#include "HLEntitySystem.h"
#include "HLGL.h"
#include "HLUtil.h"

NS_HL_BEGIN

class HLColorComponent: public HLComponent
{
	PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLColorComponent, color4B, color)
    
public:
    HLColorComponent() {m_color.r = m_color.g = m_color.b = m_color.a = 255;}

    FAMILYID
};

NS_HL_END

#endif
