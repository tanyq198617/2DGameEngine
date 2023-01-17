//
//  HLGLESStateCache.h
//  HoolaiEngine
//
//  Created by tyq on 12-9-11.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLGLESSTATECACHE_H__
#define __HLGLESSTATECACHE_H__

#include "HLMacros.h"

NS_HL_BEGIN

void hlglEnable(GLenum cap);
bool hlglIsEnabled(GLenum cap);
void hlglDisable(GLenum cap);
void hlglBlendFunc(GLenum sfactor, GLenum dfactor);
void hlglUseProgram(GLuint program);
void hlglActiveTexture(GLenum texture);
void hlglBindTexture(GLenum target, GLuint texture);
void hlglScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void hlglGetScissorBox(GLint *out);
void hlglBlendEquation(GLenum mode);
void hlglLineWidth(GLfloat width);

NS_HL_END

#endif
