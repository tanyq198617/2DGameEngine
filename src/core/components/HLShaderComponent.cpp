//
//  HLShaderComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-3-1.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLShaderComponent.h"
#include "HLProgram.h"

NS_HL_BEGIN

INIT_FAMILYID(HLShaderComponent)

PROPERTIES_INITIAL(HLShaderComponent)
REGISTER_PROPERTY(HLShaderComponent, HLProgram*, shaderProgram)

HLShaderComponent::~HLShaderComponent()
{
    if (m_shaderProgram)
    {
        m_shaderProgram->release();
    }
}

void HLShaderComponent::set_shaderProgram(HLProgram* program)
{
    program->retain();
    if (m_shaderProgram)
    {
        m_shaderProgram->release();
    }
    m_shaderProgram = program;
}

HLProgram* HLShaderComponent::get_shaderProgram()
{
    return m_shaderProgram;
}

NS_HL_END
