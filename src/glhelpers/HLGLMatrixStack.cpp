//
//  HLGLMatrix.cpp
//  HoolaiEngine
//
//  Created by liji  on 13-7-24.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLGLMatrixStack.h"
#include <map>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <pthread.h>

#ifdef TARGET_WIN32
extern bool operator<(const pthread_t& first, const pthread_t& second);
#endif

NS_HL_BEGIN

std::map<pthread_t, HLGLMatrixStack* > matrix_stacks;

OpenThreads::Mutex matrix_mutex;

HLGLMatrixStack::HLGLMatrixStack() : current_stack(NULL)
{
    kmMat4 identity; //Temporary identity matrix
    
    //Initialize all 3 stacks
    //modelview_matrix_stack = (km_mat4_stack*) malloc(sizeof(km_mat4_stack));
    km_mat4_stack_initialize(&modelview_matrix_stack);
    
    //projection_matrix_stack = (km_mat4_stack*) malloc(sizeof(km_mat4_stack));
    km_mat4_stack_initialize(&projection_matrix_stack);
    
    //texture_matrix_stack = (km_mat4_stack*) malloc(sizeof(km_mat4_stack));
    km_mat4_stack_initialize(&texture_matrix_stack);
    
    current_stack = &modelview_matrix_stack;
    
    kmMat4Identity(&identity);
    
    //Make sure that each stack has the identity matrix
    km_mat4_stack_push(&modelview_matrix_stack, &identity);
    km_mat4_stack_push(&projection_matrix_stack, &identity);
    km_mat4_stack_push(&texture_matrix_stack, &identity);
}

HLGLMatrixStack* HLGLMatrixStack::matrixStack()
{
    pthread_t thread = pthread_self();
    
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(matrix_mutex);
    
    std::map<pthread_t, HLGLMatrixStack* >::iterator itr = matrix_stacks.find(thread);
    if (itr != matrix_stacks.end())
    {
        HLGLMatrixStack* res = itr->second;
        return res;
    }
    else
    {
        HLGLMatrixStack* stack = new HLGLMatrixStack();
        matrix_stacks.insert(std::make_pair(thread, stack));
        return stack;
    }
}

void HLGLMatrixStack::destroyCurrentMatrix()
{
    pthread_t thread = pthread_self();
    
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(matrix_mutex);
    std::map<pthread_t, HLGLMatrixStack* >::iterator itr = matrix_stacks.find(thread);
    if (itr != matrix_stacks.end())
    {
        matrix_stacks.erase(itr);
    }
}

void HLGLMatrixStack::getMatrix(kmGLEnum mode, kmMat4* pOut)
{
    switch(mode)
    {
        case KM_GL_MODELVIEW:
            kmMat4Assign(pOut, modelview_matrix_stack.top);
            break;
        case KM_GL_PROJECTION:
            kmMat4Assign(pOut, projection_matrix_stack.top);
            break;
        case KM_GL_TEXTURE:
            kmMat4Assign(pOut, texture_matrix_stack.top);
            break;
        default:
            assert(1 && "Invalid matrix mode specified"); //TODO: Proper error handling
            break;
    }
}

void HLGLMatrixStack::pushMatrix()
{
    kmMat4 top;
    
    //Duplicate the top of the stack (i.e the current matrix)
    kmMat4Assign(&top, current_stack->top);
    km_mat4_stack_push(current_stack, &top);
}

void HLGLMatrixStack::popMatrix()
{
    km_mat4_stack_pop(current_stack, NULL);
}

void HLGLMatrixStack::matrixMode(kmGLEnum mode)
{
    switch(mode)
    {
        case KM_GL_MODELVIEW:
            current_stack = &modelview_matrix_stack;
            break;
        case KM_GL_PROJECTION:
            current_stack = &projection_matrix_stack;
            break;
        case KM_GL_TEXTURE:
            current_stack = &texture_matrix_stack;
            break;
        default:
            assert(0 && "Invalid matrix mode specified"); //TODO: Proper error handling
            break;
    }
}

void HLGLMatrixStack::loadIdentity()
{
    kmMat4Identity(current_stack->top);
}

void HLGLMatrixStack::loadMatrix(const kmMat4* pIn)
{
    kmMat4Assign(current_stack->top, pIn);
}

void HLGLMatrixStack::multMatrix(const kmMat4* pIn)
{
    kmMat4Multiply(current_stack->top, current_stack->top, pIn);
}

void HLGLMatrixStack::translatef(float x, float y, float z)
{
    kmMat4 translation;
    
    //Create a rotation matrix using the axis and the angle
    kmMat4Translation(&translation,x,y,z);
    
    //Multiply the rotation matrix by the current matrix
    kmMat4Multiply(current_stack->top, current_stack->top, &translation);
}

void HLGLMatrixStack::rotatef(float angle, float x, float y, float z)
{
    kmVec3 axis;
    kmMat4 rotation;
    
    //Create an axis vector
    kmVec3Fill(&axis, x, y, z);
    
    //Create a rotation matrix using the axis and the angle
    kmMat4RotationAxisAngle(&rotation, &axis, kmDegreesToRadians(angle));
    
    //Multiply the rotation matrix by the current matrix
    kmMat4Multiply(current_stack->top, current_stack->top, &rotation);
}

void HLGLMatrixStack::scalef(float x, float y, float z)
{
    kmMat4 scaling;
    kmMat4Scaling(&scaling, x, y, z);
    kmMat4Multiply(current_stack->top, current_stack->top, &scaling);
}

NS_HL_END