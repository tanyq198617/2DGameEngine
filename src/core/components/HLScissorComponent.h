//
//  HLScissorComponent.h
//  HoolaiEngine
//
//  Created by tyq on 13-5-29.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLSCISSORCOMPONENT_H__
#define __HLSCISSORCOMPONENT_H__

#include "HLEntitySystem.h"

NS_HL_BEGIN

class HLScissorComponent : public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE_IMPLEMENT(HLScissorComponent, HLRect, scissorBox)
    
public:
    HLScissorComponent() {}
    
public:
    FAMILYID
};

NS_HL_END

#endif 
