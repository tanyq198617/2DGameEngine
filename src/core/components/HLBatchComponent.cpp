//
//  HLBatchComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-3-19.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLBatchComponent.h"
#include "HLTexture.h"
#include "HLSpriteComponent.h"
#include "HLTransform2DComponent.h"
#include "HLProgram.h"
#include "HLColorComponent.h"
#include "HLShaderComponent.h"
#include <algorithm>

NS_HL_BEGIN

INIT_FAMILYID(HLBatchComponent)

PROPERTIES_INITIAL(HLBatchComponent)
REGISTER_PROPERTY(HLBatchComponent, HLTexture*, texture)

void HLBatchComponent::onActive()
{
    mEntity->onDraw += newDelegate(this, &HLBatchComponent::drawBatch);
}

void HLBatchComponent::onDeactive()
{
    mEntity->onDraw -= newDelegate(this, &HLBatchComponent::drawBatch);
    
}

HLPoint HLBatchComponent::convertToBatchSpace(HLEntity* entity, const HLPoint& point)
{
    kmMat4* transform = entity->getComponent<HLTransform2DComponent>()->get_transform();
    return HLPoint((float)((double)transform->mat[0]*point.x+(double)transform->mat[4]*point.y+transform->mat[12]),
                   (float)((double)transform->mat[1]*point.x+(double)transform->mat[5]*point.y+transform->mat[13]));
}

void HLBatchComponent::onInternalEvent(const char* event, void* info)
{
    if (!strncmp(event, "child_", 6))
    {
        updateQuad((HLEntity*)info);
    }
}

void HLBatchComponent::updateQuad(HLEntity* entity)
{
    V3F_C4B_T2F_Quad quad = getQuad(entity);
    std::list<HLEntity*>& children = mEntity->getComponent<HLTransform2DComponent>()->getChildrenRef();
    std::list<HLEntity*>::iterator iter = std::find(children.begin(), children.end(), entity);
    HLASSERT(iter != children.end(), "Something is wrong");
    
    unsigned int index = (unsigned int)std::distance(children.begin(), iter);
    mQuads[index] = quad;
    unsigned int i6 = index*6;
    unsigned int i4 = index*4;
    
    mIndices[i6+0] = (GLushort) i4+0;
    mIndices[i6+1] = (GLushort) i4+1;
    mIndices[i6+2] = (GLushort) i4+2;
    
    mIndices[i6+5] = (GLushort) i4+1;
    mIndices[i6+4] = (GLushort) i4+2;
    mIndices[i6+3] = (GLushort) i4+3;
    
    mDirty = true;
}

V3F_C4B_T2F_Quad HLBatchComponent::getQuad(HLEntity* entity)
{
    V3F_C4B_T2F_Quad quad;
    
    HLSize entitySizePix = HLSizeZero;
    HLSize untrimmedSize = entity->getProperty<HLSize>("untrimmedSize");
    HLRect textureRect = entity->getProperty<HLRect>("textureRect");
    HLSize imageSize = m_texture->getImageSize() * m_texture->getScale();
    if (entity->hasComponent<HLTransform2DComponent>())
    {
        entitySizePix = entity->getProperty<HLSize>("size");
    }
    if (entitySizePix == HLSizeZero)
    {
        if (untrimmedSize != HLSizeZero)
            entitySizePix = untrimmedSize * HLResourceManager::getSingleton()->getResourceScale();
        else
            entitySizePix = textureRect.size * HLResourceManager::getSingleton()->getResourceScale();
    }
    if (untrimmedSize == HLSizeZero)
    {
        untrimmedSize = textureRect.size;
    }
    bool textureRotated = entity->getProperty<bool>("textureRotated");
    bool flipX = entity->getProperty<bool>("flipX");
    bool flipY = entity->getProperty<bool>("flipY");
    HLSize drawSize = textureRect.size;
    if (textureRotated)
    {
        float tmp = drawSize.width;
        drawSize.width = drawSize.height;
        drawSize.height = tmp;
    }
    
    float left = textureRect.origin.x/imageSize.width;
    float right = left + drawSize.width/imageSize.width;
    float top = textureRect.origin.y/imageSize.height;
    float bottom = top + drawSize.height/imageSize.height;
    if ((flipX && !textureRotated) || (flipY && textureRotated))
    {
        float tmp = left;
        left = right;
        right = tmp;
    }
    if ((flipY && !textureRotated) || (flipX && textureRotated))
    {
        float tmp = top;
        top = bottom;
        bottom = tmp;
    }
    
    if (textureRotated)
    {
        quad.bl.texCoords.u = left;
        quad.bl.texCoords.v = top;
        quad.br.texCoords.u = left;
        quad.br.texCoords.v = bottom;
        quad.tl.texCoords.u = right;
        quad.tl.texCoords.v = top;
        quad.tr.texCoords.u = right;
        quad.tr.texCoords.v = bottom;
    }
    else
    {
        quad.bl.texCoords.u = left;
        quad.bl.texCoords.v = bottom;
        quad.br.texCoords.u = right;
        quad.br.texCoords.v = bottom;
        quad.tl.texCoords.u = left;
        quad.tl.texCoords.v = top;
        quad.tr.texCoords.u = right;
        quad.tr.texCoords.v = top;
    }
    
    drawSize = textureRect.size;
    //    if (m_untrimmedSize != HLSizeZero) {
    HLPoint offset = entity->getProperty<HLPoint>("offsetPosFromBL");
    if (flipX)
    {
        offset.x = untrimmedSize.width - offset.x - drawSize.width;
    }
    if (flipY)
    {
        offset.y = untrimmedSize.height - offset.y - drawSize.height;
    }
    offset.x = offset.x*(entitySizePix.width/untrimmedSize.width);
    offset.y = offset.y*(entitySizePix.height/untrimmedSize.height);
    drawSize.width *= (entitySizePix.width/untrimmedSize.width);
    drawSize.height *= (entitySizePix.height/untrimmedSize.height);
    //    }
    
    float vertZ = 0;
    if (entity->hasProperty("vertexZ"))
    {
        vertZ = entity->getProperty<float>("vertexZ");
    }
    HLPoint point = convertToBatchSpace(entity, offset);
    quad.bl.vertices.x = point.x;
    quad.bl.vertices.y = point.y;
    quad.bl.vertices.z = vertZ;
    point = convertToBatchSpace(entity, HLPoint(offset.x+drawSize.width, offset.y));
    quad.br.vertices.x = point.x;
    quad.br.vertices.y = point.y;
    quad.br.vertices.z = vertZ;
    point = convertToBatchSpace(entity, HLPoint(offset.x, offset.y+drawSize.height));
    quad.tl.vertices.x = point.x;
    quad.tl.vertices.y = point.y;
    quad.tl.vertices.z = vertZ;
    point = convertToBatchSpace(entity, HLPoint(offset.x+drawSize.width, offset.y+drawSize.height));
    quad.tr.vertices.x = point.x;
    quad.tr.vertices.y = point.y;
    quad.tr.vertices.z = vertZ;
    
    if (entity->hasComponent<HLColorComponent>())
    {
        color4B color = entity->getProperty<color4B>("color");
        quad.bl.colors = color;
        quad.br.colors = color;
        quad.tl.colors = color;
        quad.tr.colors = color;
    }
    else
    {
        color4B color;
        color.r = color.g = color.b = color.a = 255;
        quad.bl.colors = color;
        quad.br.colors = color;
        quad.tl.colors = color;
        quad.tr.colors = color;
    }
    
    return quad;
}

#if HL_USE_VAO
void HLBatchComponent::setupVBOandVAO()
{
    if (mVAO == -1)
    {
        glGenVertexArrays(1, &mVAO);
    }
    glBindVertexArray(mVAO);
    
    const int size = (int)mQuads.size();
    
#define kQuadSize sizeof(mQuads[0].bl)
    
    if (mVBO[0] == -1)
    {
        glGenBuffers(2, &mVBO[0]);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0]) * size, &mQuads[0], GL_DYNAMIC_DRAW);
    
    // vertices
    glEnableVertexAttribArray(kVertexAttrib_Position);
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, vertices));
    
    // colors
    glEnableVertexAttribArray(kVertexAttrib_Color);
    glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, colors));
    
    // tex coords
    glEnableVertexAttribArray(kVertexAttrib_TexCoords);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices[0]) * size * 6, &mIndices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR();
}
#else
void HLBatchComponent::setupVBO()
{
    if (mVBO[0] == -1)
    {
        glGenBuffers(2, &mVBO[0]);
    }
    
    const int size = (int)mQuads.size();
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0]) * size, &mQuads[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices[0]) * size * 6, &mIndices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR();
}
#endif

void HLBatchComponent::addChild(HLEntity* child)
{
    HLASSERT(m_texture, "texture not assigned.");
    HLASSERT(child->hasComponent<HLSpriteComponent>(), "child should have HLSpriteComponent.");
    HLASSERT(child->getProperty<HLTexture*>("texture")->getName()==m_texture->getName(), "child should use the same texture.");
    
    child->getComponent<HLSpriteComponent>()->m_useBatch = true;
    
    std::list<HLEntity*>& children = mEntity->getComponent<HLTransform2DComponent>()->getChildrenRef();
    unsigned int index;
    if (child == children.back())
    {
        index = children.size() -1;
    }
    else
    {
        std::list<HLEntity*>::iterator iter = std::find(children.begin(), children.end(), child);
        HLASSERT(iter != children.end(), "Something is wrong");
        index = (unsigned int)std::distance(children.begin(), iter);
    }
    
    V3F_C4B_T2F_Quad quad = getQuad(child);
    mQuads.insert(mQuads.begin()+index, quad);
    unsigned int size = (unsigned int)mQuads.size();
    mIndices.resize(size*6);
    for (int i = index; i < size; ++i)
    {
        unsigned int i6 = i*6;
        unsigned int i4 = i*4;
        
        mIndices[i6+0] = (GLushort) i4+0;
        mIndices[i6+1] = (GLushort) i4+1;
        mIndices[i6+2] = (GLushort) i4+2;
        
        mIndices[i6+5] = (GLushort) i4+1;
        mIndices[i6+4] = (GLushort) i4+2;
        mIndices[i6+3] = (GLushort) i4+3;
    }
    
    mDirty = true;
}

void HLBatchComponent::removeChild(HLEntity* child, bool cleanup)
{
    std::list<HLEntity*>& children = mEntity->getComponent<HLTransform2DComponent>()->getChildrenRef();
    int index = -1;
    std::list<HLEntity*>::iterator iter = children.begin();
    int counter = 0;
    while (iter != children.end())
    {
        if ((*iter) == child)
        {
            index = counter;
            break;
        }
        if (*iter)
        {
            ++counter;
        }
        ++iter;
    }
//    std::list<HLEntity*>::iterator iter = std::find(children.begin(), children.end(), child);
    HLASSERT(index >= 0, "Something is wrong");
    
    mQuads.erase(mQuads.begin()+index);
    mIndices.erase(mIndices.begin()+index*6, mIndices.begin()+index*6+6);
    for (std::vector<GLushort>::iterator itr = mIndices.begin()+index*6; itr != mIndices.end(); ++itr)
    {
        (*itr) -= 4;
    }
    if (!cleanup)
    {
        child->getComponent<HLSpriteComponent>()->m_useBatch = false;
    }
    
    mDirty = true;
}

void HLBatchComponent::removeAllChildren(bool cleanup)
{
    mQuads.clear();
    mIndices.clear();
    if (!cleanup)
    {
        std::list<HLEntity*>& children = mEntity->getComponent<HLTransform2DComponent>()->getChildrenRef();
        std::list<HLEntity*>::iterator iter = children.begin();
        for (; iter != children.end(); ++iter)
        {
            (*iter)->getComponent<HLSpriteComponent>()->m_useBatch = false;
        }
    }
    
    mDirty = true;
}

void HLBatchComponent::drawBatch()
{
    if (mQuads.size() == 0)
    {
        return;
    }
    
    if (mDirty)
    {
        mDirty = false;
#if HL_USE_VAO
        setupVBOandVAO();
#else
        setupVBO();
#endif
    }
    
    hlglBindTexture(GL_TEXTURE_2D, m_texture->getName());
    
    HLProgram *program = NULL;
    HLShaderComponent* shaderComp = mEntity->getComponent<HLShaderComponent>();
    if (shaderComp)
    {
        program = shaderComp->get_shaderProgram();
    }
    else
    {
        program = HLProgramManager::currentManager()->getProgram(kShaderPositionTextureColor);
    }
    program->use();
    if (shaderComp)
    {
        shaderComp->drawCallback(program);
    }
    else
    {
        program->getUniforms()[0].setUniform1f(1.f); // brightness
    }
    program->setUniformForModelViewProjectionMatrix();
    
    if (mEntity->hasProperty("blendEquation"))
    {
        hlglBlendEquation(mEntity->getProperty<GLenum>("blendEquation"));
        hlglBlendFunc(mEntity->getProperty<GLenum>("sfactor"), mEntity->getProperty<GLenum>("dfactor"));
    }
    else
    {
        hlglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    }
    
#if HL_USE_VAO
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)mQuads.size()*6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray( 0 );
#else
#define kQuadSize sizeof(mQuads[0].bl)
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
    // vertices
    glEnableVertexAttribArray(kVertexAttrib_Position);
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, vertices));
    
    // colors
    glEnableVertexAttribArray(kVertexAttrib_Color);
    glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, colors));
    
    // tex coords
    glEnableVertexAttribArray(kVertexAttrib_TexCoords);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[1]);
    
    glDrawElements(GL_TRIANGLES, (GLsizei)mQuads.size()*6, GL_UNSIGNED_SHORT, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
    
    CHECK_GL_ERROR();
}

NS_HL_END
