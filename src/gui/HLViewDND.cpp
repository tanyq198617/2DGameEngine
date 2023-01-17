//
//  HLViewDragSource.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-3-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLViewDND.h"
#include "HLImageView.h"
#include "HLRenderTexture.h"
#include "HLGUIManager.h"
#include "HLImageView.h"
#include "HLWidget.h"
#include "HLDirector2D.h"
#include "JSViewDND.h"

NS_HL_BEGIN

namespace gui
{
    HLViewDragSource* dragsourceinstance = NULL;
    
    HLViewDragSource::HLViewDragSource():mImageView(NULL),mWidget(NULL),mCurrDropTarget(NULL),mSourceView(NULL)
    {
        HLASSERT(dragsourceinstance==NULL, "error: dragsource already exists");
    }
    
    HLViewDragSource::~HLViewDragSource()
    {
        delete mWidget;
        dragsourceinstance = NULL;
#if USE_JS_SCRIPTING
        JSViewDND::removeJSObject(this);
#endif
    }
    
    void HLViewDragSource::doDragDrop(HLPoint p)
    {
        dragsourceinstance = this;
        if (mImageView)
        {
//            HLSize size = mImageView->getSize();
//            mImageView->setPosition(HLPoint(p.x - size.width/2, mWidget->getSize().height - p.y - size.height/2));
            onDraggingMove(p);
        }
    }
    
    void HLViewDragSource::onDraggingMove(HLPoint p)
    {
        if (mImageView)
        {
            HLSize size = mImageView->getSize();
            mImageView->setPosition(HLPoint(p.x - size.width/2, mWidget->getSize().height - p.y - size.height/2));
        }
        HLView* v = HLGUIManager::getSingleton()->getRootWidget()->hitTest(p);
        HLViewDraggingEvent event;
        event.location = p;
        if (mCurrDropTarget && mCurrDropTarget != v)
        {
            event.type = kHLViewDraggingExited;
            mCurrDropTarget->handleDraggingEvent(event, this);
        }
        if (mCurrDropTarget == v)
        {
            return;
        }
        mCurrDropTarget = v;
        if (v)
        {
            event.type = kHLViewDraggingEntered;
            v->handleDraggingEvent(event, this);
        }
    }
    
    void HLViewDragSource::onDraggingEnd(HLPoint p)
    {
        bool res = false;
        HLViewDraggingEvent event;
        event.location = p;
        if (mCurrDropTarget)
        {
            event.type = kHLViewDraggingDropped;
            res = mCurrDropTarget->handleDraggingEvent(event, this);
        }
        if (res)
        {
            event.type = kHLViewDraggingAccepted;
            mSourceView->handleDraggingEvent(event, this);
        }
        else
        {
            event.type = kHLViewDraggingCanceled;
            mSourceView->handleDraggingEvent(event, this);
        }
        delete this;
    }
    
    void HLViewDragSource::onDraggingCanceled()
    {
        HLViewDraggingEvent event;
        event.type = kHLViewDraggingCanceled;
        mSourceView->handleDraggingEvent(event, this);
        delete this;
    }
    
    void HLViewDragSource::setDraggingView(HLView* view, float scale, float opacity)
    {
        HLASSERT(mImageView==NULL, "dragging view already set");
        mSourceView = view;
        HLSize size = view->getSize();
        float winscale = HLDirector2D::getSingleton()->getWinSizeScale();
        HLRenderTexture rt(size.width/winscale, size.height/winscale);
        rt.beginWith2DProjection();
        rt.renderView(view);
        rt.end();
        mWidget = new HLWidget();
        mImageView = new HLImageView(mWidget);
        mImageView->setImage(rt.getSpriteFrame());
        size *= scale;
        color4B color;
        color.r = color.g = color.b = 255;
        color.a = 255 * opacity;
        mImageView->setTintColor(color);
        mImageView->setSize(size);
        mImageView->setFlipY(true);
    }
    
    void HLViewDragSource::setDraggingImage(HLTexture* tex, float scale, float opacity)
    {
        HLSize size = tex->getImageSize();
        if (!mImageView)
        {
            mWidget = new HLWidget();
            mImageView = new HLImageView(mWidget);
        }
        mImageView->setImage(tex);
        mImageView->setFlipY(false);
        size *= scale;
        color4B color;
        color.r = color.g = color.b = 255;
        color.a = 255 * opacity;
        mImageView->setTintColor(color);
        mImageView->setSize(size);
    }
    
    void HLViewDragSource::setDraggingImage(HLSpriteFrame* tex, float scale, float opacity)
    {
        HLSize size = tex->_sourceSize/tex->_texture->getScale();
        if (!mImageView)
        {
            mWidget = new HLWidget();
            mImageView = new HLImageView(mWidget);
        }
        mImageView->setImage(tex);
        size *= scale;
        color4B color;
        color.r = color.g = color.b = 255;
        color.a = 255 * opacity;
        mImageView->setTintColor(color);
        mImageView->setSize(size);
    }
    
    void HLViewDragSource::draw()
    {
        mWidget->visit();
    }
}

NS_HL_END
