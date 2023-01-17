//
//  HLGLRenderCommand.cpp
//  HoolaiEngine
//
//  Created by zhao shuan on 7/26/16.
//  Copyright © 2016 ICT. All rights reserved.
//

#include "HLGLRenderCommand.h"
#include "HLProgram.h"
#include "HLGLMatrixStack.h"

NS_HL_BEGIN

#define MAX_BUFFER_SIZE 4096

#ifdef DEBUG
extern int gDrawCall;
#endif

HLGLRenderCommand::HLGLRenderCommand()
{
    _polyCacheSize = 0;
    _polyCache = (V3F_C4B_T2F*)malloc(MAX_BUFFER_SIZE * sizeof(V3F_C4B_T2F));
    _indicesCache.reserve(MAX_BUFFER_SIZE*3);
}

HLGLRenderCommand& HLGLRenderCommand::get()
{
    static HLGLRenderCommand instance;
    return instance;
}

void HLGLRenderCommand::drawElements(GLenum mode, V3F_C4B_T2F* vertices, size_t size, std::vector<GLushort>& indices, bool useColorArray)
{
    while (currentIsMainThread())
    {
        if (_indicesCache.empty() && _polyCacheSize > 0)
        {
            flush();
        }
        else if (_currentMode && mode != _currentMode)
        {
            flush();
        }
        if (size > MAX_BUFFER_SIZE)
        {
            flush();
            break;
        }
        
        _currentMode = mode;
        if (_polyCacheSize + size > MAX_BUFFER_SIZE)
        {
            flush();
        }
        memcpy(_polyCache + _polyCacheSize, vertices, sizeof(V3F_C4B_T2F) * size);
        int size1 = _indicesCache.size();
        _indicesCache.insert(_indicesCache.end(), indices.begin(), indices.end());
        int size2 = _indicesCache.size();
        if (_polyCacheSize)
        {
            for (int i = size1; i < size2; ++i)
            {
                _indicesCache[i] += _polyCacheSize;
            }
        }
        convertToWorldCoordinates(_polyCache + _polyCacheSize, size);
        _polyCacheSize += size;
        if (useColorArray) {
            _useColorArray = true;
        }
        return;
    }
    CHECK_GL_ERROR();
    HLProgram::currentProgramInMainThread()->setUniformForModelViewProjectionMatrix();
    CHECK_GL_ERROR();
    
    hlglEnableVertexAttribArray( kVertexAttrib_Position );
    hlglEnableVertexAttribArray( kVertexAttrib_TexCoords );
    
    // 启用数组
    if (useColorArray)
    {
        hlglEnableVertexAttribArray( kVertexAttrib_Color );
    }
    else
    {
        hlglDisableVertexAttribArray( kVertexAttrib_Color );
    }
    
    // 设置数组属性
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), &vertices[0].vertices);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F),  &vertices[0].texCoords);
    
    if (useColorArray)
    {
        glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), &vertices[0].colors);
    }
    
    glDrawElements(mode, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
    
    CHECK_GL_ERROR();
    
#ifdef DEBUG
    if (currentIsMainThread())
        ++gDrawCall;
#endif
}

void HLGLRenderCommand::drawArray(GLenum mode, V3F_C4B_T2F* vertices, size_t size, bool useColorArray)
{
    while (currentIsMainThread())
    {
        if (!_indicesCache.empty())
        {
            flush();
        }
        else if (_currentMode && mode != _currentMode)
        {
            flush();
        }
        if (size > MAX_BUFFER_SIZE)
        {
            flush();
            break;
        }
        
        _currentMode = mode;
        if (_polyCacheSize + size > MAX_BUFFER_SIZE)
        {
            flush();
        }
        memcpy(_polyCache + _polyCacheSize, vertices, sizeof(V3F_C4B_T2F) * size);
        convertToWorldCoordinates(_polyCache + _polyCacheSize, size);
        _polyCacheSize += size;
        _indicesCache.clear();
        if (useColorArray) {
            _useColorArray = true;
        }
        return;
    }
    
    HLProgram::currentProgramInMainThread()->setUniformForModelViewProjectionMatrix();
    
    hlglEnableVertexAttribArray( kVertexAttrib_Position );
    hlglEnableVertexAttribArray( kVertexAttrib_TexCoords );
    
    // 启用数组
    if (useColorArray)
    {
        hlglEnableVertexAttribArray( kVertexAttrib_Color );
    }
    else
    {
        hlglDisableVertexAttribArray( kVertexAttrib_Color );
    }
    
    // 设置数组属性
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), &vertices[0].vertices);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F),  &vertices[0].texCoords);
    
    if (useColorArray)
    {
        glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), &vertices[0].colors);
    }
    
    glDrawArrays(mode, 0, size);
    
    CHECK_GL_ERROR();
    
#ifdef DEBUG
    if (currentIsMainThread())
        ++gDrawCall;
#endif
}

#if HL_USE_VAO
void HLGLRenderCommand::drawVAO(GLenum mode, GLuint vao, GLsizei size)
{
    flush();
    HLProgram::currentProgramInMainThread()->setUniformForModelViewProjectionMatrix();
    hlglBindVAO(vao);
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_SHORT, 0);
    hlglBindVAO( 0 );
    CHECK_GL_ERROR();
#ifdef DEBUG
    ++gDrawCall;
#endif
}
#endif

void HLGLRenderCommand::drawVBO(GLenum mode, GLuint buffer, GLuint elementBuffer, GLsizei size)
{
    flush();
    HLProgram::currentProgramInMainThread()->setUniformForModelViewProjectionMatrix();
#define kQuadSize sizeof(V3F_C4B_T2F)
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // vertices
    hlglEnableVertexAttribArray(kVertexAttrib_Position);
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, vertices));
    
    // colors
    hlglEnableVertexAttribArray(kVertexAttrib_Color);
    glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, colors));
    
    // tex coords
    hlglEnableVertexAttribArray(kVertexAttrib_TexCoords);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_SHORT, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR();
    
#ifdef DEBUG
    ++gDrawCall;
#endif
}

void HLGLRenderCommand::flush()
{
    if (!currentIsMainThread())
        return;
    if (!_polyCacheSize)
        return;
    
    kmMat4 matrixP;
    
    HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
    matrixStack->getMatrix(KM_GL_PROJECTION, &matrixP );
    
    HLProgram::currentProgramInMainThread()->setUniformForModelViewProjectionMatrix(&matrixP);
    
    hlglEnableVertexAttribArray( kVertexAttrib_Position );
    hlglEnableVertexAttribArray( kVertexAttrib_TexCoords );
    
    // 启用数组
    if (_useColorArray)
    {
        hlglEnableVertexAttribArray( kVertexAttrib_Color );
    }
    else
    {
        hlglDisableVertexAttribArray( kVertexAttrib_Color );
    }
    
    // 设置数组属性
    glVertexAttribPointer(kVertexAttrib_Position, _useZ?3:2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), &_polyCache[0].vertices);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F),  &_polyCache[0].texCoords);
    
    if (_useColorArray)
    {
        glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), &_polyCache[0].colors);
    }
    
    if (_indicesCache.size())
    {
        glDrawElements(_currentMode, _indicesCache.size(), GL_UNSIGNED_SHORT, &_indicesCache[0]);
    }
    else
    {
        glDrawArrays(_currentMode, 0, _polyCacheSize);
    }
    
    CHECK_GL_ERROR();
    
    HLProgram::currentProgramInMainThread()->setPrevUniformForModelViewProjectionMatrix();
    
    CHECK_GL_ERROR();
    
    _polyCacheSize = 0;
    _indicesCache.clear();
    
    _useColorArray = false;
    _useZ = false;
#ifdef DEBUG
    ++gDrawCall;
#endif
}

void HLGLRenderCommand::convertToWorldCoordinates(V3F_C4B_T2F* vertices, size_t size)
{
    kmMat4 matrixMV;
    HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
    matrixStack->getMatrix(KM_GL_MODELVIEW, &matrixMV);
    
    for (size_t i = 0; i < size; ++i)
    {
        auto & v = vertices[i].vertices;
        float x = matrixMV.mat[0]*v.x+matrixMV.mat[4]*v.y+matrixMV.mat[8]*v.z+matrixMV.mat[12];
        float y = matrixMV.mat[1]*v.x+matrixMV.mat[5]*v.y+matrixMV.mat[9]*v.z+matrixMV.mat[13];
        float z = matrixMV.mat[2]*v.x+matrixMV.mat[6]*v.y+matrixMV.mat[10]*v.z+matrixMV.mat[14];
        v.x = x;
        v.y = y;
        v.z = z;
        if (z)
            _useZ = true;
    }
}

NS_HL_END
