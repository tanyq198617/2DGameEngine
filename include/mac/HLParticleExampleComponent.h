//
//  HLParticleExampleComponent.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-15.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __HLPARTICLEEXAMPLECOMPONENT_H__
#define __HLPARTICLEEXAMPLECOMPONENT_H__
#include "HLParticleComponent.h"

NS_HL_BEGIN

class HLParticleExampleComponent
{
public:
    static HLParticleComponent* getFireParticleComponent(unsigned int numberOfParticles = 250);
    static HLParticleComponent* getFireworksParticleComponent(unsigned int numberOfParticles = 1500);
    static HLParticleComponent* getSunParticleComponent(unsigned int numberOfParticles = 350);
    static HLParticleComponent* getGalaxyParticleComponent(unsigned int numberOfParticles = 200);
    static HLParticleComponent* getFlowerParticleComponent(unsigned int numberOfParticles = 250);
    static HLParticleComponent* getMeteorParticleComponent(unsigned int numberOfParticles = 150);
    static HLParticleComponent* getSpiralParticleComponent(unsigned int numberOfParticles = 500);
    static HLParticleComponent* getExplosionParticleComponent(unsigned int numberOfParticles = 700);
    static HLParticleComponent* getSmokeParticleComponent(unsigned int numberOfParticles = 200);
    static HLParticleComponent* getSnowParticleComponent(unsigned int numberOfParticles = 700);
    static HLParticleComponent* getRainParticleComponent(unsigned int numberOfParticles = 1000);
};

NS_HL_END

#endif
