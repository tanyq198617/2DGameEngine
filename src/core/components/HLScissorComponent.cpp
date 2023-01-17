//
//  HLScissorComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-5-29.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLScissorComponent.h"

NS_HL_BEGIN

INIT_FAMILYID(HLScissorComponent)

PROPERTIES_INITIAL(HLScissorComponent)
REGISTER_PROPERTY(HLScissorComponent, HLRect, scissorBox)

NS_HL_END
