//
//  HLGL.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-1.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLGL_H__
#define __HLGL_H__

#include "GameEngineConfig.h"

#if defined(TARGET_ANDROID)

#define glClearDepth                glClearDepthf
#define glDeleteVertexArrays        glDeleteVertexArraysOES
#define glGenVertexArrays           glGenVertexArraysOES
#define glBindVertexArray           glBindVertexArrayOES
#define GL_DEPTH24_STENCIL8         GL_DEPTH24_STENCIL8_OES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
typedef char GLchar;
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif
//declare here while define in EGLView_android.cpp
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT;
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT;
#define glGenVertexArraysOES glGenVertexArraysOESEXT
#define glBindVertexArrayOES glBindVertexArrayOESEXT
#define glDeleteVertexArraysOES glDeleteVertexArraysOESEXT

#elif defined(TARGET_IOS)

#define glClearDepth                glClearDepthf
#define glDeleteVertexArrays        glDeleteVertexArraysOES
#define glGenVertexArrays           glGenVertexArraysOES
#define glBindVertexArray           glBindVertexArrayOES
#define GL_DEPTH24_STENCIL8         GL_DEPTH24_STENCIL8_OES
#include <OpenGLES/ES2/gl.h>
#include <OPenGLES/ES2/glext.h>

#elif defined(TARGET_MACOS)

#define glDeleteVertexArrays        glDeleteVertexArraysAPPLE
#define glBindVertexArray           glBindVertexArrayAPPLE
#define glGenVertexArrays           glGenVertexArraysAPPLE
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#elif defined(TARGET_WIN32)
#include "GL/glew.h"

#endif

#include "HLGLStateCache.h"

#endif
