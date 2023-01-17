//
//  HLDrawingPrimitives.h
//  HoolaiEngine
//
//  Created by tyq on 12-9-26.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __HLDRAWINGPRIMITIVES_H__
#define __HLDRAWINGPRIMITIVES_H__

#include "HLMacros.h"
#include "HLGL.h"
#include "HLUtil.h"

NS_HL_BEGIN

class HLPoint;
//class HLUtil;

void drawLine(const HLPoint& origin, const HLPoint& dest);
void drawRect(const HLPoint&  origin, const HLPoint&  dest);
void drawSolidRect(HLPoint origin, HLPoint dest);
void drawSolidPoly(const HLPoint* poli, unsigned int numberOfPoints);
void drawPoly(const HLPoint *poli, unsigned int numberOfPoints, bool closePolygon);
void drawSolidCircle( const HLPoint& center, float radius, float angle, unsigned int segments);
void drawCircle( const HLPoint& center, float radius, float angle, unsigned int segments, bool drawLineToCenter);
void drawColor4F(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void drawColor4F(color4F color);
void drawColor4B(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
void drawColor4B(color4B color);

NS_HL_END

#endif /* defined(__HLDRAWINGPRIMITIVES_H__) */
