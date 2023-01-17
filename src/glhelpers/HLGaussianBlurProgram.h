//
//  HLGaussianBlurProgram.h
//  HoolaiEngine
//
//  Created by tyq on 12/6/14.
//  Copyright (c) 2014 ICT. All rights reserved.
//

#ifndef __HLGAUSSIANBLURPROGRAM__
#define __HLGAUSSIANBLURPROGRAM__

#include "HLProgram.h"

NS_HL_BEGIN

class HLGaussianBlurProgram
{
private:
    static std::map<std::string, int> programs;
    static std::string getVertexShader(int radius, float sigma);
    static std::string getFragmentShader(int radius, float sigma, bool alpha = true);
    static std::string getGlowFragmentShader(int radius, float sigma);
public:
    static HLProgram* getProgramOfRaius(float radius, bool alpha = true);
    static HLProgram* getProgramOfRaiusForGlow(float radius);
};

NS_HL_END

#endif
