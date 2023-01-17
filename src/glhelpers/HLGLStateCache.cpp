//
//  HLGLESStateCache.cpp
//  HoolaiEngine
//
//  Created by tyq on 12-9-11.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLGL.h"
#include "GameEngineConfig.h"
#include <map>
#include <stdint.h>
#include "HLUtil.h"

NS_HL_BEGIN

#if ENABLE_GL_STATE_CACHE
GLuint _hlCurrentProgram = -1;
GLenum _hlGLBlendFunSFactor = 0;
GLenum _hlGLBlendFunDFactor = 0;
GLenum _hlGLActiveTexture = 0;
float _hlGLLineWidth = 1;
GLenum _hlGLBlendEquation = GL_FUNC_ADD;
GLuint _hlTextures[32] = {0};
std::map<GLenum, uint8_t> _hlGLEnabledFeatures;
GLint _scissorBox[4] = {0};
#endif

void hlglEnable(GLenum cap)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glEnable(cap);
    }
    else if ((_hlGLEnabledFeatures.find(cap)) == _hlGLEnabledFeatures.end())
    {
        glEnable(cap);
        _hlGLEnabledFeatures.insert(std::make_pair(cap, 1));
    }
#else
    glEnable(cap);
#endif
}

bool hlglIsEnabled(GLenum cap)
{
#if ENABLE_GL_STATE_CACHE
    if (! currentIsMainThread())
    {
        return glIsEnabled(cap);
    }
    return (_hlGLEnabledFeatures.find(cap)) != _hlGLEnabledFeatures.end();
#else
    return glIsEnabled(cap);
#endif
}

void hlglDisable(GLenum cap)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glDisable(cap);
    }
    else if ((_hlGLEnabledFeatures.find(cap)) != _hlGLEnabledFeatures.end())
    {
        glDisable(cap);
        _hlGLEnabledFeatures.erase(cap);
    }
#else
    glDisable(cap);
#endif
}

void hlglBlendFunc(GLenum sfactor, GLenum dfactor)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glBlendFunc(sfactor, dfactor);
    }
    else if (_hlGLBlendFunSFactor != sfactor || _hlGLBlendFunDFactor != dfactor)
    {
        glBlendFunc(sfactor, dfactor);
        _hlGLBlendFunSFactor = sfactor;
        _hlGLBlendFunDFactor = dfactor;
    }
#else
    glBlendFunc(sfactor, dfactor);
#endif
}

void hlglUseProgram(GLuint program)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glUseProgram(program);
    }
    else if (_hlCurrentProgram != program)
    {
        glUseProgram(program);
        _hlCurrentProgram = program;
    }
#else
    glUseProgram(program);
#endif
}

void hlglActiveTexture(GLenum texture)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glActiveTexture(texture);
    }
    else if (_hlGLActiveTexture != texture)
    {
        glActiveTexture(texture);
        _hlGLActiveTexture = texture;
    }
#else
    glActiveTexture(texture);
#endif
}

void hlglBindTexture(GLenum target, GLuint texture)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glBindTexture(target, texture);
    }
    else if (_hlTextures[_hlGLActiveTexture] != texture)
    {
        glBindTexture(target, texture);
        _hlTextures[_hlGLActiveTexture] = texture;
    }
#else
    glBindTexture(target, texture);
#endif
}

void hlglScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glScissor(x, y, width, height);
    }
    else if (_scissorBox[0]!=x||_scissorBox[1]!=y||_scissorBox[2]!=width||_scissorBox[3]!=height)
    {
        _scissorBox[0] = x;
        _scissorBox[1] = y;
        _scissorBox[2] = width;
        _scissorBox[3] = height;
        glScissor(x, y, width, height);
    }
#else
    glScissor(x, y, width, height);
#endif
}

void hlglGetScissorBox(GLint *out)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glGetIntegerv(GL_SCISSOR_BOX, out);
    }
    else
    {
        out[0] = _scissorBox[0];
        out[1] = _scissorBox[1];
        out[2] = _scissorBox[2];
        out[3] = _scissorBox[3];
    }
#else
    glGetIntegerv(GL_SCISSOR_BOX, out);
#endif
}

void hlglBlendEquation(GLenum mode)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glBlendEquation(mode);
    }
    else if (mode != _hlGLBlendEquation)
    {
        _hlGLBlendEquation = mode;
        glBlendEquation(mode);
    }
#else
    glBlendEquation(mode);
#endif
}

void hlglLineWidth(GLfloat width)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glLineWidth(width);
    }
    else if (width != _hlGLLineWidth)
    {
        glLineWidth(width);
        _hlGLLineWidth = width;
    }
#else
    glLineWidth(width);
#endif
}

NS_HL_END
