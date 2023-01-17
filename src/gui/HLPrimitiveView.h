//
//  HLPrimitiveView.hpp
//  HoolaiEngine
//
//  Created by zs on 2018/9/4.
//  Copyright © 2018 ICT. All rights reserved.
//

#ifndef HLPrimitiveView_h
#define HLPrimitiveView_h

#include "HLView.h"

namespace hoolai
{
    namespace gui
    {
        namespace Primitive
        {
            ///JS_BIND
            class Shape
            {
            public:
                Shape(color4F color) : mColor(color) {}
                virtual ~Shape();
                virtual void draw() = 0;
            protected:
                color4F mColor;
            };
            
            ///JS_BIND
            class Line: public Shape
            {
            public:
                ///JS_BIND
                Line(color4F color, HLPoint p1, HLPoint p2, float lineWidth = 1.f) : Shape(color), mLineWidth(lineWidth)
                {
                    mVertices[0] = p1;
                    mVertices[1] = p2;
                }
                void draw() override;
            private:
                HLPoint mVertices[2];
                float mLineWidth;
            };
            
            ///JS_BIND
            class Rect: public Shape
            {
            public:
                ///JS_BIND
                Rect(color4F color, HLPoint p1, HLPoint p2, bool solid, float lineWidth = 1.f) : Shape(color), mLineWidth(lineWidth)
                {
                    mVertices[0] = p1;
                    mVertices[1] = p2;
                    mSolid = solid;
                }
                void draw() override;
            private:
                HLPoint mVertices[2];
                bool mSolid;
                float mLineWidth;
            };
            
            ///JS_BIND
            class Circle: public Shape
            {
            public:
                ///JS_BIND
                Circle(color4F color, HLPoint center, float radius, float segments, bool solid, float lineWidth = 1.f) :
                Shape(color),
                mCenter(center),
                mRadius(radius),
                mSegments(segments),
                mSolid(solid),
                mLineWidth(lineWidth)
                {
                    
                }
                void draw() override;
            private:
                HLPoint mCenter;
                float mRadius;
                float mSegments;
                bool mSolid;
                float mLineWidth;
            };
            
            ///JS_BIND
            class Polygon: public Shape
            {
            public:
                ///JS_BIND
                Polygon(color4F color, std::vector<HLPoint>& vertices, bool solid, float lineWidth = 1.f) : Shape(color), mLineWidth(lineWidth)
                {
                    mVertices = vertices;
                    mSolid = solid;
                }
                void draw() override;
            private:
                std::vector<HLPoint> mVertices;
                bool mSolid;
                float mLineWidth;
            };
        }
        
        ///JS_BIND PrimitiveView
        class HLPrimitiveView: public HLView
        {
        public:
            ///JS_BIND
            HLPrimitiveView(HLView* parent = nullptr): HLView(parent) {}
            ///JS_BIND
            HLPrimitiveView(HLView* parent, HLRect frame): HLView(parent, frame) {}
            virtual ~HLPrimitiveView();
            
            void onDraw() override;
            
            ///JS_BIND
            void addShape(Primitive::Shape* shape);
            ///JS_BIND
            void removeShape(int index);
            ///JS_BIND
            void clearShapes();
            
        private:
            std::vector<Primitive::Shape*> mShapes;
        };
    }
}

#endif /* HLPrimitiveView_h */
