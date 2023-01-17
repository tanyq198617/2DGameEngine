//
//  HLPrimitiveView.cpp
//  HoolaiEngine
//
//  Created by zs on 2018/9/4.
//  Copyright © 2018 ICT. All rights reserved.
//

#include "HLPrimitiveView.h"
#include "HLDrawingPrimitives.h"
#include "HLGLMatrixStack.h"
#if USE_JS_SCRIPTING
#include "JSPrimitiveView.h"
#include "JSShape.h"
#endif

namespace hoolai
{
    namespace gui
    {
        namespace Primitive
        {
            Shape::~Shape()
            {
#if USE_JS_SCRIPTING
                JSShape::removeJSObject(this);
#endif
            }
            
            void Line::draw()
            {
                hlglLineWidth(mLineWidth);
                drawColor4F(mColor);
                drawLine(mVertices[0], mVertices[1]);
            }
            
            void Rect::draw()
            {
                drawColor4F(mColor);
                if (mSolid)
                {
                    drawSolidRect(mVertices[0], mVertices[1]);
                }
                else
                {
                    hlglLineWidth(mLineWidth);
                    drawRect(mVertices[0], mVertices[1]);
                }
            }
            
            void Circle::draw()
            {
                drawColor4F(mColor);
                if (mSolid)
                {
                    drawSolidCircle(mCenter, mRadius, 0, mSegments);
                }
                else
                {
                    hlglLineWidth(mLineWidth);
                    drawCircle(mCenter, mRadius, 0, mSegments, false);
                }
            }
            
            void Polygon::draw()
            {
                drawColor4F(mColor);
                if (mSolid)
                {
                    drawSolidPoly(mVertices.data(), (int)mVertices.size());
                }
                else
                {
                    hlglLineWidth(mLineWidth);
                    drawPoly(mVertices.data(), (int)mVertices.size(), true);
                }
            }
        }
        
        HLPrimitiveView::~HLPrimitiveView()
        {
#if USE_JS_SCRIPTING
            JSPrimitiveView::removeJSObject(this);
#endif
            for (auto s : mShapes)
            {
                delete s;
            }
        }
        
        void HLPrimitiveView::onDraw()
        {
            static kmMat4 mat;
            bool initMat = false;
            if (!initMat) {
                initMat = true;
                kmMat4Scaling(&mat, 1, -1, 1);
            }
            mat.mat[13] = mFrame.size.height;
            HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
            matrixStack->pushMatrix();
            matrixStack->multMatrix(&mat);
            for (auto s : mShapes)
            {
                s->draw();
            }
            matrixStack->popMatrix();
            HLView::onDraw();
        }
        
        void HLPrimitiveView::addShape(Primitive::Shape* shape)
        {
            mShapes.push_back(shape);
        }
        
        void HLPrimitiveView::removeShape(int index)
        {
            auto itr = mShapes.begin() + index;
            if (itr != mShapes.end())
            {
                delete *itr;
                mShapes.erase(itr);
            }
        }
        
        void HLPrimitiveView::clearShapes()
        {
            for (auto s : mShapes)
            {
                delete s;
            }
            mShapes.clear();
        }
    }
}
