//
//  HLGaussianBlurProgram.cpp
//  HoolaiEngine
//
//  Created by tyq on 12/6/14.
//  Copyright (c) 2014 ICT. All rights reserved.
//

#include "HLGaussianBlurProgram.h"
#include "HLStringUtil.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define STRINGIZE(x) #x
#define STRINGIZE2(x) STRINGIZE(x)
#define SHADER_STRING(text) STRINGIZE2(text)

#if defined(TARGET_ANDROID)
#define MAXOFFSET 4
#else
#define MAXOFFSET 7
#endif

NS_HL_BEGIN

std::map<std::string, int> HLGaussianBlurProgram::programs;

std::string HLGaussianBlurProgram::getVertexShader(int blurRadius, float sigma)
{
    if (blurRadius < 1) {
        return SHADER_STRING
        (
         attribute vec4 position;
         attribute vec4 a_texCoord;
         
         varying vec2 textureCoordinate;
         
         void main()
         {
             gl_Position = position;
             textureCoordinate = a_texCoord.xy;
         }
         );
    }
    
    // First, generate the normal Gaussian weights for a given sigma
    GLfloat *standardGaussianWeights = (GLfloat *)calloc(blurRadius + 1, sizeof(GLfloat));
    GLfloat sumOfWeights = 0.0;
    for (int currentGaussianWeightIndex = 0; currentGaussianWeightIndex < blurRadius + 1; currentGaussianWeightIndex++)
    {
        standardGaussianWeights[currentGaussianWeightIndex] = (1.0 / sqrt(2.0 * M_PI * pow(sigma, 2.0))) * exp(-pow(currentGaussianWeightIndex, 2.0) / (2.0 * pow(sigma, 2.0)));
        
        if (currentGaussianWeightIndex == 0)
        {
            sumOfWeights += standardGaussianWeights[currentGaussianWeightIndex];
        }
        else
        {
            sumOfWeights += 2.0 * standardGaussianWeights[currentGaussianWeightIndex];
        }
    }
    
    // Next, normalize these weights to prevent the clipping of the Gaussian curve at the end of the discrete samples from reducing luminance
    for (int currentGaussianWeightIndex = 0; currentGaussianWeightIndex < blurRadius + 1; currentGaussianWeightIndex++)
    {
        standardGaussianWeights[currentGaussianWeightIndex] = standardGaussianWeights[currentGaussianWeightIndex] / sumOfWeights;
    }
    
    // From these weights we calculate the offsets to read interpolated values from
    int numberOfOptimizedOffsets = MIN(blurRadius / 2 + (blurRadius % 2), MAXOFFSET);
    GLfloat *optimizedGaussianOffsets = (GLfloat *)calloc(numberOfOptimizedOffsets, sizeof(GLfloat));
    
    for (int currentOptimizedOffset = 0; currentOptimizedOffset < numberOfOptimizedOffsets; currentOptimizedOffset++)
    {
        GLfloat firstWeight = standardGaussianWeights[currentOptimizedOffset*2 + 1];
        GLfloat secondWeight = standardGaussianWeights[currentOptimizedOffset*2 + 2];
        
        GLfloat optimizedWeight = firstWeight + secondWeight;
        
        optimizedGaussianOffsets[currentOptimizedOffset] = (firstWeight * (currentOptimizedOffset*2 + 1) + secondWeight * (currentOptimizedOffset*2 + 2)) / optimizedWeight;
    }
    
    std::string shaderString = StringUtil::Format("\
attribute vec4 a_position;\n\
attribute vec4 a_texCoord;\n\
uniform    mat4 u_MVPMatrix;\n\
\n\
uniform float u_texelOffset_x;\n\
uniform float u_texelOffset_y;\n\
\n\
varying vec2 blurCoordinates[%lu];\n\
\n\
void main()\n\
{\n\
gl_Position = u_MVPMatrix * a_position;\n\
\n\
vec2 singleStepOffset = vec2(u_texelOffset_x, u_texelOffset_y);\n", (unsigned long)(1 + (numberOfOptimizedOffsets * 2)));
    shaderString += "blurCoordinates[0] = a_texCoord.xy;\n";
    for (int currentOptimizedOffset = 0; currentOptimizedOffset < numberOfOptimizedOffsets; currentOptimizedOffset++)
    {
        shaderString += StringUtil::Format("\
blurCoordinates[%lu] = a_texCoord.xy + singleStepOffset * %f;\n\
blurCoordinates[%lu] = a_texCoord.xy - singleStepOffset * %f;\n", (unsigned long)((currentOptimizedOffset * 2) + 1), optimizedGaussianOffsets[currentOptimizedOffset], (unsigned long)((currentOptimizedOffset * 2) + 2), optimizedGaussianOffsets[currentOptimizedOffset]);
    }
    
    shaderString += "}\n";
    
    free(optimizedGaussianOffsets);
    free(standardGaussianWeights);
    return shaderString;
}

std::string HLGaussianBlurProgram::getFragmentShader(int blurRadius, float sigma, bool alpha)
{
    if (blurRadius < 1) {
        return SHADER_STRING
        (
         varying highp vec2 textureCoordinate;
         
         uniform sampler2D u_texture;
         
         void main()
         {
             gl_FragColor = texture2D(u_texture, textureCoordinate);
         }
         );
    }
    
    // First, generate the normal Gaussian weights for a given sigma
    GLfloat *standardGaussianWeights = (GLfloat *)calloc(blurRadius + 1, sizeof(GLfloat));
    GLfloat sumOfWeights = 0.0;
    for (int currentGaussianWeightIndex = 0; currentGaussianWeightIndex < blurRadius + 1; currentGaussianWeightIndex++)
    {
        standardGaussianWeights[currentGaussianWeightIndex] = (1.0 / sqrt(2.0 * M_PI * pow(sigma, 2.0))) * exp(-pow(currentGaussianWeightIndex, 2.0) / (2.0 * pow(sigma, 2.0)));
        
        if (currentGaussianWeightIndex == 0)
        {
            sumOfWeights += standardGaussianWeights[currentGaussianWeightIndex];
        }
        else
        {
            sumOfWeights += 2.0 * standardGaussianWeights[currentGaussianWeightIndex];
        }
    }
    
    // Next, normalize these weights to prevent the clipping of the Gaussian curve at the end of the discrete samples from reducing luminance
    for (int currentGaussianWeightIndex = 0; currentGaussianWeightIndex < blurRadius + 1; currentGaussianWeightIndex++)
    {
        standardGaussianWeights[currentGaussianWeightIndex] = standardGaussianWeights[currentGaussianWeightIndex] / sumOfWeights;
    }
    
    // From these weights we calculate the offsets to read interpolated values from
    int numberOfOptimizedOffsets = MIN(blurRadius / 2 + (blurRadius % 2), MAXOFFSET);
    int trueNumberOfOptimizedOffsets = blurRadius / 2 + (blurRadius % 2);
    
#if defined(TARGET_IOS) || defined(TARGET_ANDROID)
    std::string shaderString = StringUtil::Format("\
uniform sampler2D u_texture;\n\
uniform highp float u_texelOffset_x;\n\
uniform highp float u_texelOffset_y;\n\
\n\
varying highp vec2 blurCoordinates[%lu];\n\
\n\
void main()\n\
{\n\
lowp vec4 sum = vec4(0.0);\n", (unsigned long)(1 + (numberOfOptimizedOffsets * 2)));
#else
    std::string shaderString = StringUtil::Format("\
uniform sampler2D u_texture;\n\
uniform float u_texelOffset_x;\n\
uniform float u_texelOffset_y;\n\
\n\
varying vec2 blurCoordinates[%lu];\n\
\n\
void main()\n\
{\n\
vec4 sum = vec4(0.0);\n", 1 + (numberOfOptimizedOffsets * 2) );
#endif
    if (alpha) {
        shaderString += StringUtil::Format("sum += texture2D(u_texture, blurCoordinates[0]) * %f;\n", standardGaussianWeights[0]);
    } else {
        shaderString += StringUtil::Format("sum += vec4(texture2D(u_texture, blurCoordinates[0]).rgb * %f, 1.0);\n", standardGaussianWeights[0]);
    }
    
    for (int currentBlurCoordinateIndex = 0; currentBlurCoordinateIndex < numberOfOptimizedOffsets; currentBlurCoordinateIndex++)
    {
        GLfloat firstWeight = standardGaussianWeights[currentBlurCoordinateIndex * 2 + 1];
        GLfloat secondWeight = standardGaussianWeights[currentBlurCoordinateIndex * 2 + 2];
        GLfloat optimizedWeight = firstWeight + secondWeight;
        
        if (alpha) {
            shaderString += StringUtil::Format("sum += texture2D(u_texture, blurCoordinates[%lu]) * %f;\n\
sum += texture2D(u_texture, blurCoordinates[%lu]) * %f;\n", (unsigned long)((currentBlurCoordinateIndex * 2) + 1), optimizedWeight, (unsigned long)((currentBlurCoordinateIndex * 2) + 2), optimizedWeight);
        } else {
            shaderString += StringUtil::Format("sum += vec4(texture2D(u_texture, blurCoordinates[%lu]).rgb * %f, 1.0);\n\
sum += vec4(texture2D(u_texture, blurCoordinates[%lu]).rgb * %f, 1.0);\n", (unsigned long)((currentBlurCoordinateIndex * 2) + 1), optimizedWeight, (unsigned long)((currentBlurCoordinateIndex * 2) + 2), optimizedWeight);
        }
    }
    
    // If the number of required samples exceeds the amount we can pass in via varyings, we have to do dependent texture reads in the fragment shader
    if (trueNumberOfOptimizedOffsets > numberOfOptimizedOffsets)
    {
#if defined(TARGET_IOS) || defined(TARGET_ANDROID)
        shaderString += "highp vec2 singleStepOffset = vec2(u_texelOffset_x, u_texelOffset_y);\n";
#else
        shaderString += "vec2 singleStepOffset = vec2(u_texelOffset_x, u_texelOffset_y);\n";
#endif
        for (int currentOverlowTextureRead = numberOfOptimizedOffsets; currentOverlowTextureRead < trueNumberOfOptimizedOffsets; currentOverlowTextureRead++)
        {
            GLfloat firstWeight = standardGaussianWeights[currentOverlowTextureRead * 2 + 1];
            GLfloat secondWeight = standardGaussianWeights[currentOverlowTextureRead * 2 + 2];
            
            GLfloat optimizedWeight = firstWeight + secondWeight;
            GLfloat optimizedOffset = (firstWeight * (currentOverlowTextureRead * 2 + 1) + secondWeight * (currentOverlowTextureRead * 2 + 2)) / optimizedWeight;
            if (alpha) {
                shaderString += StringUtil::Format("sum += texture2D(u_texture, blurCoordinates[0] + singleStepOffset * %f) * %f;\n\
sum += texture2D(u_texture, blurCoordinates[0] - singleStepOffset * %f) * %f;\n", optimizedOffset, optimizedWeight, optimizedOffset, optimizedWeight);
            } else {
                shaderString += StringUtil::Format("sum += vec4(texture2D(u_texture, blurCoordinates[0] + singleStepOffset * %f).rgb * %f, 1.0);\n\
sum += vec4(texture2D(u_texture, blurCoordinates[0] - singleStepOffset * %f).rgb * %f, 1.0);\n", optimizedOffset, optimizedWeight, optimizedOffset, optimizedWeight);
            }
        }
    }
    
    shaderString += "\
gl_FragColor = sum;\n\
}\n";
    free(standardGaussianWeights);
    return shaderString;
}

std::string HLGaussianBlurProgram::getGlowFragmentShader(int blurRadius, float sigma)
{
    if (blurRadius < 1) {
        return SHADER_STRING
        (
         varying highp vec2 textureCoordinate;
         
         uniform sampler2D u_texture;
         
         uniform vec4 u_glowColor;
         
         void main()
         {
             gl_FragColor = texture2D(u_texture, textureCoordinate).a * u_glowColor;
         }
         );
    }
    
    // First, generate the normal Gaussian weights for a given sigma
    GLfloat *standardGaussianWeights = (GLfloat *)calloc(blurRadius + 1, sizeof(GLfloat));
    GLfloat sumOfWeights = 0.0;
    for (int currentGaussianWeightIndex = 0; currentGaussianWeightIndex < blurRadius + 1; currentGaussianWeightIndex++)
    {
        standardGaussianWeights[currentGaussianWeightIndex] = (1.0 / sqrt(2.0 * M_PI * pow(sigma, 2.0))) * exp(-pow(currentGaussianWeightIndex, 2.0) / (2.0 * pow(sigma, 2.0)));
        
        if (currentGaussianWeightIndex == 0)
        {
            sumOfWeights += standardGaussianWeights[currentGaussianWeightIndex];
        }
        else
        {
            sumOfWeights += 2.0 * standardGaussianWeights[currentGaussianWeightIndex];
        }
    }
    
    // Next, normalize these weights to prevent the clipping of the Gaussian curve at the end of the discrete samples from reducing luminance
    for (int currentGaussianWeightIndex = 0; currentGaussianWeightIndex < blurRadius + 1; currentGaussianWeightIndex++)
    {
        standardGaussianWeights[currentGaussianWeightIndex] = standardGaussianWeights[currentGaussianWeightIndex] / sumOfWeights * 1.5;
    }
    
    // From these weights we calculate the offsets to read interpolated values from
    int numberOfOptimizedOffsets = MIN(blurRadius / 2 + (blurRadius % 2), MAXOFFSET);
    int trueNumberOfOptimizedOffsets = blurRadius / 2 + (blurRadius % 2);
    
#if defined(TARGET_IOS) || defined(TARGET_ANDROID)
    std::string shaderString = StringUtil::Format("\
uniform sampler2D u_texture;\n\
uniform highp vec4 u_glowColor;\n\
uniform highp float u_texelOffset_x;\n\
uniform highp float u_texelOffset_y;\n\
\n\
varying highp vec2 blurCoordinates[%lu];\n\
\n\
void main()\n\
{\n\
lowp vec4 sum = vec4(0.0);\n", (unsigned long)(1 + (numberOfOptimizedOffsets * 2)));
#else
    std::string shaderString = StringUtil::Format("\
uniform sampler2D u_texture;\n\
uniform vec4 u_glowColor;\n\
uniform float u_texelOffset_x;\n\
uniform float u_texelOffset_y;\n\
\n\
varying vec2 blurCoordinates[%lu];\n\
\n\
void main()\n\
{\n\
float sum = 0.0;\n", 1 + (numberOfOptimizedOffsets * 2) );
#endif
    shaderString += StringUtil::Format("sum += texture2D(u_texture, blurCoordinates[0]).a * %f;\n", standardGaussianWeights[0]);
    
    for (int currentBlurCoordinateIndex = 0; currentBlurCoordinateIndex < numberOfOptimizedOffsets; currentBlurCoordinateIndex++)
    {
        GLfloat firstWeight = standardGaussianWeights[currentBlurCoordinateIndex * 2 + 1];
        GLfloat secondWeight = standardGaussianWeights[currentBlurCoordinateIndex * 2 + 2];
        GLfloat optimizedWeight = firstWeight + secondWeight;
//        optimizedWeight *= 1.5;
        
        shaderString += StringUtil::Format("sum += texture2D(u_texture, blurCoordinates[%lu]).a * %f;\n\
sum += texture2D(u_texture, blurCoordinates[%lu]).a * %f;\n", (unsigned long)((currentBlurCoordinateIndex * 2) + 1), optimizedWeight, (unsigned long)((currentBlurCoordinateIndex * 2) + 2), optimizedWeight);
    }
    
    // If the number of required samples exceeds the amount we can pass in via varyings, we have to do dependent texture reads in the fragment shader
    if (trueNumberOfOptimizedOffsets > numberOfOptimizedOffsets)
    {
#if defined(TARGET_IOS) || defined(TARGET_ANDROID)
        shaderString += "highp vec2 singleStepOffset = vec2(u_texelOffset_x, u_texelOffset_y);\n";
#else
        shaderString += "vec2 singleStepOffset = vec2(u_texelOffset_x, u_texelOffset_y);\n";
#endif
        for (int currentOverlowTextureRead = numberOfOptimizedOffsets; currentOverlowTextureRead < trueNumberOfOptimizedOffsets; currentOverlowTextureRead++)
        {
            GLfloat firstWeight = standardGaussianWeights[currentOverlowTextureRead * 2 + 1];
            GLfloat secondWeight = standardGaussianWeights[currentOverlowTextureRead * 2 + 2];
            
            GLfloat optimizedWeight = firstWeight + secondWeight;
//            optimizedWeight *= 1.5;
            GLfloat optimizedOffset = (firstWeight * (currentOverlowTextureRead * 2 + 1) + secondWeight * (currentOverlowTextureRead * 2 + 2)) / optimizedWeight;
            
            shaderString += StringUtil::Format("sum += texture2D(u_texture, blurCoordinates[0] + singleStepOffset * %f).a * %f;\n\
sum += texture2D(u_texture, blurCoordinates[0] - singleStepOffset * %f).a * %f;\n", optimizedOffset, optimizedWeight, optimizedOffset, optimizedWeight);
        }
    }
    
    shaderString += "\
gl_FragColor = vec4(u_glowColor.rgb, u_glowColor.a*sum);\n\
}\n";
    free(standardGaussianWeights);
    return shaderString;
}

HLProgram* HLGaussianBlurProgram::getProgramOfRaius(float radius, bool alpha)
{
    int iradius = roundf(radius);
    std::string key = StringUtil::Format("%d", iradius);
    auto res = programs.find(key);
    if (res != programs.end()) {
        return HLProgramManager::currentManager()->getProgram(res->second);
    }
    
    int calculatedSampleRadius = 0;
    if (iradius >= 1) {
        GLfloat minimumWeightToFindEdgeOfSamplingArea = 1.0/256.0;
        calculatedSampleRadius = floor(sqrt(-2.0 * pow(iradius, 2.0) * log(minimumWeightToFindEdgeOfSamplingArea * sqrt(2.0 * M_PI * pow(iradius, 2.0))) ));
        calculatedSampleRadius += calculatedSampleRadius % 2; // There's nothing to gain from handling odd radius sizes, due to the optimizations I use
    }
    
    int idx = HLProgramManager::currentManager()->addProgram(getVertexShader(calculatedSampleRadius, iradius).c_str(), getFragmentShader(calculatedSampleRadius, iradius, alpha).c_str());
    programs.insert(std::make_pair(key, idx));
    
    HLProgram* program = HLProgramManager::currentManager()->getProgram(idx);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    return program;
}

HLProgram* HLGaussianBlurProgram::getProgramOfRaiusForGlow(float radius)
{
    int iradius = roundf(radius);
    std::string key = StringUtil::Format("%d_glow", iradius);
    auto res = programs.find(key);
    if (res != programs.end()) {
        return HLProgramManager::currentManager()->getProgram(res->second);
    }
    
    int calculatedSampleRadius = 0;
    if (iradius >= 1) {
        GLfloat minimumWeightToFindEdgeOfSamplingArea = 1.0/256.0;
        calculatedSampleRadius = floor(sqrt(-2.0 * pow(iradius, 2.0) * log(minimumWeightToFindEdgeOfSamplingArea * sqrt(2.0 * M_PI * pow(iradius, 2.0))) ));
        calculatedSampleRadius += calculatedSampleRadius % 2; // There's nothing to gain from handling odd radius sizes, due to the optimizations I use
    }
    
    int idx = HLProgramManager::currentManager()->addProgram(getVertexShader(calculatedSampleRadius, iradius).c_str(), getGlowFragmentShader(calculatedSampleRadius, iradius).c_str());
    programs.insert(std::make_pair(key, idx));
    
    HLProgram* program = HLProgramManager::currentManager()->getProgram(idx);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    return program;
}

NS_HL_END
