//
//  HLSpriteComponent.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-28.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLSpriteComponent.h"
#include "HLProgram.h"
#include "GameEngineConfig.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "HLTexture.h"
#include "HLColorComponent.h"
#include "HLUtil.h"
#include "HLAnimateSystem.h"
#include "HLTransform2DComponent.h"
#include "HLColorAdvanceComponent.h"
#include "HLColorComponent.h"
#include "HLShaderComponent.h"
#include "HLBlendFuncComponent.h"

NS_HL_BEGIN

//const FamilyId HLSpriteComponent::familyId = 3;
INIT_FAMILYID(HLSpriteComponent)

PROPERTIES_INITIAL(HLSpriteComponent)
REGISTER_PROPERTY(HLSpriteComponent, HLTexture*, texture)
REGISTER_PROPERTY(HLSpriteComponent, bool, gray)
REGISTER_PROPERTY(HLSpriteComponent, bool, textureRotated)
REGISTER_PROPERTY(HLSpriteComponent, HLRect, textureRect)
REGISTER_PROPERTY_READONLY(HLSpriteComponent, bool, useBatch)
REGISTER_PROPERTY(HLSpriteComponent, bool, flipX)
REGISTER_PROPERTY(HLSpriteComponent, bool, flipY)
REGISTER_PROPERTY(HLSpriteComponent, HLSize, untrimmedSize);
REGISTER_PROPERTY(HLSpriteComponent, HLPoint, offsetPosFromBL)
REGISTER_PROPERTY(HLSpriteComponent, float, brightness)
REGISTER_PROPERTY(HLSpriteComponent, float, alphaTest)

HLSpriteComponent::~HLSpriteComponent()
{
    if (m_texture) 
    {
        m_texture->release();
    }
}

void HLSpriteComponent::onDeactive()
{
    HLAnimateSystem::stopAnimationsWithTarget(mEntity);
    mEntity->onDraw -= newDelegate(this, &HLSpriteComponent::drawSprite);
    
}

void HLSpriteComponent::updateCoordsAndVerts()
{
    if (m_textureRect == HLRectZero)
    {
        m_textureRect.size = m_texture->getImageSize() * m_texture->getScale();
    }
    
    static HLSize size;
    size = HLSizeZero;
    static HLSize imageSize;
    imageSize = m_texture->getImageSize() * m_texture->getScale();
    
    // 2d矩阵
    if (mTransComp)
    {
        size = mTransComp->get_size();
    }
    
    // 
    if (size == HLSizeZero)
    {
        if (m_untrimmedSize != HLSizeZero)
            size = m_untrimmedSize * HLResourceManager::getSingleton()->getResourceScale();
        else
            size = m_textureRect.size * HLResourceManager::getSingleton()->getResourceScale();
    }
    
    static HLSize untrimmedSize;
    untrimmedSize = m_untrimmedSize;
    if (untrimmedSize == HLSizeZero)
    {
        untrimmedSize = m_textureRect.size;
    }
    
    HLSize drawSize = m_textureRect.size;
    if (m_textureRotated)
    {
        float tmp = drawSize.width;
        drawSize.width = drawSize.height;
        drawSize.height = tmp;
    }
    
    float left = m_textureRect.origin.x/imageSize.width;
    float right = left + drawSize.width/imageSize.width;
    float top = m_textureRect.origin.y/imageSize.height;
    float bottom = top + drawSize.height/imageSize.height;
    if ((m_flipX && !m_textureRotated) || (m_flipY && m_textureRotated))
    {
        float tmp = left;
        left = right;
        right = tmp;
    }
    if ((m_flipY && !m_textureRotated) || (m_flipX && m_textureRotated)) {
        float tmp = top;
        top = bottom;
        bottom = tmp;
    }
    
    if (m_textureRotated)
    {
        mCoordinates[0] = left;
        mCoordinates[1] = top;
        mCoordinates[2] = left;
        mCoordinates[3] = bottom;
        mCoordinates[4] = right;
        mCoordinates[5] = top;
        mCoordinates[6] = right;
        mCoordinates[7] = bottom;
    }
    else
    {
        mCoordinates[0] = left;
        mCoordinates[1] = bottom;
        mCoordinates[2] = right;
        mCoordinates[3] = bottom;
        mCoordinates[4] = left;
        mCoordinates[5] = top;
        mCoordinates[6] = right;
        mCoordinates[7] = top;
    }
    
    drawSize = m_textureRect.size;
    HLPoint offset;
    //    if (m_untrimmedSize != HLSizeZero) {
    offset = m_offsetPosFromBL;
    if (m_flipX)
    {
        offset.x = untrimmedSize.width - offset.x - drawSize.width;
    }
    if (m_flipY)
    {
        offset.y = untrimmedSize.height - offset.y - drawSize.height;
    }
    offset.x = offset.x*(size.width/untrimmedSize.width);
    offset.y = offset.y*(size.height/untrimmedSize.height);
    drawSize.width *= (size.width/untrimmedSize.width);
    drawSize.height *= (size.height/untrimmedSize.height);
    //    }
    
    // sprite的坐标 2个三角形
    mVertices[0] = offset.x;
    mVertices[1] = offset.y;
    mVertices[2] = offset.x+drawSize.width;
    mVertices[3] = offset.y;
    mVertices[4] = offset.x;
    mVertices[5] = offset.y+drawSize.height;
    mVertices[6] = offset.x+drawSize.width;
    mVertices[7] = offset.y+drawSize.height;
}

void HLSpriteComponent::drawSprite()
{
    // 绘制
    if (!mEntity)
    {
        return;
    }
    
    if (!m_texture)
    {
        return;
    }
    
    
    if (m_texture->getName() == 0)
    {
        return;
    }
    
    if (m_useBatch)
    {
        return;
    }
    
    if (mDirty)
    {
        // 更新图片和框
        updateCoordsAndVerts();
    }
    
    // 获得形状处理器
    HLProgram *program = NULL;
    if (mShaderComp)
    {
        program = mEntity->getComponent<HLShaderComponent>()->get_shaderProgram();
    }
    
    // 设置不同的程序类型
    if (!program)
    {
        if (m_alphaTest >= 0)
        {
            program = HLProgramManager::currentManager()->getProgram(kShaderPositionTextureColorAlphaTest);// 为切边而加
        }
        else if (mColorAdvComp)
        {
            if (m_gray)
            {
                if (mBlendComp && mBlendComp->get_preMultiAlpha())
                {
                    program = HLProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorAdvanceGrayWithExtraAlphaPreMultAlpha:kShaderPositionTextureColorAdvanceGrayPreMultAlpha);
                }
                else
                {
                    program = HLProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorAdvanceGrayWithExtraAlpha:kShaderPositionTextureColorAdvanceGray);
                }
            }
            else
            {
                if (mBlendComp && mBlendComp->get_preMultiAlpha())
                {
                    program = HLProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorAdvanceWithExtraAlphaPreMultAlpha:kShaderPositionTextureColorAdvancePreMultAlpha);// 加色
                }
                else
                {
                    program = HLProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorAdvanceWithExtraAlpha:kShaderPositionTextureColorAdvance);// 加色
                }
            }
        }
        else
        {
            if (m_gray)
            {
                if (mBlendComp && mBlendComp->get_preMultiAlpha())
                {
                    program = HLProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorGrayWithExtraAlphaPreMultAlpha:kShaderPositionTextureColorGrayPreMultAlpha);// 变灰
                }
                else
                {
                    program = HLProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorGrayWithExtraAlpha:kShaderPositionTextureColorGray);// 变灰
                }
            }
            else
            {
                if (mBlendComp && mBlendComp->get_preMultiAlpha())
                {
                    program = HLProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorWithExtraAlphaPreMultAlpha:kShaderPositionTextureColorPreMultAlpha);// 正常
                }
                else
                {
                    program = HLProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorWithExtraAlpha:kShaderPositionTextureColor);// 正常
                }
            }
        }
    }
    // 启动
    program->use();

    //baidu modleView project
    
    // 视图转换
    program->setUniformForModelViewProjectionMatrix();
    
    if (mShaderComp)
    {
        mShaderComp->drawCallback(program);
    }
    else
    {
        // 需要 精讲
        std::vector<HLProgramUniform>& uniforms = program->getUniforms();
        
        for (std::vector<HLProgramUniform>::iterator it = uniforms.begin(); it != uniforms.end(); ++it)
        {
            HLProgramUniform& uniform = *it;
            if (!strcmp(uniform.name, kUniformBrightness))
            {
                uniform.setUniform1f(m_brightness);
            }
            else if (!strcmp(uniform.name, kUniformColorPercent))
            {
                color4F color = mColorAdvComp->get_colorPercent();
                uniform.setUniform4fv((GLfloat*)&color, 1);
            }
            else if (!strcmp(uniform.name, kUniformColorAmount))
            {
                color4F color = mColorAdvComp->get_colorAmount();
                uniform.setUniform4fv((GLfloat*)&color, 1);
            }
            else if (!strcmp(uniform.name, kUniformAlphaThreshold))
            {
                uniform.setUniform1f(m_alphaTest);
            }
        }
    }
    
    if (m_texture->getAlphaTexture())
    {// bug on nexus 10 and some other samsong devices
        GLint sampler1 = glGetUniformLocation(program->getProgram(), "u_texture_alpha");
        glUniform1i(sampler1, 1);
    }
    
    
    // 混色 不同算法 需要精讲
    if (mBlendComp)
    {
        hlglBlendEquation(mBlendComp->get_blendEquation());
        hlglBlendFunc(mBlendComp->get_sfactor(), mBlendComp->get_dfactor());
    }
    else
    {
        if (m_texture->hasPremultipliedAlpha())
        {
            hlglBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
        }
        else
        {
            hlglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        }
    }
    
    if (m_alphaTest>=0 && m_texture->getAlphaTexture())
    {
        hlglBindTexture(GL_TEXTURE_2D, m_texture->getAlphaTexture());
    }
    else
    {
        // 绑定纹理 后续使用的都是相关的纹理
        hlglBindTexture(GL_TEXTURE_2D, m_texture->getName());
        if (m_texture->getAlphaTexture())
        {
            hlglActiveTexture(GL_TEXTURE1);
            hlglBindTexture(GL_TEXTURE_2D, m_texture->getAlphaTexture());
            hlglActiveTexture(GL_TEXTURE0);
        }
    }
    
    // 开启顶点属性数组
    glEnableVertexAttribArray( kVertexAttrib_Position );
    glEnableVertexAttribArray( kVertexAttrib_TexCoords );
    
    // 启用数组
    if (!mColorAdvComp)
    {
        glEnableVertexAttribArray( kVertexAttrib_Color );
    }
    
    // 设置数组属性
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, mVertices);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, mCoordinates);
    
    // 
    GLubyte *colors = NULL;
    if (!mColorAdvComp || m_gray)
    {
        colors = new GLubyte[32];
        if (mEntity->hasComponent<HLColorComponent>())
        {
            color4B color = mEntity->getComponent<HLColorComponent>()->get_color();
            for (int i=0; i<8; i++)
            {
                memcpy(colors + sizeof(color4B)*i, &color, sizeof(color4B));
            }
        }
        else
        {
            memset(colors, 255, 32*sizeof(GLbyte));
        }
        // (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
        glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);
    }
    
    // 开始绘图
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    if (colors)
    {
        delete colors;
    }
    // 将绑定设置成原来的属性
    if (mBlendComp)
    {
        hlglBlendEquation(GL_FUNC_ADD);
    }
    
    CHECK_GL_ERROR();
    
#if HL_SPRITE_DEBUG_DRAW
    hlglLineWidth(1);
    program = HLProgramManager::currentManager()->getProgram(kShaderPosition_uColor);
    GLfloat colors1[4] = {1, 0, 0, 1};
    program->use();
    program->setUniformForModelViewProjectionMatrix();
    std::vector<HLProgramUniform>& uniforms = program->getUniforms();
    for (std::vector<HLProgramUniform>::iterator it = uniforms.begin(); it != uniforms.end(); ++it)
    {
        HLProgramUniform& uniform = *it;
        if (!strcmp(uniform.name, kUniformColor))
        {
            uniform.setUniform4fv(colors1, 1);
        }
        else if (!strcmp(uniform.name, kUniformPointSize))
        {
            uniform.setUniform1f(1.f);
        }
    }
    
    GLfloat vertices2[] = {0, size.height, 0, 0, size.width, 0, size.width, size.height};
    
    glEnableVertexAttribArray( kVertexAttrib_Position );
    glDisableVertexAttribArray( kVertexAttrib_Color );
    glDisableVertexAttribArray( kVertexAttrib_TexCoords );
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices2);
    glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) 4);
    CHECK_GL_ERROR();
#endif
}

HLRect HLSpriteComponent::get_textureRect()
{
    return m_textureRect;
}

void HLSpriteComponent::set_textureRect(HLRect rect)
{
    m_textureRect = rect;
    mDirty = true;
}

HLTexture* HLSpriteComponent::get_texture()
{
    return m_texture;
}

void HLSpriteComponent::set_texture(HLTexture* texture)
{
    if (texture)
    {
        texture->retain();
    }
    if (m_texture)
    {
        m_texture->release();
    }
    m_texture = texture;
    m_textureRect.origin = HLPointZero;
    if (m_texture)
    {
        m_textureRect.size = m_texture->getImageSize() * m_texture->getScale();
    }
    mDirty = true;
}

void HLSpriteComponent::set_flipX(bool flipX)
{
    m_flipX = flipX;
    mDirty = true;
}

bool HLSpriteComponent::get_flipX()
{
    return m_flipX;
}

void HLSpriteComponent::set_flipY(bool flipY)
{
    m_flipY = flipY;
    mDirty = true;
}

bool HLSpriteComponent::get_flipY()
{
    return m_flipY;
}

HLSize HLSpriteComponent::get_untrimmedSize()
{
    return m_untrimmedSize;
}

void HLSpriteComponent::set_untrimmedSize(HLSize size)
{
    m_untrimmedSize = size;
    mDirty = true;
}

HLPoint HLSpriteComponent::get_offsetPosFromBL()
{
    return m_offsetPosFromBL;
}

void HLSpriteComponent::set_offsetPosFromBL(HLPoint offset)
{
    m_offsetPosFromBL = offset;
    mDirty = true;
}

void HLSpriteComponent::onActive()
{
    mEntity->onDraw += newDelegate(this, &HLSpriteComponent::drawSprite);
    mTransComp = mEntity->getComponent<HLTransform2DComponent>();
    mColorComp = mEntity->getComponent<HLColorComponent>();
    mShaderComp = mEntity->getComponent<HLShaderComponent>();
    mColorAdvComp = mEntity->getComponent<HLColorAdvanceComponent>();
    mBlendComp = mEntity->getComponent<HLBlendFuncComponent>();
}

void HLSpriteComponent::onInternalEvent(const char* event, void* info)
{
    if (!strcmp(event, "size_changed"))
    {
        mDirty = true;
    }
    else if (!strcmp(event, "component_will_add"))
    {
        HLComponent* comp = (HLComponent*)info;
        if (comp->getFamilyId() == HLTransform2DComponent::familyId)
        {
            if (!mTransComp)
            {
                mTransComp = (HLTransform2DComponent*)comp;
            }
        }
        else if (comp->getFamilyId() == HLColorComponent::familyId)
        {
            if (!mColorComp)
            {
                mColorComp = (HLColorComponent*)comp;
            }
        }
        else if (comp->getFamilyId() == HLShaderComponent::familyId)
        {
            if (!mShaderComp)
            {
                mShaderComp = (HLShaderComponent*)comp;
            }
        }
        else if (comp->getFamilyId() == HLColorAdvanceComponent::familyId)
        {
            if (!mColorAdvComp)
            {
                mColorAdvComp = (HLColorAdvanceComponent*)comp;
            }
        }
        else if (comp->getFamilyId() == HLBlendFuncComponent::familyId)
        {
            if (!mBlendComp)
            {
                mBlendComp = (HLBlendFuncComponent*)comp;
            }
        }
    }
    else if (!strcmp(event, "component_will_remove"))
    {
        if (mTransComp == info)
        {
            mTransComp = NULL;
        }
        else if (mColorComp == info)
        {
            mColorComp = NULL;
        }
        else if (mShaderComp == info)
        {
            mShaderComp = NULL;
        }
        else if (mColorAdvComp == info)
        {
            mColorAdvComp = NULL;
        }
        else if (mBlendComp == info)
        {
            mBlendComp = NULL;
        }
    }
}

NS_HL_END
