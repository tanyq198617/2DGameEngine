//
//  HLDrawingPrimitives.cpp
//  HoolaiEngine
//
//  Created by tyq on 12-9-26.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLDrawingPrimitives.h"
#include "HLGeometry.h"
#include "HLProgram.h"

NS_HL_BEGIN

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

static bool initialized = false;
static HLProgram *shader_ = NULL;
static HLProgramUniform* colorUniform = NULL;
static color4F color_ = {1,1,1,1};
static HLProgramUniform* pointSizeUniform = NULL;
static GLfloat pointSize_ = 1;

static void lazy_init( void )
{
    if( ! initialized ) {
        
        //
        // Position and 1 color passed as a uniform (to similate glColor4ub )
        //
        shader_ = HLProgramManager::currentManager()->getProgram(kShaderPosition_uColor);
        
        std::vector<HLProgramUniform>&v = shader_->getUniforms();
        for (std::vector<HLProgramUniform>::iterator it = v.begin(); it != v.end(); ++it)
        {
            HLProgramUniform* u = &(*it);
            if (!strcmp(u->name, kUniformColor))
            {
                colorUniform = u;
            }
            else if (!strcmp(u->name, kUniformPointSize))
            {
                pointSizeUniform = u;
            }
        }
        shader_->use();
        pointSizeUniform->setUniform1f(pointSize_);
        
        initialized = true;
    }
}

void drawLine(const HLPoint& origin, const HLPoint& dest)
{
    lazy_init();
    
    GLfloat vertices[4] = {origin.x, origin.y, dest.x, dest.y};
    shader_->use();
    CHECK_GL_ERROR();
    shader_->setUniformForModelViewProjectionMatrix();
    CHECK_GL_ERROR();
    colorUniform->setUniform4fv((GLfloat*) &color_.r, 1);
    CHECK_GL_ERROR();
    
    glEnableVertexAttribArray(kVertexAttrib_Position);
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    CHECK_GL_ERROR();
    glDrawArrays(GL_LINES, 0, 2);
}

void drawRect(const HLPoint&  origin, const HLPoint&  dest)
{
    drawLine(HLPoint(origin.x, origin.y), HLPoint(dest.x, origin.y));
    drawLine(HLPoint(dest.x, origin.y), HLPoint(dest.x, dest.y));
    drawLine(HLPoint(dest.x, dest.y), HLPoint(origin.x, dest.y));
    drawLine(HLPoint(origin.x, dest.y), HLPoint(origin.x, origin.y));
}

void drawSolidRect(HLPoint origin, HLPoint dest)
{
    HLPoint vertices[] = {origin, HLPoint(dest.x, origin.y), dest, HLPoint(origin.x, dest.y)};
    drawSolidPoly(vertices, 4);
}

void drawSolidPoly(const HLPoint* poli, unsigned int numberOfPoints)
{
    lazy_init();
    
    shader_->use();
    shader_->setUniformForModelViewProjectionMatrix();
    colorUniform->setUniform4fv((GLfloat*) &color_.r, 1);
    
    glEnableVertexAttribArray(kVertexAttrib_Position);
    GLfloat* newPoli = new GLfloat[numberOfPoints*2];
    
    if (sizeof(HLPoint) == sizeof(GLfloat)*2)
    {
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, poli);
    }
    else
    {
        for (unsigned int i=0; i<numberOfPoints;i++)
        {
            newPoli[i*2] = poli[i].x;
            newPoli[i*2+1] = poli[i].y;
        }
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, newPoli);
    }
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) numberOfPoints);
    
    delete newPoli;
    
}

void drawPoly(const HLPoint *poli, unsigned int numberOfPoints, bool closePolygon)
{
    lazy_init();
    
    shader_->setUniformForModelViewProjectionMatrix();
    colorUniform->setUniform4fv((GLfloat*) &color_.r, 1);
    
    glEnableVertexAttribArray(kVertexAttrib_Position);
    GLfloat* newPoli = new GLfloat[numberOfPoints*2];
    
    if (sizeof(HLPoint) == sizeof(GLfloat)*2)
    {
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, poli);
    }
    else
    {
        for (unsigned int i=0; i<numberOfPoints;i++)
        {
            newPoli[i*2] = poli[i].x;
            newPoli[i*2+1] = poli[i].y;
        }
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, newPoli);
    }
    
    if (closePolygon)
    {
        glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) numberOfPoints);
    }
    else
    {
        glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) numberOfPoints);
    }
    
    delete newPoli;
}

void drawCircle( const HLPoint& center, float radius, float angle, unsigned int segments, bool drawLineToCenter)
{
    lazy_init();
    
    int additionalSegment = 1;
    if (drawLineToCenter)
        additionalSegment++;
    
    const float coef = 2.0f * (float)M_PI/segments;
    
    GLfloat *vertices = (GLfloat*)calloc( sizeof(GLfloat)*2*(segments+2), 1);
    if( ! vertices )
        return;
    
    for(unsigned int i = 0;i <= segments; i++) {
        float rads = i*coef;
        GLfloat j = radius * cosf(rads + angle) + center.x;
        GLfloat k = radius * sinf(rads + angle) + center.y;
        
        vertices[i*2] = j;
        vertices[i*2+1] = k;
    }
    vertices[(segments+1)*2] = center.x;
    vertices[(segments+1)*2+1] = center.y;
    
    shader_->use();
    shader_->setUniformForModelViewProjectionMatrix();
    colorUniform->setUniform4fv((GLfloat*) &color_.r, 1);
    
    glEnableVertexAttribArray(kVertexAttrib_Position);
    
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments+additionalSegment);
    
    free( vertices );
}

void drawSolidCircle( const HLPoint& center, float radius, float angle, unsigned int segments)
{
    const float coef = 2.0f * (float)M_PI/segments;
    
    GLfloat *vertices = (GLfloat*)calloc( sizeof(GLfloat)*2*(segments+2), 1);
    if( ! vertices )
        return;
    
    for(unsigned int i = 0;i <= segments; i++) {
        float rads = i*coef;
        GLfloat j = radius * cosf(rads + angle) + center.x;
        GLfloat k = radius * sinf(rads + angle) + center.y;
        
        vertices[i*2] = j;
        vertices[i*2+1] = k;
    }
    vertices[(segments+1)*2] = center.x;
    vertices[(segments+1)*2+1] = center.y;
    
    drawSolidPoly((HLPoint *)vertices, segments);
    
    free( vertices );
}

void drawColor4F(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    color_.r = r;
    color_.g = g;
    color_.b = b;
    color_.a = a;
}

void drawColor4B(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    color_.r = r/255.0f;
    color_.g = g/255.0f;
    color_.b = b/255.0f;
    color_.a = a/255.0f;
}

void drawColor4F(color4F color)
{
    color_ = color;
}

void drawColor4B(color4B color)
{
    color_.r = color.r/255.0f;
    color_.g = color.g/255.0f;
    color_.b = color.b/255.0f;
    color_.a = color.a/255.0f;
}

NS_HL_END
