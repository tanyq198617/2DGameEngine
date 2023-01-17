//
//  HLParticleExampleComponent.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-15.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLParticleExampleComponent.h"
#include "GameEngineConfig.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif

NS_HL_BEGIN

HLParticleComponent* HLParticleExampleComponent::getFireParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(-1);
    comp->set_emitterType(0);
    comp->set_gravity(HLPointZero);
    comp->set_radialAcceleration(0);
    comp->set_radialAccelVariance(0);
    comp->set_speed(60);
    comp->set_speedVariance(20);
    comp->set_angle(90);
    comp->set_angleVariance(10);
    comp->set_posVariance(HLPoint(40, 20));
    comp->set_lifespan(3);
    comp->set_lifespanVariance(0.25f);
    comp->set_startSize(54.0f);
    comp->set_startSizeVariance(10.0f);
    comp->set_endSize(-1);
    comp->set_emissionRate(numberOfParticles/3);
    color4F color = {0.76f, 0.25f, 0.12f, 1.0f};
    comp->set_startColor(color);
    color.r = color.b = color.g = color.a = 0.0f;
    comp->set_startColorVariance(color);
    color.a = 1.0f;
    comp->set_finishColor(color);
    color.a = 0.0f;
    comp->set_finishColorVariance(color);
    
    comp->setIsBlendAdditive(true);
    return comp;
}

HLParticleComponent* HLParticleExampleComponent::getFireworksParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(-1);
    comp->set_emitterType(0);
    comp->set_gravity(HLPoint(0,-90));
    comp->set_radialAcceleration(0);
    comp->set_radialAccelVariance(0);
    comp->set_speed(180);
    comp->set_speedVariance(50);
    comp->set_angle(90);
    comp->set_angleVariance(20);
    comp->set_posVariance(HLPointZero);
    comp->set_lifespan(3.5f);
    comp->set_lifespanVariance(1);
    comp->set_emissionRate(numberOfParticles/3.5f);
    color4F color = {0.5f, 0.5f, 0.5f, 1.0f};
    comp->set_startColor(color);
    comp->set_startColorVariance(color);
    color.r = color.g = color.b = 0.1f;
    color.a = 0.2f;
    comp->set_finishColor(color);
    comp->set_finishColorVariance(color);
    comp->set_startSize(8.0f);
    comp->set_startSizeVariance(2.0f);
    comp->set_endSize(-1);
    
    comp->setIsBlendAdditive(false);
    return comp;
}

HLParticleComponent* HLParticleExampleComponent::getSunParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(-1);
    comp->set_emitterType(0);
    comp->set_gravity(HLPointZero);
    comp->set_radialAcceleration(0);
    comp->set_radialAccelVariance(0);
    comp->set_speed(20);
    comp->set_speedVariance(5);
    comp->set_angle(90);
    comp->set_angleVariance(360);
    comp->set_posVariance(HLPointZero);
    comp->set_lifespan(1);
    comp->set_lifespanVariance(0.5f);
    comp->set_startSize(30.0f);
    comp->set_startSizeVariance(10.0f);
    comp->set_endSize(-1);
    comp->set_emissionRate(numberOfParticles);
    color4F color = {0.76f, 0.25f, 0.12f, 1.0f};
    comp->set_startColor(color);
    color.r = color.b = color.g = color.a = 0;
    comp->set_startColorVariance(color);
    comp->set_finishColorVariance(color);
    color.a = 1.0f;
    comp->set_finishColorVariance(color);
    
    comp->setIsBlendAdditive(true);
    return comp;
}

HLParticleComponent* HLParticleExampleComponent::getGalaxyParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(-1);
    comp->set_emitterType(0);
    comp->set_gravity(HLPointZero);
    comp->set_speed(60);
    comp->set_speedVariance(10);
    comp->set_radialAcceleration(-80);
    comp->set_radialAccelVariance(0);
    comp->set_tangentialAcceleration(80);
    comp->set_tangentialAccelVariance(0);
    comp->set_angle(90);
    comp->set_angleVariance(360);
    comp->set_posVariance(HLPointZero);
    comp->set_lifespan(4);
    comp->set_lifespanVariance(1);
    comp->set_startSize(37.0f);
    comp->set_startSizeVariance(10.0f);
    comp->set_endSize(-1);
    comp->set_emissionRate(numberOfParticles/4.0f);
    color4F color = {0.12f, 0.25f, 0.76f, 1.0f};
    comp->set_startColor(color);
    color.r = color.b = color.g = color.a = 0;
    comp->set_startColorVariance(color);
    comp->set_finishColorVariance(color);
    color.a = 1.0f;
    comp->set_finishColorVariance(color);
    
    comp->setIsBlendAdditive(true);
    return comp;
}

HLParticleComponent* HLParticleExampleComponent::getFlowerParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(-1);
    comp->set_emitterType(0);
    comp->set_gravity(HLPointZero);
    comp->set_speed(80);
    comp->set_speedVariance(10);
    comp->set_radialAcceleration(-60);
    comp->set_radialAccelVariance(0);
    comp->set_tangentialAcceleration(15);
    comp->set_tangentialAccelVariance(0);
    comp->set_angle(90);
    comp->set_angleVariance(360);
    comp->set_posVariance(HLPointZero);
    comp->set_lifespan(4);
    comp->set_lifespanVariance(1);
    comp->set_startSize(30.0f);
    comp->set_startSizeVariance(10.0f);
    comp->set_endSize(-1);
    comp->set_emissionRate(numberOfParticles/4.0f);
    color4F color = {0.5f, 0.5f, 0.5f, 1.0f};
    comp->set_startColor(color);
    color.a = 0.5f;
    comp->set_startColorVariance(color);
    color.r = color.b = color.g = 0.0f;
    color.a = 1.0f;
    comp->set_finishColor(color);
    color.a = 0.0f;
    comp->set_finishColorVariance(color);
    
    comp->setIsBlendAdditive(true);
    return comp;
}

HLParticleComponent* HLParticleExampleComponent::getMeteorParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(-1);
    comp->set_emitterType(0);
    comp->set_gravity(HLPoint(-200,200));
    comp->set_speed(15);
    comp->set_speedVariance(5);
    comp->set_radialAcceleration(0);
    comp->set_radialAccelVariance(0);
    comp->set_tangentialAcceleration(0);
    comp->set_tangentialAccelVariance(0);
    comp->set_angle(90);
    comp->set_angleVariance(360);
    comp->set_posVariance(HLPointZero);
    comp->set_lifespan(2);
    comp->set_lifespanVariance(1);
    comp->set_startSize(60.0f);
    comp->set_startSizeVariance(10.0f);
    comp->set_endSize(-1);
    comp->set_emissionRate(numberOfParticles/2.0f);
    color4F color = {0.2f, 0.4f, 0.7f, 1.0f};
    comp->set_startColor(color);
    color.r = color.g = 0.0f;
    color.b = 0.2f;
    color.a = 0.1f;
    comp->set_startColorVariance(color);
    color.b = 0.0f;
    color.a = 1.0f;
    comp->set_finishColor(color);
    color.a = 0.0f;
    comp->set_finishColorVariance(color);
    
    comp->setIsBlendAdditive(true);
    return comp;
}

HLParticleComponent* HLParticleExampleComponent::getSpiralParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(-1);
    comp->set_emitterType(0);
    comp->set_gravity(HLPointZero);
    comp->set_speed(150);
    comp->set_speedVariance(0);
    comp->set_radialAcceleration(-380);
    comp->set_radialAccelVariance(0);
    comp->set_tangentialAcceleration(45);
    comp->set_tangentialAccelVariance(0);
    comp->set_angle(90);
    comp->set_angleVariance(0);
    comp->set_posVariance(HLPointZero);
    comp->set_lifespan(12);
    comp->set_lifespanVariance(0);
    comp->set_startSize(20.0f);
    comp->set_startSizeVariance(0.0f);
    comp->set_endSize(-1);
    comp->set_emissionRate(numberOfParticles/12.0f);
    color4F color = {0.5f, 0.5f, 0.5f, 1.0f};
    comp->set_startColor(color);
    comp->set_finishColor(color);
    color.a = 0.0f;
    comp->set_startColorVariance(color);
    comp->set_finishColorVariance(color);
    
    comp->setIsBlendAdditive(false);
    return comp;
}

HLParticleComponent* HLParticleExampleComponent::getExplosionParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(0.1f);
    comp->set_emitterType(0);
    comp->set_gravity(HLPointZero);
    comp->set_speed(70);
    comp->set_speedVariance(40);
    comp->set_radialAcceleration(0);
    comp->set_radialAccelVariance(0);
    comp->set_tangentialAcceleration(0);
    comp->set_tangentialAccelVariance(0);
    comp->set_angle(90);
    comp->set_angleVariance(360);
    comp->set_posVariance(HLPointZero);
    comp->set_lifespan(5);
    comp->set_lifespanVariance(2);
    comp->set_startSize(15.0f);
    comp->set_startSizeVariance(10.0f);
    comp->set_endSize(-1);
    comp->set_emissionRate(numberOfParticles/0.1f);
    color4F color = {0.7f, 0.1f, 0.2f, 1.0f};
    comp->set_startColor(color);
    color.r = color.g = color.b = 0.5f;
    color.a = 0.0f;
    comp->set_startColorVariance(color);
    comp->set_finishColor(color);
    comp->set_finishColorVariance(color);
    
    comp->setIsBlendAdditive(false);
    return comp;
}

HLParticleComponent* HLParticleExampleComponent::getSmokeParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(-1);
    comp->set_emitterType(0);
    comp->set_gravity(HLPointZero);
    comp->set_speed(25);
    comp->set_speedVariance(10);
    comp->set_radialAcceleration(0);
    comp->set_radialAccelVariance(0);
    comp->set_tangentialAcceleration(0);
    comp->set_tangentialAccelVariance(0);
    comp->set_angle(90);
    comp->set_angleVariance(5);
    comp->set_posVariance(HLPointZero);
    comp->set_lifespan(4);
    comp->set_lifespanVariance(1);
    comp->set_startSize(60.0f);
    comp->set_startSizeVariance(10.0f);
    comp->set_endSize(-1);
    comp->set_emissionRate(numberOfParticles/4.0f);
    color4F color = {0.8f, 0.8f, 0.8f, 1.0f};
    comp->set_startColor(color);
    color.r = color.g = color.b = 0.02f;
    color.a = 0.0f;
    comp->set_startColorVariance(color);
    color.r = color.g = color.b = 0.0f;
    color.a = 1.0f;
    comp->set_finishColor(color);
    color.a = 0.0f;
    comp->set_finishColorVariance(color);
    
    comp->setIsBlendAdditive(false);
    return comp;
}

HLParticleComponent* HLParticleExampleComponent::getSnowParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(-1);
    comp->set_emitterType(0);
    comp->set_gravity(HLPoint(0,-1));
    comp->set_speed(5);
    comp->set_speedVariance(1);
    comp->set_radialAcceleration(0);
    comp->set_radialAccelVariance(1);
    comp->set_tangentialAcceleration(0);
    comp->set_tangentialAccelVariance(1);
    comp->set_angle(-90);
    comp->set_angleVariance(5);
#if GAME_PROJECTION == PROJECTION_2D
    HLSize winSize = HLDirector2D::getSingleton()->getWinSize();
#else
    HLSize winSize = HLDirector3D::sharedDirector()->getWinSize();
#endif
    comp->set_posVariance(HLPoint(winSize.width/2, 0));
    comp->set_lifespan(45);
    comp->set_lifespanVariance(15);
    comp->set_startSize(10.0f);
    comp->set_startSizeVariance(5.0f);
    comp->set_endSize(-1);
    comp->set_emissionRate(10);
    color4F color = {1.0f, 1.0f, 1.0f, 1.0f};
    comp->set_startColor(color);
    color.r = color.g = color.b = color.a = 0.0f;
    comp->set_startColorVariance(color);
    color.r = color.g = color.b = 1.0f;
    color.a = 0.0f;
    comp->set_finishColor(color);
    color.r = color.g = color.b = color.a = 0.0f;
    comp->set_finishColorVariance(color);
    
    comp->setIsBlendAdditive(false);
    return comp;
}

HLParticleComponent* HLParticleExampleComponent::getRainParticleComponent(unsigned int numberOfParticles)
{
    HLParticleComponent *comp = new HLParticleComponent(numberOfParticles);
    comp->set_duration(-1);
    comp->set_emitterType(0);
    comp->set_gravity(HLPoint(10,-10));
    comp->set_speed(130);
    comp->set_speedVariance(30);
    comp->set_radialAcceleration(0);
    comp->set_radialAccelVariance(1);
    comp->set_tangentialAcceleration(0);
    comp->set_tangentialAccelVariance(1);
    comp->set_angle(-90);
    comp->set_angleVariance(5);
#if GAME_PROJECTION == PROJECTION_2D
    HLSize winSize = HLDirector2D::getSingleton()->getWinSize();
#else
    HLSize winSize = HLDirector3D::sharedDirector()->getWinSize();
#endif
    comp->set_posVariance(HLPoint(winSize.width/2, 0));
    comp->set_lifespan(4.5f);
    comp->set_lifespanVariance(0);
    comp->set_startSize(4.0f);
    comp->set_startSizeVariance(2.0f);
    comp->set_endSize(-1);
    comp->set_emissionRate(20);
    color4F color = {0.7f, 0.8f, 1.0f, 1.0f};
    comp->set_startColor(color);
    color.r = color.g = color.b = color.a = 0.0f;
    comp->set_startColorVariance(color);
    color.r = 0.7; color.g = 0.8f; color.b = 1.0f;
    color.a = 0.5f;
    comp->set_finishColor(color);
    color.r = color.g = color.b = color.a = 0.0f;
    comp->set_finishColorVariance(color);
    
    comp->setIsBlendAdditive(false);
    return comp;
}

NS_HL_END

