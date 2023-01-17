//
//  HLGLConfiguration.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-28.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLGLCONFIGURATION_H__
#define __HLGLCONFIGURATION_H__

#include "HLMacros.h"
#include "HLGL.h"
#include <string>
#include "HLSingleton.h"

NS_HL_BEGIN

class HLGLConfiguration: public HLSingleton<HLGLConfiguration>
{
    friend class HLSingleton<HLGLConfiguration>;
private:
    GLint _maxTextureSize;
    GLint _maxModelviewStackDepth;
    bool _supportsPVRTC;
    bool _supportsETC;
    bool _supportsNPOT;
    bool _supportsBGRA8888;
    bool _supportsDiscardFramebuffer;
    bool _supportsShareableVAO;
    GLint _maxSamplesAllowed;
    GLint _maxTextureUnits;
    std::string _glExtensions;
    HLGLConfiguration();
public:
    int getMaxTextureSize()
    {
        return _maxTextureSize;
    }
    
    int getMaxModelviewStackDepth()
    {
        return _maxModelviewStackDepth;
    }
    
    int getTaxTextureUnits()
    {
        return _maxTextureUnits;
    }
    
    bool isSupportsNPOT()
    {
        return _supportsNPOT;
    }
    
    bool isSupportsPVRTC()
    {
        return _supportsPVRTC;
    }
    
    bool isSupportsETC()
    {
        return _supportsETC;
    }
    
    bool isSupportsBGRA8888()
    {
        return _supportsBGRA8888;
    }
    
    bool isSupportsDiscardFramebuffer()
    {
        return _supportsDiscardFramebuffer;
    }
    
    
    bool isSupportsShareableVAO()
    {
        return _supportsShareableVAO;
    }
    
    bool checkForGLExtension(const std::string& searchName);
};

NS_HL_END

#endif
