//
//  HLGLRenderCommand.hpp
//  HoolaiEngine
//
//  Created by zhao shuan on 7/26/16.
//  Copyright © 2016 ICT. All rights reserved.
//

#pragma once
#include "HLMacros.h"
#include "HLUtil.h"
#include <vector>
#include "HLProgram.h"

NS_HL_BEGIN

class HLGLRenderCommand
{
public:
    HLGLRenderCommand();
    static HLGLRenderCommand& get();
    void drawElements(GLenum mode, V3F_C4B_T2F* vertices, size_t size, std::vector<GLushort>& indices, bool useColorArray = true);
    void drawArray(GLenum mode, V3F_C4B_T2F* vertices, size_t size, bool useColorArray = true);
#if HL_USE_VAO
    void drawVAO(GLenum mode, GLuint vao, GLsizei size);
#endif
    void drawVBO(GLenum mode, GLuint buffer, GLuint elementBuffer, GLsizei size);
    void flush();
private:
    void convertToWorldCoordinates(V3F_C4B_T2F* vertices, size_t size);
    V3F_C4B_T2F *_polyCache;
    int _polyCacheSize;
    std::vector<GLushort> _indicesCache;
    GLenum _currentMode{0};
    bool _useColorArray{false};
    bool _useZ{false};
};

NS_HL_END
