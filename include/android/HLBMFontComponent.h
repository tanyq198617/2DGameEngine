//
//  HLBMFontComponent.h
//  HoolaiEngine
//
//  Created by zhao shuan on 12-8-28.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLBMFONTCOMPONENT_H__
#define __HLBMFONTCOMPONENT_H__

#include "HLEntitySystem.h"
#include "HLGeometry.h"
#include "HLBMFont.h"

NS_HL_BEGIN

// Deprecated, use GUI instead
class HLBMFontComponent: public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLBMFontComponent, std::string, fontfile)
    PROPERTY_DECLARE_IMPLEMENT(HLBMFontComponent, std::string, text)
    
public:
    HLBMFontComponent():m_fontfile(""), m_text(""), bmfont(NULL), loaded(false) {}
    
    ~HLBMFontComponent() {bmfont->release();}
    
    virtual void onActive();
    virtual void onDeactive();
    
    void draw();
    
    FAMILYID
    
private:
    HLBMFont *bmfont;
    bool loaded;
};

NS_HL_END

#endif /* defined(__HLBMFONTCOMPONENT_H__) */
