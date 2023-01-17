//
//  HLGLConfiguration.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-28.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLGLConfiguration.h"
#include "GameEngineConfig.h"

NS_HL_BEGIN

HLGLConfiguration::HLGLConfiguration()
{
    _glExtensions = (const char *)glGetString(GL_EXTENSIONS);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_maxTextureUnits);
#ifdef TARGET_IOS
    glGetIntegerv(GL_MAX_SAMPLES_APPLE, &_maxSamplesAllowed); // ios
#endif
    
    _supportsPVRTC = checkForGLExtension("GL_IMG_texture_compression_pvrtc");
    _supportsETC = checkForGLExtension("GL_OES_compressed_ETC1_RGB8_texture");
    _supportsNPOT = true;
    _supportsBGRA8888 = checkForGLExtension("GL_IMG_texture_format_BGRA888");
    _supportsDiscardFramebuffer = checkForGLExtension("GL_EXT_discard_framebuffer");
    _supportsShareableVAO = checkForGLExtension("vertex_array_object");
}

bool HLGLConfiguration::checkForGLExtension(const std::string& searchName)
{
    if (_glExtensions.length() && strstr(_glExtensions.c_str(), searchName.c_str())) {
        return true;
    }
    return false;
}

NS_HL_END
