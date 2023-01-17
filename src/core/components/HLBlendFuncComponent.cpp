//
//  HLBlendFuncComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-3-1.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLBlendFuncComponent.h"

NS_HL_BEGIN

INIT_FAMILYID(HLBlendFuncComponent)

PROPERTIES_INITIAL(HLBlendFuncComponent)
REGISTER_PROPERTY(HLBlendFuncComponent, GLenum, sfactor)
REGISTER_PROPERTY(HLBlendFuncComponent, GLenum, dfactor)
REGISTER_PROPERTY(HLBlendFuncComponent, GLenum, blendEquation)
REGISTER_PROPERTY(HLBlendFuncComponent, bool, preMultiAlpha)

NS_HL_END
