//
//  HLBMFontComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 12-8-28.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLBMFontComponent.h"
#include "HLProgram.h"

using namespace std;

NS_HL_BEGIN

INIT_FAMILYID(HLBMFontComponent)

PROPERTIES_INITIAL(HLBMFontComponent)
REGISTER_PROPERTY(HLBMFontComponent, std::string, fontfile);
REGISTER_PROPERTY(HLBMFontComponent, std::string, text);

void HLBMFontComponent::onActive()
{
    mEntity->onDraw += newDelegate(this, &HLBMFontComponent::draw);
}

void HLBMFontComponent::onDeactive()
{
    mEntity->onDraw -= newDelegate(this, &HLBMFontComponent::draw);
    
}

string HLBMFontComponent::get_fontfile()
{
    return m_fontfile;
}

void HLBMFontComponent::set_fontfile(string fontfile)
{
    if (m_fontfile != fontfile)
    {
        m_fontfile = fontfile;
        if (bmfont)
        {
            bmfont->release();
        }
        bmfont = HLBMFont::getFont(fontfile);
        bmfont->retain();
    }
}

void HLBMFontComponent::draw()
{
    if (!bmfont)
    {
        return;
    }
    
    if (mEntity&&mEntity->hasProperty("color"))
    {
        color4B color = mEntity->getProperty<color4B>("color");
        bmfont->setColor(color.r, color.g, color.b, color.a);
    }
    
    HLProgram *program = HLProgramManager::currentManager()->getProgram(kShaderPositionTextureColor);
    program->use();
    program->setUniformForModelViewProjectionMatrix();
    hlglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    program->getUniforms()[0].setUniform1f(1.f); // brightness
    bmfont->print(0, 0, m_text.c_str());
    CHECK_GL_ERROR();
}

NS_HL_END
