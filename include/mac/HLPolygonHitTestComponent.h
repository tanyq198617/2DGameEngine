//
//  HLPolygonHitTestComponent.h
//  HoolaiEngine
//
//  Created by tyq on 13-2-6.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLPOLYGONHITTESTCOMPONENT_H__
#define __HLPOLYGONHITTESTCOMPONENT_H__

#include "HLEntitySystem.h"

NS_HL_BEGIN

class HLPolygonHitTestComponent:public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLPolygonHitTestComponent, std::string, outlinefile)
    PROPERTY_DECLARE_IMPLEMENT(HLPolygonHitTestComponent, std::vector<HLPoint>, outlinepoints)
    
public:
    
    virtual void onActive();
    virtual void onDeactive();
    
    void hitTest(kmMat4* transform, const HLPoint& point, bool* ret);
private:

    FAMILYID
};

NS_HL_END

#endif
