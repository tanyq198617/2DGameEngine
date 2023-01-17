//
//  HLBlendFuncComponent.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-1.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLBLENDFUNCCOMPONENT_H__
#define __HLBLENDFUNCCOMPONENT_H__

#include "HLEntitySystem.h"
#include "HLGL.h"

NS_HL_BEGIN

class HLBlendFuncComponent: public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE_IMPLEMENT(HLBlendFuncComponent, GLenum, sfactor)
    PROPERTY_DECLARE_IMPLEMENT(HLBlendFuncComponent, GLenum, dfactor)
    PROPERTY_DECLARE_IMPLEMENT(HLBlendFuncComponent, GLenum, blendEquation)
    PROPERTY_DECLARE_IMPLEMENT(HLBlendFuncComponent, bool, preMultiAlpha)
    
public:
    HLBlendFuncComponent():m_sfactor(GL_SRC_ALPHA), m_dfactor(GL_ONE_MINUS_SRC_ALPHA), m_blendEquation(GL_FUNC_ADD), m_preMultiAlpha(false) {}
    
    FAMILYID
};

NS_HL_END

#endif
