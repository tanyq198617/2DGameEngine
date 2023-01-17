//
//  HLBatchComponent.h
//  HoolaiEngine
//
//  Created by tyq on 13-3-19.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLBATCHCOMPONENT_H__
#define __HLBATCHCOMPONENT_H__

#include "HLEntitySystem.h"
#include "HLGL.h"

NS_HL_BEGIN

class HLTexture;

class HLBatchComponent: public HLComponent
{
    friend class HLTransform2DComponent;
    friend class HLSpriteComponent;
    friend class HLColorComponent;
    PROPERTIES_DECLARE
    PROPERTY_DECLARE_IMPLEMENT(HLBatchComponent, HLTexture*, texture)
    
public:
    HLBatchComponent():mDirty(true), m_texture(NULL)
#if HL_USE_VAO
    ,mVAO(-1)
#endif
    { mVBO[0] = -1;}
    
    ~HLBatchComponent()
    {
#if HL_USE_VAO
        if (mVAO != -1)
        {
            glDeleteVertexArrays(1, &mVAO);
        }
#endif
        if (mVBO[0] != -1)
        {
            glDeleteBuffers(2, &mVBO[0]);
        }
    }
    
    virtual void onActive();
    virtual void onDeactive();
    
    void drawBatch();
    
private:
    void updateQuad(HLEntity* entity);
    V3F_C4B_T2F_Quad getQuad(HLEntity* entity);
    
    HLPoint convertToBatchSpace(HLEntity* entity, const HLPoint& point);
    
    void addChild(HLEntity* child);
    void removeChild(HLEntity* child, bool cleanup);
    void removeAllChildren(bool cleanup);
#if HL_USE_VAO
    void setupVBOandVAO();
#else
    void setupVBO();
#endif
    
    virtual void onInternalEvent(const char* event, void* info);
    
    bool mDirty;
#if HL_USE_VAO
    GLuint mVAO;
#endif
    GLuint mVBO[2];
    std::vector<GLushort> mIndices;
    std::vector<V3F_C4B_T2F_Quad> mQuads;
    
    FAMILYID
};

NS_HL_END

#endif
