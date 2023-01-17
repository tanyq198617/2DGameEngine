//
//  HLPrimitiveComponent.h
//  HoolaiEngine
//
//  Created by zhao shuan on 4/9/15.
//  Copyright (c) 2015 ICT. All rights reserved.
//

#ifndef __HLPRIMITIVE_H__
#define __HLPRIMITIVE_H__

#include "HLEntitySystem.h"
#include "HLColorComponent.h"

NS_HL_BEGIN

enum HLPrimitiveDrawType{
    kDrawTypeLine = 0,
    kDrawTypeRect,
    kDrawTypeSolidRect,
    kDrawTypePoly,
    kDrawTypeSolidCircle,
    kDrawTypeCircle
};

class HLPrimitiveComponent : public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE_IMPLEMENT(HLPrimitiveComponent, std::vector<HLPoint>, verts)
    PROPERTY_DECLARE_IMPLEMENT(HLPrimitiveComponent, float, radius)
    PROPERTY_DECLARE_IMPLEMENT(HLPrimitiveComponent, int, drawType)
    PROPERTY_DECLARE_IMPLEMENT(HLPrimitiveComponent, unsigned int, segments)
    
public:
    HLPrimitiveComponent():m_radius(1),m_drawType(kDrawTypeLine), m_segments(20) {}
    
    virtual void onDeactive();
    
    virtual void onActive();
    
    void draw();
public:
    FAMILYID
private:
    HLColorComponent* mColorComp;
};

NS_HL_END

#endif
