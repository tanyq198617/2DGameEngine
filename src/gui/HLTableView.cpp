//
//  HLTableView.cpp
//  HoolaiEngine
//
//  Created by liumingliang on 13-2-1.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLTableView.h"
#include "HLView.h"
#include "HLDrawingPrimitives.h"
#include <iostream>
#include "kazmath/GL/matrix.h"
#include "HLDirector2D.h"
#include "HLAlertView.h"
#include "HLTexture.h"

NS_HL_BEGIN

#define TABLECELL_HEIGHT    25
using namespace hoolai;
using namespace gui;
using namespace std;

namespace gui
{
    HLTableView::HLTableView(HLView * parent):HLScrollView(parent)
    {
        initMembers();
    }
    
    HLTableView::HLTableView(HLView * parent, HLRect frame):HLScrollView(parent, frame)
    {
        initMembers();
    }
    
    void HLTableView::initMembers()
    {
        scrollViewDidScroll = newDelegate(this, &HLTableView::anyScrollChanged);
        _delegate=NULL;
        _dataSource=NULL;
        m_offsetX = - this->getContentOffset().x;
        m_offsetY = -  this->getContentOffset().y;
        mDirection = kHLScrollViewDirectionVertical;
        color4B color;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        color.a = 0;
        
        setBackgroundColor(color);
        HLTimer *timer = new HLTimer(0,false);
        timer->delegate = newDelegate(this, &HLTableView::updateReloadData); //
    }
    void HLTableView::setDelegate(HLTableViewDelegate* delegate)
    {
        _delegate=delegate;
    }
    void HLTableView::setDataSource(HLTableViewDataSource* dataSource)
    {
        _dataSource=dataSource;
    }
    int HLTableView::getHeightForRow(int index)
    {
        return m_vHeightForRow[index];
    }
    
    void HLTableView::updateReloadData(HLTimer* timer, float dt)
    {
        reloadData();
    }
    bool HLTableView::FindCellIdentifierFromCellsDequeue(string stringIdentifier)
    {
        map<string,vector<HLTableViewCell*> >::iterator itr;
        itr = cellsDequeueReusable.find(stringIdentifier);
        if (itr != cellsDequeueReusable.end())
        {
            return true;
        }
        
        return false;
    }
    //TODO: Need to create from the template
    HLTableViewCell* HLTableView::CreatCellFromTemplate(HLTableView* tableView,string identifier)
    {
        HLTableViewCell* cell=NULL;
        cell = new HLTableViewCell(tableView,identifier);
        cell->setIsActivation(true);
        cell->setFrame( hoolai::HLRect(0,0,200,200));
        
        cell->m_BackgroundButton = new HLButton(cell, HLRect(0,0,200,50));//from the temlate
//        hoolai::HLTexture *tex = hoolai::HLTexture::getTexture("button.png");
//        cell->m_BackgroundButton->setNormalBackgroundImage(tex, hoolai::HLRect(0.5,0.5,0,0));
        cell->m_BackgroundButton->setUserInteractionEnabled(false);
        
//        cell->
        //init some var
        if (cell->testLabel==NULL) {
            cell->testLabel = new hoolai::gui::HLLabel(cell, hoolai::HLRect(0,0,200,200));
            cell->testLabel->setTextAlignment(hoolai::gui::kTextHAlignmentCenter|hoolai::gui::kTextVAlignmentTop);
        }
        cell->testButton =new hoolai::gui::HLButton(cell, hoolai::HLRect(0,0,40,20));
        
      
//        cell->m_BackgroundButton->setNormalTitle("22");
//        cell->m_BackgroundButton->setTitleSize(8);
        
//        HLButton* button = new HLButton(cell, HLRect(50,0,20,20));
//        cell->setBackgroundView(button);
//        cell->setSelected(false);
        
        
        return cell;
    }
    
    int HLTableView::getCellIndexFromOffset(int offest)
    {
        int tempOffset=0;
        for (int i=0; i<m_vHeightForRow.size(); i++) {
            tempOffset+=m_vHeightForRow[i];
            if (tempOffset>offest) {
                return i;
            }
        }
        return (int)m_vHeightForRow.size();
    }
    int HLTableView::getCurrentHeight(int atIndex)
    {
        int tempHeight=0;
        if (atIndex==0) {
            return 0;
        }
        for (int i=0; i<atIndex; i++) {
            tempHeight+=m_vHeightForRow[i];
            if (i==atIndex-1) {
                return tempHeight;
            }
        }
        return 0;
    }
    
    //
    void HLTableView::reloadData()
    {
        HLASSERT(_dataSource, "error!you should set DataSource before using reloadData()!");
        
        m_vHeightForRow.clear();
        mNumberOfRowsInSection=0;
        cellsDequeueReusable.clear();
        
        mContainer->removeAllSubviews();

        //cout
        mNumberOfRowsInSection=_dataSource->tableViewNumberOfRowsInSection(this, 0);
        //hight       
        for (int i=0; i<mNumberOfRowsInSection; i++)
        {
           int mHeightForRow = _delegate->tableViewHeightForRowAtIndexPath(this,i);//单排固定高度
           m_vHeightForRow.push_back(mHeightForRow);
        }
        
        int xOffset = -getContentOffset().x;
        int yOffset = -getContentOffset().y;
        int beginIndex= getCellIndexFromOffset(yOffset);
        int endIndex=getCellIndexFromOffset(yOffset+getFrame().size.height);///mHeightForRow;
        if (beginIndex<0)
        {
            beginIndex=0;
        }
        if (endIndex>mNumberOfRowsInSection) {
            endIndex=mNumberOfRowsInSection;
        }
//        for (int i=beginIndex; i<endIndex; i++)
//        {
//            updateTableViewCell(i);
//        }
        for (int i=0; i<mNumberOfRowsInSection; i++) {
            updateTableViewCell(i);
        }

    }
   
    void HLTableView::updateTableViewCell(int atIndex)
    {
//        printf("更新 atIndex=%d\n",atIndex);
        HLTableViewCell* tableViewCell = _dataSource->tableViewCellForRowAtIndexPath(this,atIndex);
        tableViewCell->setTableCellTag(atIndex);
        tableViewCell->setPosition(0,0+getCurrentHeight(atIndex));
        
        map<string,vector<HLTableViewCell*> >::iterator itr;
        
        itr = cellsDequeueReusable.find(tableViewCell->getIsIdentifier());
        

        if (itr != cellsDequeueReusable.end())
        {
            vector<HLTableViewCell*>& views= (vector<HLTableViewCell*>&)(itr->second);
            bool hasB=false;
            for (int i=0; i<views.size(); i++)
            {
                if (views[i]->getTableCellTag()==tableViewCell->getTableCellTag())
                {
                    hasB=true;
                    break;
                }
            }
            if (hasB==false)
            {
                views.push_back(tableViewCell);
            }
        }else
        {
            vector<HLTableViewCell*> views;
            views.push_back(tableViewCell);
            cellsDequeueReusable.insert( make_pair(tableViewCell->getIsIdentifier(),views));
        }
    }
 
    void HLTableView::visit()
    {
        if (!mVisible)
        {
            return;
        }

//        kmGLPushMatrix();
//        kmGLMultMatrix(&mTransform);
//        drawColor4B(mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, mBackgroundColor.a);
//        drawSolidRect(HLPoint(-mEdgeInsets.left, -mEdgeInsets.bottom), HLPoint(mFrame.size.width, mFrame.size.height));
        
        if (mZoomScale == 1)
        {
            HLView::visit();
        }
        
//        kmGLPopMatrix();

//
//        
//        bool scissorEnabled = hlglIsEnabled(GL_SCISSOR_TEST);
//        GLint scissorBox[4];
//        if (mClipToBounds)
//        {
//            if (scissorEnabled)
//            {
//                hlglGetScissorBox(scissorBox);
//            }
//            else
//            {
//                hlglEnable(GL_SCISSOR_TEST);
//            }
//#ifdef TARGET_IOS
//            float scale = 1/HLDirector2D::getSingleton()->getContentScaleFactor();
//#else
//            float scale = HLDirector2D::getSingleton()->getWinSizeScale();
//#endif
//            kmMat4* transform = nodeToWorldTransform();
//            hlglScissor(roundf(transform->mat[12]/scale), roundf(transform->mat[13]/scale), mBounds.size.width/scale, mBounds.size.height/scale);
//        }
//        onDraw();
//        std::list<HLView*>::const_iterator itr = mSubViews.begin();
//        for (; itr != mSubViews.end(); ++itr)
//        {
//            if ((*itr)->isVisible())
//            {
//                (*itr)->visit();
//            }
//        }
//        if (mClipToBounds)
//        {
//            if (scissorEnabled)
//            {
//                hlglScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
//            }
//            else
//            {
//                hlglDisable(GL_SCISSOR_TEST);
//            }
//        }
//        kmGLPopMatrix();
    }
    //
    HLTableViewCell* HLTableView::dequeueReusableCellWithIdentifier(std::string CellIdentifier,int indexPath)
    {
        HLTableViewCell* cell=NULL;
        if (FindCellIdentifierFromCellsDequeue(CellIdentifier)==false) {
                return NULL;
        }else 
        {
            //To find whether the cells activation
            map<string,vector<HLTableViewCell*> >::iterator itr;
            
            itr = cellsDequeueReusable.find(CellIdentifier);
            
            if (itr != cellsDequeueReusable.end())
            {
                vector<HLTableViewCell*> cells= (vector<HLTableViewCell*>)(itr->second);
                int noActivationIndex=-1;
                for (int i=0; i<cells.size(); i++)
                {
                    if (cells[i]->getTableCellTag()==indexPath)
                    {
                        cells[i]->setIsActivation(true);
                        return cells[i];
                    }
                    if (cells[i]->getIsActivation()==false)
                    {
                        noActivationIndex=i;
                    }
                }
                if (noActivationIndex!=-1) {
                    cells[noActivationIndex]->setIsActivation(true);
                    return cells[noActivationIndex];//->setIsActivation(true);
                }

                return NULL;
            }else{
                //not activation ,copy the same style cell and return.
                //vector<HLTableViewCell*> cells= (vector<HLTableViewCell*>)(itr->second);
                //or consider to use deap copy???
                // cell = HLTableViewCell::CreatCellFromTemplate(CellIdentifier);
                return NULL;
            }
            
            
        }
        
        return cell;
    }
    
    void HLTableView::anyScrollChanged(HLScrollView* scrollView)
    {
        int pre_beginIndex= getCellIndexFromOffset(m_offsetY);// m_offsetY/mHeightForRow;
        int pre_endIndex=getCellIndexFromOffset(m_offsetY+getFrame().size.height);///mHeightForRow;
        
        int xOffset= - scrollView->getContentOffset().x;
        int yOffset= - scrollView->getContentOffset().y;
        
        int now_beginIndex=getCellIndexFromOffset(yOffset);
        int now_endIndex=getCellIndexFromOffset(yOffset+getFrame().size.height);///mHeightForRow;
//        printf("%ld",cellsDequeueReusable.size());
        
//-------
//        pre_beginIndex<0?0:pre_beginIndex;
//        pre_endIndex>mNumberOfRowsInSection?mNumberOfRowsInSection:pre_endIndex;
//        now_beginIndex<0?0:now_beginIndex;
//        now_endIndex>mNumberOfRowsInSection?mNumberOfRowsInSection:now_endIndex;
        
        if (pre_beginIndex<=now_beginIndex&&pre_endIndex>=now_endIndex) {
            return;
        }
        m_offsetX=xOffset;
        m_offsetY=yOffset;
        
        if (now_endIndex>pre_endIndex) {
         
            for (int i=pre_endIndex; i<(now_endIndex<mNumberOfRowsInSection?now_endIndex+1:now_endIndex); i++)//Update a row in advance
            {
                updateTableViewCell(i);
            }
            if (now_beginIndex>pre_beginIndex) {
                for (int i=pre_beginIndex; i<+now_beginIndex; i++) {
                    setTableViewCellNotActivation(i);
                }
            }
        }

        if (now_beginIndex<pre_beginIndex) {
         
            for (int i=now_beginIndex>0?(now_beginIndex-1):now_beginIndex; i<pre_beginIndex; i++) {//Update a row in advance
                updateTableViewCell(i);
            }
            if (pre_endIndex>now_endIndex) {
                for (int i=now_endIndex; i<pre_endIndex; i++) {
                    setTableViewCellNotActivation(i);
                }
            }
        }
        
    }
    HLTableViewCell* HLTableView::getTableViewCelFromIndex(int atIndex)
    {
        map<string,vector<HLTableViewCell*> >::iterator itr;
        vector<HLTableViewCell*> cells;
        for (itr = cellsDequeueReusable.begin(); itr != cellsDequeueReusable.end(); ++itr )
        {
            cells=itr->second;
            for (int i=0; i<cells.size(); i++)
            {
                if (cells[i]->getTableCellTag()==atIndex) {

                    return cells[i];
                }
            }
        }
        return cells[0];
    }
    
    void HLTableView::setTableViewCellNotActivation(int atIndex)
    {
//        map<string,vector<HLTableViewCell*> >::iterator itr;
//        
//        for (itr = cellsDequeueReusable.begin(); itr != cellsDequeueReusable.end(); ++itr )
//        {
//            vector<HLTableViewCell*> cells=itr->second;
//            for (int i=0; i<cells.size(); i++)
//            {
//                if (cells[i]->getTableCellTag()==atIndex) {
//                    cells[i]->setIsActivation(false);
//                    break;
//                }
//            }
//        }
        HLTableViewCell* cell = getTableViewCelFromIndex(atIndex);
        cell->setIsActivation(false);
        
    }
    //###
    void HLTableView::didSelectRowEventBegin(HLPoint pressPoint)
    {
        if (mNumberOfRowsInSection<=0) {
            return;
        }
        int pressPointX=pressPoint.x;
        int pressPointY=pressPoint.y;
        int xOffset= - getContentOffset().x;
        int yOffset= - getContentOffset().y;
        if (yOffset<0) {
            return;
        }
        int didSelIndex=getCellIndexFromOffset(pressPointY+yOffset);///mHeightForRow;
        
//        cell->m_BackgroundButton->setEnabled(false);
      
        HLTableViewCell* cell = getTableViewCelFromIndex(didSelIndex);
        cell->m_BackgroundButton->setEnabled(false);
//        m_BackgroundButton
    }
    //###
    void HLTableView::didSelectRowEventEnd(HLPoint pressPoint)
    {
        int pressPointX=pressPoint.x;
        int pressPointY=pressPoint.y;
        int xOffset= - getContentOffset().x;
        int yOffset= - getContentOffset().y;
        if (yOffset<0) {
            return;
        }
        //        int now_beginIndex=yOffset/mHeightForRow;
        //        int now_endIndex=(yOffset+getFrame().size.height)/mHeightForRow;
        int didSelIndex=getCellIndexFromOffset(pressPointY+yOffset);///mHeightForRow;
        
        if (didSelIndex+1>mNumberOfRowsInSection) {
            return;
        }
        _delegate->tableViewDidSelectRowAtIndexPath(this, didSelIndex);
    }
    
     void HLTableView::tableCellBackgroundReset()
    {
        map<string,vector<HLTableViewCell*> >::iterator itr;
        vector<HLTableViewCell*> cells;
        for (itr = cellsDequeueReusable.begin(); itr != cellsDequeueReusable.end(); ++itr )
        {
            cells=itr->second;
            for (int i=0; i<cells.size(); i++)
            {
                if(cells[i]->m_BackgroundButton->getButtonState()!=HLButton::kButtonStateNormal)
                {
                cells[i]->m_BackgroundButton->setEnabled(true);
                }
            }
        }
    }
    
    
    void HLTableView::onTouches(HLTouchEvent* touch)
    {
        switch (touch->type)
        {
            case kHLTouchesBegin:
            {
                mViewOnTouch = mContainer->hitTest(touch->touches[0].location);
                if (touch->touches.size() == 1)
                {
                    mTouchPoint = touchLocationInView(&(touch->touches[0]));
                    mLastTouchPoint = mTouchPoint;
                    mOriginalPoint = mContainer->getPosition();
                    mTouchMoved = false;
                    mDragging = true;
                    mScrollDistance = HLPointZero;
                    mTouchLength = 0.f;
                    gettimeofday(&mTimestamp,  0);
               
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
                    mViewOnTouch->onTouches(touch);
                }else{
                    
                    HLPoint newPoint = touchLocationInView(&(touch->touches[0]));
                    
                    didSelectRowEventBegin(newPoint);
                }
            }
                break;
            case kHLTouchesMove:
            {
                if (touch->touches.size() == 1 && mDragging)
                {
                    HLPoint newPoint = touchLocationInView(&(touch->touches[0]));
                    HLPoint moveDistance = newPoint - mTouchPoint;

                    if (!mTouchMoved && mViewOnTouch && fabsf(moveDistance.x) < 5 && fabsf(moveDistance.y) < 5)
                    {
                        return;
                    }
                    mTouchMoved = true;
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
                        setContentOffset(HLPoint(newX, newY));
                        mLastTouchPoint = newPoint;
                        gettimeofday(&mTimestamp,  0);
                    }
                }
                else if (touch->touches.size() == 2 && mDragging)
                {
                    const float len = HLPoint::distance((touchLocationInView(&(touch->touches[0]))-mContainer->getPosition())/mZoomScale,
                                                        (touchLocationInView(&(touch->touches[1]))-mContainer->getPosition())/mZoomScale);
                    setZoomScale(mZoomScale*len/mTouchLength);
                }
                else
                {
                    if (mViewOnTouch)
                    {
                        mViewOnTouch->onTouches(touch);
                        return;
                    }
                }
                
                if (mViewOnTouch)
                {
                    touch->type = kHLTouchesCancel;
                    mViewOnTouch->onTouches(touch);
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
                    if (delta > 0.3 || (fabsf(mScrollDistance.x) < 2 && fabsf(mScrollDistance.y) < 2))
                    {
                        mScrollDistance = HLPointZero;
                    }
                    HLTimer* timer = new HLTimer(0, true);
                    mDeaccelerating = true;
                    timer->delegate = newDelegate(this, &HLTableView::deaccelerateScrolling);
                }else if (touch->touches.size()==1&&!mViewOnTouch)
                {
                    //                    HLPoint newPoint = mouseLocationInView(mouseEvent);
                    HLPoint newPoint = touchLocationInView(&(touch->touches[0]));
                    
                    didSelectRowEventEnd(newPoint);
                }
                mDragging = false;
                mTouchMoved = false;
                if (mViewOnTouch)
                {
                    mViewOnTouch->onTouches(touch);
                }
                tableCellBackgroundReset();
            }
                break;
            default:
            {
                mDragging = false;
                mTouchMoved = false;
                if (mViewOnTouch)
                {
                    mViewOnTouch->onTouches(touch);
                }
            }
                break;
        }
    }
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    void HLTableView::onMouseEvent(HLMouseEvent* mouseEvent)
    {
        if (mouseEvent->clickCount>1) {
            return;
        }
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
                    mViewOnTouch->onMouseEvent(mouseEvent);
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
                        setContentOffset(HLPoint(newX, newY));
                        mLastTouchPoint = newPoint;
                        gettimeofday(&mTimestamp,  0);
                    }
                }
                else
                {
                    if (mViewOnTouch)
                    {
                        mViewOnTouch->onMouseEvent(mouseEvent);
                        return;
                    }
                }
                
                if (mViewOnTouch)
                {
                    mouseEvent->type = kHLMouseCancel;
                    mViewOnTouch->onMouseEvent(mouseEvent);
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
                    HLTimer* timer = new HLTimer(0, true);
                    mDeaccelerating = true;
                    timer->delegate = newDelegate(this, &HLTableView::deaccelerateScrolling);
                }else{
                    HLPoint newPoint = mouseLocationInView(mouseEvent);
                    
                    didSelectRowEventEnd(newPoint);
                }
                mDragging = false;
                mTouchMoved = false;
                if (mViewOnTouch)
                {
                    mViewOnTouch->onMouseEvent(mouseEvent);
                }
            }
                break;
            case kHLMouseCancel:
            {
                mDragging = false;
                mTouchMoved = false;
                if (mViewOnTouch)
                {
                    mViewOnTouch->onMouseEvent(mouseEvent);
                }
                tableCellBackgroundReset();
            }
                break;
            default:
                break;
        }
    }
#endif
    void HLTableView::deaccelerateScrolling(HLTimer* timer, float dt)
    {
        HLScrollView::deaccelerateScrolling(timer, dt);
    }
    
    
};
NS_HL_END