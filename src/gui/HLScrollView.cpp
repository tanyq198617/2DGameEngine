//
//  HLScrollView.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLScrollView.h"
#include "kazmath/GL/matrix.h"
#include "HLDrawingPrimitives.h"
#include "HLDirector2D.h"
#include "GameEngineConfig.h"
#include "JSScrollView.h"

#ifndef TARGET_WIN32
using std::min;
#endif

NS_HL_BEGIN

#define SCROLL_DEACCEL_RATE  0.95f
#define SCROLL_DEACCEL_DIST  1.0f
#define BOUNCE_DURATION      0.15f
#define INSET_RATIO          0.5f

namespace gui
{
    class HLScrollViewContentView: public HLView
    {
    public:
        HLScrollViewContentView(HLScrollView* parent, HLRect frame):HLView(parent, frame)
        {
            mScrollView = parent;
        }
    protected:
        virtual void onDraw()
        {
            std::list<HLView*>::const_iterator itr = mSubViews.begin();
            for (; itr != mSubViews.end(); ++itr)
            {
                if ((*itr)->isVisible() && mScrollView->isViewVisible(*itr))
                {
                    (*itr)->visit();
                }
            }
        }
    private:
        HLScrollView* mScrollView;
    };
    
    HLScrollView::HLScrollView(HLView * parent):HLView(parent),mTouchEnable(true)
    {
        initMembers();
    }
    
    HLScrollView::HLScrollView(HLView * parent, HLRect frame):HLView(parent, frame),mTouchEnable(true)
    {
        initMembers();
    }
    
    HLScrollView::HLScrollView(HLView * parent, const HLScrollView& copy):HLView(parent, copy),mTouchEnable(true)
    {
        initMembers();
        mDirection = copy.mDirection;
        mBounceable = copy.mBounceable;
        setContentSize(copy.getContentSize());
    }
    
    HLScrollView::~HLScrollView()
    {
#if USE_JS_SCRIPTING
        JSScrollView::removeJSObject(this);
#endif
        if (mAnimationTimer)
        {
            mAnimationTimer->cancel();
        }
    }
    
    void HLScrollView::initMembers()
    {
        mContainer = NULL;
        mContainer = new HLScrollViewContentView(this, mFrame);
        mContainer->setPosition(0, 0);
        mBounceable = true;
        mClipToBounds = true;
        mDirection = kHLScrollViewDirectionBoth;
        mZoomScale = 1.0f;
        mDragging = false;
        mDeaccelerating = false;
        mTouchMoved = false;
        mTouchLength = 0.0f;
        mMinScale = mMaxScale = 1.0f;
        mUserInteractionEnabled = true;
        mAnimationTimer = NULL;
        mViewOnTouch = NULL;
        mPaging = false;
    }
    
    void HLScrollView::onInternalEvent(const char * event)
    {
        if (!strcmp(event, "subview_added"))
        {
            if (mContainer)
            {
                HLView *view = mSubViews.back();
                removeSubview(view, false);
                mContainer->addSubview(view);
            }
        }
        else if (!strcmp(event, "subview_modified"))
        {
            HLASSERT(0, "you cannot modify subview directly in scrollview, please use getContentView()\n");
        }
    }
    
    bool HLScrollView::isViewVisible(HLView *view)
    {
        const HLPoint offset = getContentOffset();
        const HLSize size = mBounds.size;
        const float scale = mZoomScale;
        
        HLRect viewRect = HLRect(-offset.x/scale, -offset.y/scale, size.width/scale, size.height/scale);
        return viewRect.intersectsRect(view->getFrame());
    }
    
    void HLScrollView::setContentOffset(HLPoint offset, bool animated)
    {
        if (offset == mContainer->getPosition())
        {
            return;
        }
        if (animated)
        {
            setContentOffsetInDuration(offset, BOUNCE_DURATION);
        }
        else
        {
            if (!mBounceable)
            {
                const HLPoint minOffset = minContainerOffset();
                const HLPoint maxOffset = maxContainerOffset();
                
                offset.x = MAX(minOffset.x, MIN(maxOffset.x, offset.x));
                offset.y = MAX(minOffset.y, MIN(maxOffset.y, offset.y));
            }
            
            mContainer->setPosition(offset);
            scrollViewDidScroll(this);
        }
    }
    
    void HLScrollView::setContentOffsetInDuration(HLPoint offset, float dt)
    {
        HLView::startAnimations("scrollview_setcontentoffset", dt);
        HLView::animationDidStopCallback = newDelegate(this, &HLScrollView::stoppedAnimatedScroll);
        mContainer->setPosition(offset);
        scrollViewDidScroll(this);
        
        HLView::commitAnimations();
        if (mAnimationTimer)
        {
            mAnimationTimer->cancel();
        }
        mAnimationTimer = new HLTimer(0, true);
        mAnimationTimer->delegate = newDelegate(this, &HLScrollView::performedAnimatedScroll);
    }
    
    void HLScrollView::performedAnimatedScroll(HLTimer* timer, float dt)
    {
        if (mDragging)
        {
            mAnimationTimer->cancel();
            mAnimationTimer = NULL;
            return;
        }
        scrollViewDidScroll(this);
    }
    
    void HLScrollView::stoppedAnimatedScroll(bool finished, void* context)
    {
        if (finished&&mAnimationTimer)
        {
            mAnimationTimer->cancel();
            mAnimationTimer = NULL;
        }
        scrollViewDidEndScrollingAnimation(this);
    }
    
    HLPoint HLScrollView::getContentOffset()
    {
        return mContainer->getPosition();
    }
    
    void HLScrollView::setZoomScale(float s)
    {
        if (mZoomScale != s)
        {
            HLPoint oldCenter, newCenter;
            HLPoint center;
            
            if (mTouchLength == 0.0f)
            {
                center = HLPoint(mBounds.size.width*0.5f, mBounds.size.height*0.5f);
            }
            else
            {
                center = mTouchPoint;
            }
            
            oldCenter = (center-mContainer->getPosition())/mZoomScale;
            mZoomScale = MAX(mMinScale, MIN(mMaxScale, s));
            newCenter = mContainer->getPosition()+oldCenter*mZoomScale;
            
            const HLPoint offset = oldCenter - newCenter;
            scrollViewDidZoom(this);
            if (mTouchEnable) {
                setContentOffset(mContainer->getPosition()+offset);
            }
        }
    }
    
    float HLScrollView::getZoomScale()
    {
        return mZoomScale;
    }
    
    void HLScrollView::setZoomScale(float s, bool animated)
    {
        if (animated)
        {
            setZoomScaleInDuration(s, BOUNCE_DURATION);
        }
        else
        {
            setZoomScale(s);
        }
    }
    
    void HLScrollView::setZoomScaleInDuration(float s, float dt)
    {
        // 暂不支持动画缩放
        //        if (dt > 0)
        //        {
        //
        //        }
        //        else
        //        {
        setZoomScale(s);
        //        }
    }
    
    void HLScrollView::relocateContainer(bool animated)
    {
        HLPoint oldPoint, min, max;
        float newX, newY;
        
        min = minContainerOffset();
        max = maxContainerOffset();
        
        oldPoint = mContainer->getPosition();
        
        newX = oldPoint.x;
        newY = oldPoint.y;
        
        if (mPaging)
        {
            float page_width = mBounds.size.width;
            float page_height = mBounds.size.height;
            float posX = -newX;
            float posY = -newY;

            if (mDirection&kHLScrollViewDirectionHorizontal && fabs(mScrollSpeed.x) > 5)
            {
                int pageX = posX/page_width;
                if (mScrollSpeed.x < 0)
                {
                    ++pageX;
                }
                newX = -pageX*page_width;
            }
            else
            {
                int pageX = (posX+page_width/2)/page_width;
                newX = -pageX*page_width;
            }
            if (mDirection&kHLScrollViewDirectionVertical && fabs(mScrollSpeed.y) > 5)
            {
                int pageY = posY/page_height;
                if (mScrollSpeed.y < 0)
                {
                    ++pageY;
                }
                newY = -pageY*page_height;
            }
            else
            {
                int pageY = (posY+page_height/2)/page_height;
                newY = -pageY*page_height;
            }
        }
        
        if (mDirection&kHLScrollViewDirectionHorizontal)
        {
            newX = MIN(newX, max.x);
            newX = MAX(newX, min.x);
        }
        
        if (mDirection&kHLScrollViewDirectionVertical)
        {
            newY = MIN(newY, max.y);
            newY = MAX(newY, min.y);
        }
        
        if (newY != oldPoint.y || newX != oldPoint.x)
        {
            if (mTouchEnable)
            {
                setContentOffset(HLPoint(newX, newY), animated);
            }
        }
        else if (animated)
        {
            scrollViewDidEndScrollingAnimation(this);
        }
    }
    
    HLPoint HLScrollView::maxContainerOffset()
    {
        return HLPointZero;
    }
    
    HLPoint HLScrollView::minContainerOffset()
    {
        return HLPoint(min(mBounds.size.width-mContainer->getSize().width*mZoomScale, 0.f),
                       min(mBounds.size.height-mContainer->getSize().height*mZoomScale, 0.f));
    }
    
    void HLScrollView::setContentSize(HLSize size)
    {
        mContainer->setSize(size);
        mMaxInset = maxContainerOffset();
        mMaxInset = HLPoint(mMaxInset.x + mBounds.size.width * INSET_RATIO,
                            mMaxInset.y + mBounds.size.height * INSET_RATIO);
        mMinInset = minContainerOffset();
        mMinInset = HLPoint(mMinInset.x - mBounds.size.width * INSET_RATIO,
                            mMinInset.y - mBounds.size.height * INSET_RATIO);
    }
    
    HLSize HLScrollView::getContentSize() const
    {
        return mContainer->getSize();
    }
    
    void HLScrollView::onTouches(HLTouchEvent* touch)
    {
        switch (touch->type)
        {
            case kHLTouchesBegin:
            {
                scrollViewDidDrag(this);
                mViewOnTouch = mContainer->hitTest(touch->touches[0].location);
                if (touch->touches.size() == 1)
                {
                    mTouchPoint = touchLocationInView(&(touch->touches[0]));
                    mLastTouchPoint = mTouchPoint;
                    mScrollStartPoint = mOriginalPoint = mContainer->getPosition();
                    mTouchMoved = false;
                    mDragging = true;
                    if (mAnimationTimer)
                    {
                        mAnimationTimer->cancel();
                        mAnimationTimer = NULL;
                    }
                    mScrollDistance = HLPointZero;
                    mTouchLength = 0.f;
                    gettimeofday(&mTimestamp,  0);
                    mScollStartTime = mTimestamp;
                }
                else if (touch->touches.size() == 2)
                {
                    mTouchPoint = HLPoint::midpoint(touchLocationInView(&(touch->touches[0])), touchLocationInView(&(touch->touches[1])));
                    mTouchLength = HLPoint::distance((touchLocationInView(&(touch->touches[0]))-mContainer->getPosition())/mZoomScale,
                                                     (touchLocationInView(&(touch->touches[1]))-mContainer->getPosition())/mZoomScale);
                    mDragging = false;
                }
                if (mViewOnTouch)
                {
                    mViewOnTouch->handleTouchEvent(touch);
                }
            }
                break;
            case kHLTouchesMove:
            {
                if (touch->touches.size() == 1 && mDragging)
                {
                    HLPoint newPoint = touchLocationInView(&(touch->touches[0]));
                    HLPoint moveDistance = newPoint - mTouchPoint;
                    // 忽略掉微小的移动
                    if (!mTouchMoved && mViewOnTouch && fabsf(moveDistance.x) < 5 && fabsf(moveDistance.y) < 5)
                    {
                        return;
                    }
                    mTouchMoved = true;
                    //                    mTouchPoint = newPoint;
                    if (touch->touches.size() == 1)
                    {
                        if (mDirection == kHLScrollViewDirectionVertical)
                        {
                            moveDistance.x = 0.f;
                        }
                        else if (mDirection == kHLScrollViewDirectionHorizontal)
                        {
                            moveDistance.y = 0.f;
                        }
                        
                        HLPoint newPos = mOriginalPoint + moveDistance;
                        
                        float newX = MIN(newPos.x, mMaxInset.x);
                        newX = MAX(newX, mMinInset.x);
                        float newY = MIN(newPos.y, mMaxInset.y);
                        newY = MAX(newY, mMinInset.y);
                        
                        if (newX < minContainerOffset().x)
                        {
                            newX = minContainerOffset().x - (minContainerOffset().x - newX)/3;
                        }
                        else if (newX > maxContainerOffset().x) {
                            newX = maxContainerOffset().x + (newX - maxContainerOffset().x)/3;
                        }
                        
                        if (newY < minContainerOffset().y)
                        {
                            newY = minContainerOffset().y - (minContainerOffset().y - newY)/3;
                        }
                        else if (newY > maxContainerOffset().y) {
                            newY = maxContainerOffset().y + (newY - maxContainerOffset().y)/3;
                        }
                        newPos = HLPoint(newX, newY);
                        
                        if (mScrollDistance != HLPointZero)
                        {
                            if (mScrollDistance.x * moveDistance.x < 0 || mScrollDistance.y * moveDistance.y < 0)
                            {
                                mScrollStartPoint = mContainer->getPosition();
                                mScollStartTime = mTimestamp;
                            }
                        }
                        
                        mScrollDistance = newPos - mContainer->getPosition();
                        if (mTouchEnable)
                        {
                            setContentOffset(HLPoint(newX, newY));
                        }
                        
                        gettimeofday(&mTimestamp,  0);
                        mLastTouchPoint = newPoint;
                    }
                }
                else if (touch->touches.size() == 2 && mDragging)
                {
                    const float len = HLPoint::distance((touchLocationInView(&(touch->touches[0]))-mContainer->getPosition())/mZoomScale,
                                                        (touchLocationInView(&(touch->touches[1]))-mContainer->getPosition())/mZoomScale);
                    setZoomScale(mZoomScale*len/mTouchLength);
                }
                if (mViewOnTouch)
                {
                    touch->type = kHLTouchesCancel;
                    mViewOnTouch->handleTouchEvent(touch);
                    mViewOnTouch = NULL;
                }
            }
                break;
            case kHLTouchesEnd:
            {
                if (touch->touches.size() == 1 && mTouchMoved)
                {
                    struct timeval now;
                    gettimeofday(&now,  0);
                    float delta = now.tv_sec-mTimestamp.tv_sec + (now.tv_usec - mTimestamp.tv_usec) / 1000000.0;
                    float winSizeScale = HLDirector2D::getSingleton()->getWinSizeScale();
                    if (!mPaging && (delta > 0.1 || (fabsf(mScrollDistance.x) < 5*winSizeScale && fabsf(mScrollDistance.y) < 5*winSizeScale)))
                    {
                        mScrollDistance = HLPointZero;
                        mScrollSpeed = HLPointZero;
                    }
                    else
                    {
                        mScrollDistance = mContainer->getPosition() - mScrollStartPoint;
                        struct timeval now;
                        gettimeofday(&now,  0);
                        float delta = now.tv_sec-mScollStartTime.tv_sec + (now.tv_usec - mScollStartTime.tv_usec) / 1000000.0;
                        mScrollSpeed = mScrollDistance / delta;
                    }
                    if (mAnimationTimer)
                    {
                        mAnimationTimer->cancel();
                        mAnimationTimer = NULL;
                    }
                    if (mPaging)
                    {
                        relocateContainer(true);
                    }
                    else
                    {
                        mAnimationTimer = new HLTimer(0, true);
                        mDeaccelerating = true;
                        mAnimationTimer->delegate = newDelegate(this, &HLScrollView::deaccelerateScrolling);
                    }
                }
                else if (touch->touches.size() == 1 && !mTouchMoved)
                {
                    mScrollDistance = HLPointZero;
                    mScrollSpeed = HLPointZero;
                    if (mAnimationTimer)
                    {
                        mAnimationTimer->cancel();
                        mAnimationTimer = NULL;
                    }
                    if (mPaging)
                    {
                        relocateContainer(true);
                    }
                    else
                    {
                        mAnimationTimer = new HLTimer(0, true);
                        mDeaccelerating = true;
                        mAnimationTimer->delegate = newDelegate(this, &HLScrollView::deaccelerateScrolling);
                    }
                }
                mDragging = false;
                mTouchMoved = false;
                if (mViewOnTouch)
                {
                    HLView *view = mViewOnTouch;
                    mViewOnTouch = NULL;
                    view->handleTouchEvent(touch);
                }
            }
                break;
            default:
            {
                mDragging = false;
                mTouchMoved = false;
                if (mViewOnTouch)
                {
                    HLView *view = mViewOnTouch;
                    mViewOnTouch = NULL;
                    view->handleTouchEvent(touch);
                }
                mScrollDistance = HLPointZero;
                mScrollSpeed = HLPointZero;
                if (mAnimationTimer)
                {
                    mAnimationTimer->cancel();
                    mAnimationTimer = NULL;
                }
                if (mPaging)
                {
                    relocateContainer(true);
                }
                else
                {
                    mAnimationTimer = new HLTimer(0, true);
                    mDeaccelerating = true;
                    mAnimationTimer->delegate = newDelegate(this, &HLScrollView::deaccelerateScrolling);
                }
            }
                break;
        }
    }
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    void HLScrollView::onMouseEvent(HLMouseEvent* mouseEvent)
    {
        switch (mouseEvent->type)
        {
            case kHLMouseDown:
            {
                mViewOnTouch = mContainer->hitTest(mouseEvent->location);
                mTouchPoint = mouseLocationInView(mouseEvent);
                mLastTouchPoint = mTouchPoint;
                mOriginalPoint = mContainer->getPosition();
                mTouchMoved = false;
                mDragging = true;
                mScrollDistance = HLPointZero;
                mTouchLength = 0.f;
                gettimeofday(&mTimestamp,  0);
                if (mViewOnTouch)
                {
                    mViewOnTouch->handleMouseEvent(mouseEvent);
                }
            }
                break;
            case kHLMouseDrag:
            {
                if (mDragging)
                {
                    mTouchMoved = true;
                    HLPoint newPoint = mouseLocationInView(mouseEvent);
                    HLPoint moveDistance = newPoint - mTouchPoint;
                    //                    mTouchPoint = newPoint;
                    if (mBounds.containsPoint(newPoint))
                    {
                        if (mDirection == kHLScrollViewDirectionVertical)
                        {
                            moveDistance.x = 0.f;
                        }
                        else if (mDirection == kHLScrollViewDirectionHorizontal)
                        {
                            moveDistance.y = 0.f;
                        }
                        
                        HLPoint newPos = mOriginalPoint + moveDistance;
                        
                        float newX = MIN(newPos.x, mMaxInset.x);
                        newX = MAX(newX, mMinInset.x);
                        float newY = MIN(newPos.y, mMaxInset.y);
                        newY = MAX(newY, mMinInset.y);
                        
                        if (newX < minContainerOffset().x)
                        {
                            newX = minContainerOffset().x - (minContainerOffset().x - newX)/3;
                        }
                        else if (newX > maxContainerOffset().x) {
                            newX = maxContainerOffset().x + (newX - maxContainerOffset().x)/3;
                        }
                        
                        if (newY < minContainerOffset().y)
                        {
                            newY = minContainerOffset().y - (minContainerOffset().y - newY)/3;
                        }
                        else if (newY > maxContainerOffset().y) {
                            newY = maxContainerOffset().y + (newY - maxContainerOffset().y)/3;
                        }
                        newPos = HLPoint(newX, newY);
                        mScrollDistance = newPos - mContainer->getPosition();
                        if (mTouchEnable) {
                            setContentOffset(HLPoint(newX, newY));
                        }
                        mLastTouchPoint = newPoint;
                        gettimeofday(&mTimestamp,  0);
                    }
                }
                else
                {
                    if (mViewOnTouch)
                    {
                        mViewOnTouch->handleMouseEvent(mouseEvent);
                        return;
                    }
                }
                
                if (mViewOnTouch)
                {
                    mouseEvent->type = kHLMouseCancel;
                    mViewOnTouch->handleMouseEvent(mouseEvent);
                    mViewOnTouch = NULL;
                }
            }
                break;
            case kHLMouseUp:
            {
                if (mTouchMoved)
                {
                    struct timeval now;
                    gettimeofday(&now,  0);
                    float delta = now.tv_sec-mTimestamp.tv_sec + (now.tv_usec - mTimestamp.tv_usec) / 1000000.0;
                    if (delta > 0.3 || (fabsf(mScrollDistance.x) < 2 && fabsf(mScrollDistance.y) < 2))
                    {
                        mScrollDistance = HLPointZero;
                    }
                    if (mAnimationTimer)
                    {
                        mAnimationTimer->cancel();
                    }
                    mAnimationTimer = new HLTimer(0, true);
                    mDeaccelerating = true;
                    mAnimationTimer->delegate = newDelegate(this, &HLScrollView::deaccelerateScrolling);
                }
                else
                {
                    mScrollDistance = HLPointZero;
                    if (mAnimationTimer)
                    {
                        mAnimationTimer->cancel();
                    }
                    mAnimationTimer = new HLTimer(0, true);
                    mDeaccelerating = true;
                    mAnimationTimer->delegate = newDelegate(this, &HLScrollView::deaccelerateScrolling);
                }
                mDragging = false;
                mTouchMoved = false;
                if (mViewOnTouch)
                {
                    mViewOnTouch->handleMouseEvent(mouseEvent);
                }
            }
                break;
            case kHLMouseCancel:
            {
                mDragging = false;
                mTouchMoved = false;
                if (mViewOnTouch)
                {
                    mViewOnTouch->handleMouseEvent(mouseEvent);
                }
            }
                break;
            default:
                break;
        }
    }
#endif
    
    void HLScrollView::deaccelerateScrolling(HLTimer* timer, float dt)
    {
        if (mDragging)
        {
            mDeaccelerating = false;
            timer->cancel();
            mAnimationTimer = NULL;
            return;
        }
        
        float newX, newY;
        HLPoint maxInset, minInset;
        
        mScrollSpeed = mScrollSpeed * SCROLL_DEACCEL_RATE;
        
        mScrollDistance = mScrollSpeed * dt;
        
        HLPoint newPoint = mContainer->getPosition() + mScrollDistance;
        
        maxInset = maxContainerOffset();
        minInset = minContainerOffset();
        
        if (mBounceable)
        {
            newX = newPoint.x;
            newY = newPoint.y;
            
            //        newX = MIN(newPoint.x, maxInset.x);
            //        newX = MAX(newX, minInset.x);
            //        newY = MIN(newPoint.y, maxInset.y);
            //        newY = MAX(newY, minInset.y);
            
            if (mContainer->getPosition().x > maxInset.x && newPoint.x < maxInset.x)
            {
                newX = maxInset.x;
                mScrollSpeed.x = 0;
            }
            else if (mContainer->getPosition().x < minInset.x && newPoint.x > minInset.x)
            {
                newX = minInset.x;
                mScrollSpeed.x = 0;
            }
            
            if (mContainer->getPosition().y > maxInset.y && newPoint.y < maxInset.y)
            {
                newY = maxInset.y;
                mScrollSpeed.y = 0;
            }
            else if (mContainer->getPosition().y < minInset.y && newPoint.y > minInset.y)
            {
                newY = minInset.y;
                mScrollSpeed.y = 0;
            }
            
            if (newX > maxInset.x + (mMaxInset.x-maxInset.x)/3/2 && mScrollDistance.x >= 0)
            {
                mScrollSpeed.x = -40;
                mScrollSpeed.x /= SCROLL_DEACCEL_RATE;
            }
            else if (newX < minInset.x-(minInset.x - mMinInset.x)/3/2 && mScrollDistance.x <= 0)
            {
                mScrollSpeed.x = 40;
                mScrollSpeed.x /= SCROLL_DEACCEL_RATE;
            }
            
            if (newY > maxInset.y+(mMaxInset.y-maxInset.y)/3/2 && mScrollDistance.y >= 0)
            {
                mScrollSpeed.y = -40;
                mScrollSpeed.y /= SCROLL_DEACCEL_RATE;
            }
            else if (newY < minInset.y-(minInset.y-mMinInset.y)/3/2&& mScrollDistance.y <= 0)
            {
                mScrollSpeed.y = 40;
                mScrollSpeed.y /= SCROLL_DEACCEL_RATE;
            }
            if (mTouchEnable) {
                setContentOffset(HLPoint(newX, newY));
            }
            
            mScrollDistance = mScrollSpeed * dt;
            
            if ((fabsf(mScrollDistance.x) <= SCROLL_DEACCEL_DIST &&
                 fabsf(mScrollDistance.y) <= SCROLL_DEACCEL_DIST))
            {
                mDeaccelerating = false;
                timer->cancel();
                mAnimationTimer = NULL;
                relocateContainer(true);
            }
        }
        else
        {
            newX = MIN(newPoint.x, maxInset.x);
            newX = MAX(newX, minInset.x);
            newY = MIN(newPoint.y, maxInset.y);
            newY = MAX(newY, minInset.y);
            
            if (newX == maxInset.x && mScrollDistance.x > 0)
            {
                mScrollSpeed.x = 0;
            }
            else if (newX < minInset.x && mScrollDistance.x < 0)
            {
                mScrollSpeed.x = 0;
            }
            
            if (newY > maxInset.y && mScrollDistance.y > 0)
            {
                mScrollSpeed.y = 0;
            }
            else if (newY < minInset.y && mScrollDistance.y < 0)
            {
                mScrollSpeed.y = 0;
            }
            
            if (mTouchEnable) {
                setContentOffset(HLPoint(newX, newY));
            }
            
            mScrollDistance = mScrollSpeed * dt;
            
            if ((fabsf(mScrollDistance.x) <= SCROLL_DEACCEL_DIST &&
                 fabsf(mScrollDistance.y) <= SCROLL_DEACCEL_DIST) ||
                ((newX == maxInset.x || newX == minInset.x) &&
                 (newY == maxInset.y || newY == minInset.y)))
            {
                mDeaccelerating = false;
                timer->cancel();
                mAnimationTimer = NULL;
                relocateContainer(true);
            }
        }
    }
    
    HLView* HLScrollView::hitTest(HLPoint p)
    {
        if (!mVisible)
        {
            return NULL;
        }
        if (mUserInteractionEnabled)
        {
            HLPoint p1 = convertToNodeSpace(p);
            p1.x += mEdgeInsets.left;
            p1.y += mEdgeInsets.top;
            if (p1.x < 0 || p1.y < 0 || p1.x > mFrame.size.width || p1.y > mFrame.size.height)
            {
                return NULL;
            }
            return this;
        }
        return NULL;
    }
    
    void HLScrollView::visit()
    {
        if (!mVisible)
        {
            return;
        }
        if (mZoomScale == 1)
        {
            HLView::visit();
        }
    }
    
    bool HLScrollView::onDraggingEvent(const HLViewDraggingEvent& event, HLViewDragSource* dragsource)
    {
        HLView * v = mContainer->hitTest(event.location);
        if (v)
        {
            return v->handleDraggingEvent(event, dragsource);
        }
        return false;
    }
    
    HLView* HLScrollView::clone(HLView* parent)
    {
        HLScrollView* view = new HLScrollView(parent, *this);
        std::list<HLView*>::iterator itr = mContainer->mSubViews.begin();
        for (; itr != mContainer->mSubViews.end(); ++itr)
        {
            (*itr)->clone(view->mContainer);
        }
        return view;
    }
    
#if USE_JS_SCRIPTING
    JSCppWrapperBase* HLScrollView::getOrCreateJSWrapper(JSContext* cx)
    {
        return JSScrollView::getOrCreateWrapper(cx, this);
    }
#endif
}

NS_HL_END
