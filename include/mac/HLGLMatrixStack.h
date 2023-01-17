//
//  HLGLMatrix.h
//  HoolaiEngine
//
//  Created by liji  on 13-7-24.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLGLMATRIX_H__
#define __HLGLMATRIX_H__

#include "kazmath/GL/matrix.h"
#include "HLMacros.h"
#include "kazmath/GL/mat4stack.h"

NS_HL_BEGIN

class HLGLMatrixStack
{
public:
    static HLGLMatrixStack* matrixStack();
    static void destroyCurrentMatrix();
    
    void getMatrix(kmGLEnum mode, kmMat4* pOut);
    void pushMatrix();
    void popMatrix();
    void matrixMode(kmGLEnum mode);
    void loadIdentity();
    void loadMatrix(const kmMat4* pIn);
    void multMatrix(const kmMat4* pIn);
    void translatef(float x, float y, float z);
    void rotatef(float angle, float x, float y, float z);
    void scalef(float x, float y, float z);
    
private:
    HLGLMatrixStack();
    
    km_mat4_stack modelview_matrix_stack;
    km_mat4_stack projection_matrix_stack;
    km_mat4_stack texture_matrix_stack;
    
    km_mat4_stack* current_stack;
};

NS_HL_END

#endif
