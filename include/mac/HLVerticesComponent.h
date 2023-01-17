//
//  HLVerticesComponent.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-6-4.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLVERTICESCOMPONENT_H__
#define __HLVERTICESCOMPONENT_H__

#include "HLEntitySystem.h"

NS_HL_BEGIN

class HLVerticesComponent: public HLComponent
{
	PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLVerticesComponent, float*, vertices)
    PROPERTY_DECLARE(HLVerticesComponent, int, vertexsize)
    PROPERTY_DECLARE(HLVerticesComponent, int, vertexcount)
    
public:
    HLVerticesComponent():m_vertices(NULL), m_vertexsize(0), m_vertexcount(0) {}
    
    ~HLVerticesComponent()
    {
        if (m_vertices) {
            free(m_vertices);
        }
    }
    
    FAMILYID
};

NS_HL_END

#endif
