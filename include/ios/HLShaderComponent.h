//
//  HLShaderComponent.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-1.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLSHADERCOMPONENT_H__
#define __HLSHADERCOMPONENT_H__

#include "HLEntitySystem.h"

NS_HL_BEGIN

class HLProgram;

typedef CDelegate1<HLProgram*> ShaderDrawHandler;

class HLShaderComponent: public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLShaderComponent, HLProgram*, shaderProgram)
public:
    HLShaderComponent():m_shaderProgram(NULL){}
    ~HLShaderComponent();
    ShaderDrawHandler drawCallback; // set uniforms
    
    FAMILYID
};

NS_HL_END

#endif
