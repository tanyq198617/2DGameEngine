//
//  HLProgram.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-5-26.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLPROGRAM_H__
#define __HLPROGRAM_H__

#include "HLObject.h"
#include "HLGL.h"
#include "kazmath/GL/matrix.h"
#include "HLSingleton.h"
#include <stdint.h>

NS_HL_BEGIN

enum {
    kVertexAttrib_Position,
    kVertexAttrib_Color,
    kVertexAttrib_TexCoords,
    
    kVertexAttrib_MAX,
};


enum {
    kShaderPositionTextureColor = 0,
    kShaderPositionTexture,
    kShaderPositionTextureColorAdvance,
    kShaderPositionTextureColorGray,
    kShaderPositionTextureColorAdvanceGray,
    kShaderPositionColor,
    kShaderPosition_uColor,
    kShaderPositionTextureA8Color,
    kShaderPositionTextureA8Dist,
    kShaderPositionTextureA8DistOutline,
    kShaderPositionTextureColorAlphaTest,
    kShaderPositionTextureColorWithExtraAlpha,
    kShaderPositionTextureWithExtraAlpha,
    kShaderPositionTextureColorAdvanceWithExtraAlpha,
    kShaderPositionTextureColorGrayWithExtraAlpha,
    kShaderPositionTextureColorAdvanceGrayWithExtraAlpha,
    kShaderPositionTextureColorPreMultAlpha,
    kShaderPositionTexturePreMultAlpha,
    kShaderPositionTextureColorAdvancePreMultAlpha,
    kShaderPositionTextureColorGrayPreMultAlpha,
    kShaderPositionTextureColorAdvanceGrayPreMultAlpha,
    kShaderPositionTextureColorWithExtraAlphaPreMultAlpha,
    kShaderPositionTextureWithExtraAlphaPreMultAlpha,
    kShaderPositionTextureColorAdvanceWithExtraAlphaPreMultAlpha,
    kShaderPositionTextureColorGrayWithExtraAlphaPreMultAlpha,
    kShaderPositionTextureColorAdvanceGrayWithExtraAlphaPreMultAlpha
};

// uniform names
const char* const kUniformMVPMatrix_s = "u_MVPMatrix";
const char* const kUniformSampler_s = "u_texture";
const char* const kUniformSamplerAlpha_s = "u_texture_alpha";
const char* const kUniformBrightness = "u_brightness";
const char* const kUniformColor = "u_color";
const char* const kUniformColorPercent = "u_colorPercent";
const char* const kUniformColorAmount = "u_colorAmount";
const char* const kUniformPointSize = "u_pointSize";
const char* const kUniformBase = "u_base";
const char* const kUniformScale = "u_scale";
const char* const kUniformOutlineColor = "u_outlinecolor";
const char* const kUniformBorderOffset = "u_borderoffset";
const char* const kUniformAlphaThreshold = "u_alpha_theshold";
const char* const kUniformThreshold = "u_threshold";

// Attribute names
#define    kAttributeNameColor            "a_color"
#define    kAttributeNamePosition        "a_position"
#define    kAttributeNameTexCoord        "a_texCoord"

class HLProgramUniform
{
public:
    HLProgramUniform(){} //:data(NULL) {}
    
    char name[64];
    GLint location;
    
    void setUniform1i(GLint i1);
    void setUniform1f(GLfloat f1);
    void setUniform2i(GLint i1, GLint i2);
    void setUniform2f(GLfloat f1, GLfloat f2);
    void setUniform3i(GLint i1, GLint i2, GLint i3);
    void setUniform3f(GLfloat f1, GLfloat f2, GLfloat f3);
    void setUniform4i(GLint i1, GLint i2, GLint i3, GLint i4);
    void setUniform4f(GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4);
    void setUniform2iv(GLint* ints, unsigned int num);
    void setUniform2fv(GLfloat* floats, unsigned int num);
    void setUniform3iv(GLint* ints, unsigned int num);
    void setUniform3fv(GLfloat* floats, unsigned int num);
    void setUniform4iv(GLint* ints, unsigned int num);
    void setUniform4fv(GLfloat* floats, unsigned int num);
    void setUniformMatrix4fv(GLfloat* matrix, unsigned int num);
private:
    std::vector<uint8_t> data;
    bool updateUniform(GLvoid* data, unsigned int bytes);
};

class HLProgram:public HLObject
{
    friend class HLProgramManager;
private:
    GLuint            _program;
    GLuint            _vertShader;
    GLuint            _fragShader;
    std::vector<HLProgramUniform> mUniforms;
    GLint             _mvpUniformLocation;
    
public:
//    HLProgram(const std::string vShaderFilename, const std::string fShaderFilename)
//    {
//        const GLchar* vertexSource = HLString::stringWithContentsOfFile(vShaderFilename.c_str())->_value.c_str();
//        const GLchar* fragmentSource = HLString::stringWithContentsOfFile(fShaderFilename.c_str())->_value.c_str();
//        HLProgram(vertexSource, fragmentSource);
//    }
    static HLProgram* programFromFiles(const char* vShaderFilename, const char* fShaderFilename);
    
    HLProgram(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray);
    
    bool compileShader(GLuint * shader, GLenum type, const GLchar* source);
    
public:
    virtual ~HLProgram();
    
    void addAttribute(const char* attributeName, GLuint index);
    
    bool link();
    
    void use()
    {
        hlglUseProgram(_program);
    }
    
    void updateUniforms();
    
    void setUniformForModelViewProjectionMatrix();
    
    void reset()
    {
        _vertShader = _fragShader = 0;
        mUniforms.clear();
        _program = 0;
    }
    
    std::vector<HLProgramUniform>& getUniforms() {return mUniforms;}
    
    HLProgramUniform& getUniform(const char* name);
    
    inline const GLuint getProgram() const { return _program; }
};

class HLProgramManager: public HLObject
{
private:
//    std::map<std::string, HLProgram*> mProgramsMap;
    std::vector<HLProgram*> mPrograms;
    unsigned int mProgramCount;
    
    HLProgramManager() {loadDefaultShaders();}
    
public:
    
    virtual ~HLProgramManager();
    
    void loadDefaultShaders();
    
    static HLProgramManager* currentManager();
    
    unsigned int addProgram(const char* vShader, const char* fShader);
    
    HLProgram* getProgram(unsigned int index)
    {
        return mPrograms[index];
    }
    
//    void purgePrograms()
//    {
//        std::map<std::string, HLProgram*>::iterator itr;
//        for (itr = programs.begin(); itr != programs.end(); itr++) 
//        {
//            if (itr->second->retainCount()==1)
//                itr->second->release();
//        }
//        programs.clear();
//    }
};

NS_HL_END

#endif
