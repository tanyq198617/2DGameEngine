//
//  HLParticleComponent.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-15.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLParticleComponent.h"
#include "HLProgram.h"
#include "HLTransform2DComponent.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif


NS_HL_BEGIN

INIT_FAMILYID(HLParticleComponent)

PROPERTIES_INITIAL(HLParticleComponent)
REGISTER_PROPERTY(HLParticleComponent, float, angle)
REGISTER_PROPERTY(HLParticleComponent, float, angleVariance)
REGISTER_PROPERTY(HLParticleComponent, float, duration)
REGISTER_PROPERTY(HLParticleComponent, int, blendFuncSource)
REGISTER_PROPERTY(HLParticleComponent, int, blendFuncDestination)

REGISTER_PROPERTY(HLParticleComponent, color4F, startColor)
REGISTER_PROPERTY(HLParticleComponent, color4F, startColorVariance)
REGISTER_PROPERTY(HLParticleComponent, color4F, finishColor)
REGISTER_PROPERTY(HLParticleComponent, color4F, finishColorVariance)

REGISTER_PROPERTY(HLParticleComponent, float, startSize)
REGISTER_PROPERTY(HLParticleComponent, float, startSizeVariance)
REGISTER_PROPERTY(HLParticleComponent, float, endSize)
REGISTER_PROPERTY(HLParticleComponent, float, endSizeVariance)

REGISTER_PROPERTY(HLParticleComponent, HLPoint, posVariance)

REGISTER_PROPERTY(HLParticleComponent, float, rotationStart)
REGISTER_PROPERTY(HLParticleComponent, float, rotationStartVariance)
REGISTER_PROPERTY(HLParticleComponent, float, rotationEnd)
REGISTER_PROPERTY(HLParticleComponent, float, rotationEndVariance)
REGISTER_PROPERTY(HLParticleComponent, int, emitterType)

//typeA: Gravity + tangential accel + radial accel
REGISTER_PROPERTY(HLParticleComponent, HLPoint, gravity)
REGISTER_PROPERTY(HLParticleComponent, float, speed)
REGISTER_PROPERTY(HLParticleComponent, float, speedVariance)
REGISTER_PROPERTY(HLParticleComponent, float, radialAcceleration)
REGISTER_PROPERTY(HLParticleComponent, float, radialAccelVariance)
REGISTER_PROPERTY(HLParticleComponent, float, tangentialAcceleration)
REGISTER_PROPERTY(HLParticleComponent, float, tangentialAccelVariance)

//typeB: radius movement
REGISTER_PROPERTY(HLParticleComponent, float, startRadius)
REGISTER_PROPERTY(HLParticleComponent, float, startRadiusVariance)
REGISTER_PROPERTY(HLParticleComponent, float, endRadius)
REGISTER_PROPERTY(HLParticleComponent, float, endRadiusVariance)
REGISTER_PROPERTY(HLParticleComponent, float, rotatePerSecond)
REGISTER_PROPERTY(HLParticleComponent, float, rotatePerSecondVariance)

REGISTER_PROPERTY(HLParticleComponent, float, lifespan)
REGISTER_PROPERTY(HLParticleComponent, float, lifespanVariance)
REGISTER_PROPERTY(HLParticleComponent, float, emissionRate)

REGISTER_PROPERTY(HLParticleComponent, HLTexture*, texture)

void HLParticleComponent::onActive()
{
    mEntity->onDraw += newDelegate(this, &HLParticleComponent::draw);
    mEntity->onUpdate += newDelegate(this, &HLParticleComponent::update);
}

void HLParticleComponent::onDeactive()
{
    mEntity->onDraw -= newDelegate(this, &HLParticleComponent::draw);
    mEntity->onUpdate -= newDelegate(this, &HLParticleComponent::update);
    
}

void HLParticleComponent::draw()
{
    if (!m_texture) 
        return;
    
//    int scaleFactor = CONTENT_SCALE_FACTOR;
    
    HLProgram *program = HLProgramManager::currentManager()->getProgram(kShaderPositionTextureColor);
    program->use();
//    program->getUniforms()[0].setUniform1f(1.0f);
    
//    if (scaleFactor > 1) {
//        kmGLPushMatrix();
//        kmMat4 scale;
//        kmMat4Identity(&scale);
//        kmMat4Scaling(&scale, scaleFactor, scaleFactor, 0);
//        kmGLMultMatrix(&scale);
//    }
    
    program->setUniformForModelViewProjectionMatrix();
    
    std::vector<HLProgramUniform>& uniforms = program->getUniforms();
    for (std::vector<HLProgramUniform>::iterator it = uniforms.begin(); it != uniforms.end(); ++it)
    {
        HLProgramUniform& uniform = *it;
        if (!strcmp(uniform.name, kUniformBrightness))
        {
            uniform.setUniform1f(1);
        }
    }
    
//    if (scaleFactor > 1) {
//        kmGLPopMatrix();
//    }
    
    hlglBindTexture(GL_TEXTURE_2D, m_texture->getName());
    hlglBlendFunc(m_blendFuncSource, m_blendFuncDestination);
    
    HLASSERT(_particleIdx == _particleCount, "Abnormal error in particle quad");
#if HL_USE_VAO
    glBindVertexArray(_VAOName);
    
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glDrawElements(GL_TRIANGLES, (GLsizei)_particleIdx*6, GL_UNSIGNED_SHORT, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glBindVertexArray( 0 );
#else
    #define kQuadSize sizeof(_quads[0].bl)
    
    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    // vertices
    glEnableVertexAttribArray(kVertexAttrib_Position);
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, vertices));
    
    // colors
    glEnableVertexAttribArray(kVertexAttrib_Color);
    glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, colors));
    
    // tex coords
    glEnableVertexAttribArray(kVertexAttrib_TexCoords);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glDrawElements(GL_TRIANGLES, (GLsizei) _particleIdx*6, GL_UNSIGNED_SHORT, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
#endif
    CHECK_GL_ERROR();
}

void HLParticleComponent::update(float dt)
{
    if (m_isActive && m_emissionRate) 
    {
        float rate = 1.0f / m_emissionRate;
        if (_particleCount < _totalParticles)
        {
            _emitCounter += dt;
        }
        
        while (_particleCount < _totalParticles && _emitCounter > rate) 
        {
            this->addParticle(mEntity);
            _emitCounter -= rate;
        }
        
        if (m_duration != -1)
        {
            _elapsed += dt;
            if (m_duration < _elapsed)
                stopSystem();
        }
    }
    
    _particleIdx = 0;
    
    HLPoint currentPosition = HLPointZero;
    if (mEntity->hasProperty("position"))
    {
        currentPosition = mEntity->getProperty<HLPoint>("position");
    }
    
    if (mEntity->isVisible()) 
    {
        while (_particleIdx < _particleCount) 
        {
            tParticle* p = &_particles[_particleIdx];
            
            p->timeToLive -= dt;
            
            if (p->timeToLive > 0) 
            {
                if (m_emitterType == 0) 
                {
                    HLPoint tmp, radial, tangential;
                    
                    radial = HLPointZero;
                    // radial acceleration
                    if (p->pos.x || p->pos.y)
                    {
                        radial = p->pos.normalize();
                    }
                    tangential = radial;
                    radial = radial * p->modeA.radialAccel;
                    
                    // tangential acceleration
                    float newy = tangential.x;
                    tangential.x = -tangential.y;
                    tangential.y = newy;
                    tangential = tangential * p->modeA.tangentialAccel;
                    
                    // (gravity + radial + tangential) * dt
                    tmp = radial + tangential + m_gravity;
                    tmp = tmp * dt;
                    p->modeA.dir = p->modeA.dir + tmp;
                    tmp = p->modeA.dir * dt;
                    p->pos = p->pos + tmp;
                }
                else 
                {
                    // Update the angle and radius of the particle.
                    p->modeB.angle += p->modeB.degreesPerSecond * dt;
                    p->modeB.radius += p->modeB.deltaRadius * dt;
                    
                    p->pos.x = - cosf(p->modeB.angle) * p->modeB.radius;
                    p->pos.y = - sinf(p->modeB.angle) * p->modeB.radius;
                }
                
                // color
                p->color.r += (p->deltaColor.r * dt);
                p->color.g += (p->deltaColor.g * dt);
                p->color.b += (p->deltaColor.b * dt);
                p->color.a += (p->deltaColor.a * dt);
                
                // size
                p->size += (p->deltaSize * dt);
                p->size = MAX( 0, p->size );
                
                // angle
                p->rotation += (p->deltaRotation * dt);
                
                //
                // update values in quad
                //
                
                HLPoint newPos;
                
                HLPoint diff = currentPosition - p->startPos;
                newPos = p->pos - diff;
                
                updateQuadWithParticle(p, newPos);
                
                _particleIdx++;
            }
            else 
            {
                // life < 0
                if( _particleIdx != _particleCount-1 )
                {
                    _particles[_particleIdx] = _particles[_particleCount-1];
                }
                _particleCount--;
                
                // TODO auto remove on finish
            }
        }
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0] );
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(_quads[0])*_particleCount, _quads);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR();
}

void HLParticleComponent::updateQuadWithParticle(tParticle* particle, const HLPoint& newPosition)
{
    V3F_C4B_T2F_Quad *quad;
    quad = &_quads[_particleIdx];
    quad->bl.colors.r = (GLubyte)(particle->color.r * 255);
    quad->bl.colors.g = (GLubyte)(particle->color.g * 255);
    quad->bl.colors.b = (GLubyte)(particle->color.b * 255);
    quad->bl.colors.a = (GLubyte)(particle->color.a * 255);
    quad->br.colors = quad->bl.colors;
    quad->tl.colors = quad->bl.colors;
    quad->tr.colors = quad->bl.colors;
    
    GLfloat size_2 = particle->size/2;
    if (particle->rotation) 
    {
        GLfloat x1 = -size_2;
        GLfloat y1 = -size_2;
        
        GLfloat x2 = size_2;
        GLfloat y2 = size_2;
        GLfloat x = newPosition.x;
        GLfloat y = newPosition.y;
        
        GLfloat r = (GLfloat)-DEGREES_TO_RADIANS(particle->rotation);
        GLfloat cr = cosf(r);
        GLfloat sr = sinf(r);
        GLfloat ax = x1 * cr - y1 * sr + x;
        GLfloat ay = x1 * sr + y1 * cr + y;
        GLfloat bx = x2 * cr - y1 * sr + x;
        GLfloat by = x2 * sr + y1 * cr + y;
        GLfloat cx = x2 * cr - y2 * sr + x;
        GLfloat cy = x2 * sr + y2 * cr + y;
        GLfloat dx = x1 * cr - y2 * sr + x;
        GLfloat dy = x1 * sr + y2 * cr + y;
        
        // bottom-left
        quad->bl.vertices.x = ax;
        quad->bl.vertices.y = ay;
        
        // bottom-right vertex:
        quad->br.vertices.x = bx;
        quad->br.vertices.y = by;
        
        // top-left vertex:
        quad->tl.vertices.x = dx;
        quad->tl.vertices.y = dy;
        
        // top-right vertex:
        quad->tr.vertices.x = cx;
        quad->tr.vertices.y = cy;
    }
    else 
    {
        // bottom-left vertex:
        quad->bl.vertices.x = newPosition.x - size_2;
        quad->bl.vertices.y = newPosition.y - size_2;
        
        // bottom-right vertex:
        quad->br.vertices.x = newPosition.x + size_2;
        quad->br.vertices.y = newPosition.y - size_2;
        
        // top-left vertex:
        quad->tl.vertices.x = newPosition.x - size_2;
        quad->tl.vertices.y = newPosition.y + size_2;
        
        // top-right vertex:
        quad->tr.vertices.x = newPosition.x + size_2;
        quad->tr.vertices.y = newPosition.y + size_2; 
    }
}

bool HLParticleComponent::allocMemory()
{
    if (_quads) free(_quads);
    if (_indices) free(_indices);
    
    _quads = (V3F_C4B_T2F_Quad*)malloc(_totalParticles*sizeof(V3F_C4B_T2F_Quad));
    _indices = (GLushort*)malloc(_totalParticles*6*sizeof(GLushort));
    if (!_quads || !_indices) {
        // not enough memory
        return false;
    }
    memset(_quads, 0, _totalParticles*sizeof(V3F_C4B_T2F_Quad));
    memset(_indices, 0, _totalParticles*6*sizeof(GLushort));
    return true;
}

void HLParticleComponent::setupIndices()
{
    for (int i = 0; i < _totalParticles; i++) 
    {
        const unsigned int i6 = i*6;
        const unsigned int i4 = i*4;
        _indices[i6+0] = (GLushort) i4+0;
        _indices[i6+1] = (GLushort) i4+1;
        _indices[i6+2] = (GLushort) i4+2;
        
        _indices[i6+5] = (GLushort) i4+1;
        _indices[i6+4] = (GLushort) i4+2;
        _indices[i6+3] = (GLushort) i4+3;
    }
}

#if HL_USE_VAO
void HLParticleComponent::setupVBOandVAO()
{
    glGenVertexArrays(1, &_VAOName);
    glBindVertexArray(_VAOName);
    
#define kQuadSize sizeof(_quads[0].bl)
    
    glGenBuffers(2, &_buffersVBO[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_quads[0]) * _totalParticles, _quads, GL_DYNAMIC_DRAW);
    
    // vertices
    glEnableVertexAttribArray(kVertexAttrib_Position);
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, vertices));
    
    // colors
    glEnableVertexAttribArray(kVertexAttrib_Color);
    glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, colors));
    
    // tex coords
    glEnableVertexAttribArray(kVertexAttrib_TexCoords);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * _totalParticles * 6, _indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR();
}
#else
void HLParticleComponent::setupVBO()
{
    glGenBuffers(2, &_buffersVBO[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_quads[0]) * _totalParticles, _quads, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * _totalParticles * 6, _indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR();
}
#endif

void HLParticleComponent::initParticle(HLEntity* entity, tParticle* particle)
{
    // timeToLive
    // no negative life. prevent division by 0
    particle->timeToLive = m_lifespan + m_lifespanVariance * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
    particle->timeToLive = MAX(0, particle->timeToLive);
    
    // position
    particle->pos.x = m_posVariance.x * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
    
    particle->pos.y = m_posVariance.y + m_posVariance.y * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
    
    // Color
    color4F start;
    start.r = clampf(m_startColor.r + m_startColorVariance.r * ((2.0f*((float)rand()/RAND_MAX))-1.0f), 0, 1);
    start.g = clampf(m_startColor.g + m_startColorVariance.g * ((2.0f*((float)rand()/RAND_MAX))-1.0f), 0, 1);
    start.b = clampf(m_startColor.b + m_startColorVariance.b * ((2.0f*((float)rand()/RAND_MAX))-1.0f), 0, 1);
    start.a = clampf(m_startColor.a + m_startColorVariance.a * ((2.0f*((float)rand()/RAND_MAX))-1.0f), 0, 1);
    
    color4F end;
    end.r = clampf(m_finishColor.r + m_finishColorVariance.r * ((2.0f*((float)rand()/RAND_MAX))-1.0f), 0, 1);
    end.g = clampf(m_finishColor.g + m_finishColorVariance.g * ((2.0f*((float)rand()/RAND_MAX))-1.0f), 0, 1);
    end.b = clampf(m_finishColor.b + m_finishColorVariance.b * ((2.0f*((float)rand()/RAND_MAX))-1.0f), 0, 1);
    end.a = clampf(m_finishColor.a + m_finishColorVariance.a * ((2.0f*((float)rand()/RAND_MAX))-1.0f), 0, 1);
    
    particle->color = start;
    particle->deltaColor.r = (end.r - start.r) / particle->timeToLive;
    particle->deltaColor.g = (end.g - start.g) / particle->timeToLive;
    particle->deltaColor.b = (end.b - start.b) / particle->timeToLive;
    particle->deltaColor.a = (end.a - start.a) / particle->timeToLive;
    
    // size
    float startS = m_startSize + m_startSizeVariance * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
    startS = MAX(0, startS); // No negative value
    
    particle->size = startS;
    
    if (m_endSize == -1) 
    {
        particle->deltaSize = 0;
    }
    else
    {
        float endS = m_endSize + m_endSizeVariance * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
        endS = MAX(0, endS); // No negative values
        particle->deltaSize = (endS - startS) / particle->timeToLive;
    }
    
    // rotation
    float startA = m_rotationStart + m_rotationStartVariance * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
    float endA = m_rotationEnd + m_rotationEndVariance * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
    particle->rotation = startA;
    particle->deltaRotation = (endA - startA) / particle->timeToLive;
    
    if(entity->hasProperty("position"))
    {
        particle->startPos = entity->getProperty<HLPoint>("position");
    }
    
    // direction
    float a = DEGREES_TO_RADIANS( m_angle + m_angleVariance * ((2.0f*((float)rand()/RAND_MAX))-1.0f) );
    
    if (m_emitterType == 0) 
    {
        HLPoint v(cosf( a ), sinf( a ));
        float s = m_speed + m_speedVariance * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
        
        // direction
        particle->modeA.dir = v * s;
        
        // radial accel
        particle->modeA.radialAccel = m_radialAcceleration + m_radialAccelVariance * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
        
        
        // tangential accel
        particle->modeA.tangentialAccel = m_tangentialAcceleration + m_tangentialAccelVariance * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
    }
    else
    {
        float startRadius = m_startRadius + m_startRadiusVariance * ((2.0f*((float)rand()/RAND_MAX))-1.0f);
        float endRadius = m_endRadius + m_endRadiusVariance *((2.0f*((float)rand()/RAND_MAX))-1.0f);
        
        particle->modeB.radius = startRadius;
        
        if(m_endRadius == -1)
        {
            particle->modeB.deltaRadius = 0;
        }
        else
        {
            particle->modeB.deltaRadius = (endRadius - startRadius) / particle->timeToLive;
        }
        
        particle->modeB.angle = a;
        particle->modeB.degreesPerSecond = DEGREES_TO_RADIANS(m_rotatePerSecond + m_rotatePerSecond * ((2.0f*((float)rand()/RAND_MAX))-1.0f));
    }
}

void HLParticleComponent::set_texture(HLTexture* texture)
{
    texture->retain();
    if (m_texture) 
    {
        m_texture->release();
    }
    m_texture = texture;
    initTexCoordsWithRect(HLRect(0,0,texture->getImageSize().width, texture->getImageSize().height));
}

HLTexture* HLParticleComponent::get_texture()
{
    return m_texture;
}

void HLParticleComponent::initTexCoordsWithRect(const HLRect& pointRect)
{
    HLRect rect = pointRect;
    GLfloat wide = (GLfloat)pointRect.size.width;
    GLfloat high = (GLfloat)pointRect.size.height;
    if (m_texture) 
    {
        wide = m_texture->getImageSize().width;
        high = m_texture->getImageSize().height;
    }
    GLfloat left = rect.origin.x / wide;
    GLfloat top = rect.origin.y / high;
    GLfloat right = left + rect.size.width / wide;
    GLfloat bottom = top + rect.size.height / high;
    
    V3F_C4B_T2F_Quad *quads = NULL;
    unsigned int start = 0, end = 0;
    
    quads = _quads;
    start = 0;
    end = _totalParticles;
    
    for(unsigned int i=start; i<end; i++) 
    {
        // bottom-left vertex:
        quads[i].bl.texCoords.u = left;
        quads[i].bl.texCoords.v = bottom;
        // bottom-right vertex:
        quads[i].br.texCoords.u = right;
        quads[i].br.texCoords.v = bottom;
        // top-left vertex:
        quads[i].tl.texCoords.u = left;
        quads[i].tl.texCoords.v = top;
        // top-right vertex:
        quads[i].tr.texCoords.u = right;
        quads[i].tr.texCoords.v = top;
    }
}

//void HLParticleComponent::set_speed(float speed)
//{
//    m_speed = speed * CONTENT_SCALE_FACTOR;
//}
//
//float HLParticleComponent::get_speed()
//{
//    return m_speed / CONTENT_SCALE_FACTOR;
//}
//
//void HLParticleComponent::set_posVariance(HLPoint posVariance)
//{
//    m_posVariance = posVariance * CONTENT_SCALE_FACTOR;
//}
//
//HLPoint HLParticleComponent::get_posVariance()
//{
//    return m_posVariance / CONTENT_SCALE_FACTOR;
//}
//
//void HLParticleComponent::set_startSize(float startSize)
//{
//    m_startSize = startSize * CONTENT_SCALE_FACTOR;
//}
//
//float HLParticleComponent::get_startSize()
//{
//    return m_startSize / CONTENT_SCALE_FACTOR;
//}
//
//void HLParticleComponent::set_endSize(float endSize)
//{
//    m_endSize = endSize * CONTENT_SCALE_FACTOR;
//}
//
//float HLParticleComponent::get_endSize()
//{
//    return m_endSize / CONTENT_SCALE_FACTOR;
//}

NS_HL_END
