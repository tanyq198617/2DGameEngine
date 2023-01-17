//
//  HLEventType.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-12-11.
//  Copyright (c) 2013年 hoolai. All rights reserved.
//

#ifndef __HLEVENTTYPE_H__
#define __HLEVENTTYPE_H__

#include "HLMacros.h"
#include "HLGeometry.h"
#include <vector>
#include <stdint.h>

NS_HL_BEGIN

typedef struct {
    int64_t _id;
    HLPoint location;
    int tapCount;
} HLTouch;

typedef enum {
    kHLTouchesBegin = 1,
    kHLTouchesMove,
    kHLTouchesEnd,
    kHLTouchesCancel
} HLTouchEventType;

class HLTouchEvent
{
public:
    int _id;
    HLTouchEventType type;
    std::vector<HLTouch> touches;
};

typedef enum {
    kHLMouseDown = 1,
    kHLMouseMove,
    kHLMouseDrag,
    kHLMouseUp,
    kHLRightMouseDown,
    kHLRightMouseDrag,
    kHLRightMouseUp,
    kHLMouseCancel
} HLMouseEventType;

class HLMouseEvent
{
public:
    int _id;
    HLMouseEventType type;
    HLPoint location;
    uint32_t clickCount;
};

NS_HL_END

#endif
