//
//  HLColorAdvanceComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 3/4/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLColorAdvanceComponent.h"

NS_HL_BEGIN

INIT_FAMILYID(HLColorAdvanceComponent)

PROPERTIES_INITIAL(HLColorAdvanceComponent)
REGISTER_PROPERTY(HLColorAdvanceComponent, color4F, colorPercent)
REGISTER_PROPERTY(HLColorAdvanceComponent, color4F, colorAmount)

NS_HL_END
