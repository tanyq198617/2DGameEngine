//
//  HLParticleComponent.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-15.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __HLPARTICLECOMPONENT_H__
#define __HLPARTICLECOMPONENT_H__

#include "HLEntitySystem.h"
#include "HLTexture.h"
#include "GameEngineConfig.h"

NS_HL_BEGIN

typedef struct sParticle {
    HLPoint     pos;
    HLPoint     startPos;
    
    color4F    color;
    color4F    deltaColor;
    
    float        size;
    float        deltaSize;
    
    float        rotation;
    float        deltaRotation;
    
    float        timeToLive;
    
    unsigned int    atlasIndex;
    
    //! Mode A: gravity, direction, radial accel, tangential accel
    struct {
        HLPoint        dir;
        float        radialAccel;
        float        tangentialAccel;
    } modeA;
    
    //! Mode B: radius mode
    struct {
        float        angle;
        float        degreesPerSecond;
        float        radius;
        float        deltaRadius;
    } modeB;
    
}tParticle;

class HLParticleComponent: public HLComponent
{
	PROPERTIES_DECLARE
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, bool, isActive)
    
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, angle)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, angleVariance)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, duration)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, int, blendFuncSource)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, int, blendFuncDestination)
    
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, color4F, startColor)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, color4F, startColorVariance)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, color4F, finishColor)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, color4F, finishColorVariance)
    
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, startSize)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, startSizeVariance)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, endSize)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, endSizeVariance)
    
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, HLPoint, posVariance)
    
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, rotationStart)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, rotationStartVariance)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, rotationEnd)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, rotationEndVariance)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, int, emitterType)
    
    //typeA: Gravity + tangential accel + radial accel
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, HLPoint, gravity)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, speed)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, speedVariance)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, radialAcceleration)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, radialAccelVariance)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, tangentialAcceleration)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, tangentialAccelVariance)
    
    //typeB: radius movement
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, startRadius)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, startRadiusVariance)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, endRadius)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, endRadiusVariance)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, rotatePerSecond)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, rotatePerSecondVariance)
    
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, lifespan)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, lifespanVariance)
    PROPERTY_DECLARE_IMPLEMENT(HLParticleComponent, float, emissionRate)
    
    PROPERTY_DECLARE(HLParticleComponent, HLTexture*, texture)

public:
    // create with config file
    // to be implemented
    HLParticleComponent(const char* file){}
    
    HLParticleComponent(unsigned int numberOfParticles):_quads(NULL),_indices(NULL),m_isActive(true), m_angle(0), m_angleVariance(0),_particleCount(0),m_texture(NULL), m_rotationStart(0), m_rotationStartVariance(0), m_rotationEnd(0), m_rotationEndVariance(0), m_emitterType(0), m_speed(0), m_speedVariance(0), m_radialAcceleration(0), m_radialAccelVariance(0), m_tangentialAcceleration(0), m_tangentialAccelVariance(0), m_startRadius(0), m_startRadiusVariance(0), m_endRadius(0), m_endRadiusVariance(0), m_rotatePerSecond(0), m_rotatePerSecondVariance(0), m_lifespan(0), m_lifespanVariance(0), m_emissionRate(0), _elapsed(0), _emitCounter(0)
#if HL_USE_VAO
    , _VAOName(0)
#endif
    {
        _totalParticles=numberOfParticles;
        _particles = (tParticle*)calloc(_totalParticles, sizeof(tParticle));
        _allocatedParticles = numberOfParticles;
        m_blendFuncSource = GL_ONE;
        m_blendFuncDestination = GL_ONE_MINUS_SRC_ALPHA;
        if (!allocMemory())
            return;
        setupIndices();
#if HL_USE_VAO
        setupVBOandVAO();
#else
        setupVBO();
#endif
        // Need to listen the event only when not use batchnode, because it will use VBO
        
    }
    
    virtual ~HLParticleComponent()
    {
        if (m_texture) 
            m_texture->release();
        if (_particles) 
            free(_particles);
        if (_quads)
            free(_quads);
        if (_indices)
            free(_indices);
        
        glDeleteBuffers(2, &_buffersVBO[0]);
#if HL_USE_VAO
        glDeleteVertexArrays(1, &_VAOName);
#endif
    }
    
    virtual void onActive();
    virtual void onDeactive();
    
    void draw();
    void update(float dt);
    
    void setIsBlendAdditive(bool additive)
    {
        if( additive )
        {
            m_blendFuncSource = GL_SRC_ALPHA;
            m_blendFuncDestination = GL_ONE;
        }
        else
        {
//            if( m_texture && ! m_texture->getHasPremultipliedAlpha() )
//            {
                m_blendFuncSource = GL_SRC_ALPHA;
                m_blendFuncDestination = GL_ONE_MINUS_SRC_ALPHA;
//            }
//            else 
//            {
//                m_blendFuncSource = GL_ONE;
//                m_blendFuncDestination = GL_ONE_MINUS_SRC_ALPHA;
//            }
        }
    }
    
    bool getIsBlendAdditive()
    {
        return( m_blendFuncSource == GL_SRC_ALPHA && m_blendFuncDestination == GL_ONE);
    }
    
private:
    bool allocMemory();
    void setupIndices();
#if HL_USE_VAO
    void setupVBOandVAO();
#else
    void setupVBO();
#endif
    
    void updateQuadWithParticle(tParticle* particle, const HLPoint& newPosition);
    
    bool addParticle(HLEntity* entity)
    {
        if (_particleCount == _totalParticles) 
        {
            return false;
        }
        tParticle *particle = &_particles[_particleCount];
        initParticle(entity, particle);
        ++_particleCount;
        return true;
    }
    
    void initParticle(HLEntity* entity, tParticle* particle);
    
    void stopSystem()
    {
        m_isActive = false;
        _elapsed = m_duration;
        _emitCounter = 0;
    }
    
    void initTexCoordsWithRect(const HLRect& pointRect);
    
    unsigned int _totalParticles;
    unsigned int _allocatedParticles;
    unsigned int _particleCount;
    unsigned int _particleIdx;
    tParticle *_particles;
    GLushort* _indices;
    V3F_C4B_T2F_Quad *_quads;
    float _emitCounter;
    float _elapsed;
#if HL_USE_VAO
    GLuint _VAOName;
#endif
    GLuint _buffersVBO[2]; // 0: vertex 1: indeces

    FAMILYID
};

NS_HL_END

#endif
