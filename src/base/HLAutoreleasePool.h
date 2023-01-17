//
//  HLAutoreleasePool.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-4.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLAUTORELEASEPOOL_H__
#define __HLAUTORELEASEPOOL_H__

#include "HLMacros.h"
#include "HLObject.h"
#include <list>

NS_HL_BEGIN

class HLAutoreleasePool
{
private:
    std::list<HLObject*> managedObjects;
public:
    HLAutoreleasePool();
    ~HLAutoreleasePool();
    static HLAutoreleasePool * currentPool();
    void purgePool();
    void addObject(HLObject *);
};

NS_HL_END

#endif
