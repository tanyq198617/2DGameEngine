//
//  HLClippingComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-5-10.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLClippingComponent.h"

NS_HL_BEGIN

INIT_FAMILYID(HLClippingComponent)

PROPERTIES_INITIAL(HLClippingComponent)
REGISTER_PROPERTY(HLClippingComponent, HLEntity*, stencil)
REGISTER_PROPERTY(HLClippingComponent, bool, inverted)

extern GLint g_sStencilBits;
static bool g_sStencilBitsInit = false;
static int g_clippingCompCount = 0;

HLClippingComponent::HLClippingComponent():m_stencil(NULL),m_inverted(false)
{
    if (!g_sStencilBitsInit)
    {
        glGetIntegerv(GL_STENCIL_BITS, &g_sStencilBits);
        if (g_sStencilBits <= 0)
        {
            HLLOG("Stencil buffer is not enabled.");
        }
        g_sStencilBitsInit = true;
    }
    ++g_clippingCompCount;
}

HLClippingComponent::~HLClippingComponent()
{
    --g_clippingCompCount;
    if (g_clippingCompCount == 0)
    {
        g_sStencilBitsInit = false;
        g_sStencilBits = -1;
    }
}

NS_HL_END
