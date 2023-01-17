//
//  HLPrimitiveComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 4/9/15.
//  Copyright (c) 2015 ICT. All rights reserved.
//

#include "HLPrimitiveComponent.h"
#include "HLDrawingPrimitives.h"

NS_HL_BEGIN

INIT_FAMILYID(HLPrimitiveComponent)

PROPERTIES_INITIAL(HLPrimitiveComponent)
REGISTER_PROPERTY(HLPrimitiveComponent, std::vector<HLPoint>, verts)
REGISTER_PROPERTY(HLPrimitiveComponent, float, radius)
REGISTER_PROPERTY(HLPrimitiveComponent, int, drawType)
REGISTER_PROPERTY(HLPrimitiveComponent, unsigned int, segments)

void HLPrimitiveComponent::onDeactive()
{
    mEntity->onDraw -= newDelegate(this, &HLPrimitiveComponent::draw);
}

void HLPrimitiveComponent::onActive()
{
    mEntity->onDraw += newDelegate(this, &HLPrimitiveComponent::draw);
    mColorComp = mEntity->getComponent<HLColorComponent>();
}

void HLPrimitiveComponent::draw()
{
    if (!mEntity)
    {
        return;
    }
    
    if (mColorComp)
    {
        drawColor4B(mColorComp->get_color());
    }
    else
    {
        drawColor4B(0, 0, 0, 255);
    }
    
    if (m_drawType == kDrawTypeLine && m_verts.size() > 1)
    {
        drawLine(m_verts[0], m_verts[1]);
    }
    else if (m_drawType == kDrawTypeRect && m_verts.size() > 1)
    {
        drawRect(m_verts[0], m_verts[1]);
    }
    else if (m_drawType == kDrawTypeSolidRect && m_verts.size() > 1)
    {
        drawSolidRect(m_verts[0], m_verts[1]);
    }
    else if (m_drawType == kDrawTypePoly && m_verts.size() > 1)
    {
        drawPoly(&m_verts[0], m_verts.size(), true);
    }
    else if (m_drawType == kDrawTypeSolidCircle)
    {
        if (m_verts.empty())
            drawSolidCircle(HLPointZero, m_radius, 0, m_segments);
        else
            drawSolidCircle(m_verts[0], m_radius, 0, m_segments);
    }
    else if (m_drawType == kDrawTypeCircle)
    {
        if (m_verts.empty())
            drawCircle(HLPointZero, m_radius, 0, m_segments, false);
        else
            drawCircle(m_verts[0], m_radius, 0, m_segments, false);
    }
}

NS_HL_END
