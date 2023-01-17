//
//  HLTableView.h
//  HoolaiEngine
//
//  Created by liumingliang on 13-2-1.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HoolaiEngine__HLTableView__
#define __HoolaiEngine__HLTableView__

//#include "HLMacros.h"
//#include "HLView.h"
//#include "HLDelegate.h"
//#include "HLTouchDispatcher.h"
//#include "HLTimer.h"
#include "HLScrollView.h"
#include <string>
#include <map>
#include <vector>
#include "HLTableViewCell.h"

NS_HL_BEGIN
namespace gui
{
    class HLTableView;
    class HLTableViewDelegate
    {
    public:
        virtual int tableViewIndentationLevelForRowAtIndexPath(HLTableView* tableView, int indexPath)=0;
        // Called after the user changes the selection.
        virtual void tableViewDidSelectRowAtIndexPath(HLTableView * tableView,int indexPath)=0;
        virtual int tableViewHeightForRowAtIndexPath(HLTableView * tableView,int indexPath)=0;
        
    };
    class HLTableView;
    class HLTableViewDataSource
    {
    public:
        virtual int numberOfSectionsInTableView(HLTableView *tableView)=0; // Default is 1 if not implemented
        virtual int tableViewNumberOfRowsInSection(HLTableView * tableView ,int section)=0;
        // Row display. Implementers should *always* try to reuse cells by setting each cell's reuseIdentifier and querying for available reusable cells with dequeueReusableCellWithIdentifier:
        // Cell gets various attributes set automatically based on table (separators) and data source (accessory views, editing controls)
        virtual HLTableViewCell * tableViewCellForRowAtIndexPath(HLTableView *tableView,int indexPath)=0;
    };
    
    class HLTableView :public HLScrollView
    {
    public:
        HLTableView(HLView * parent);
        HLTableView(HLView * parent, HLRect frame);
        
        void reloadData();// reloads everything from scratch. redisplays visible rows. because we only keep info about visible rows, this is cheap. will adjust offset if table shrinks
        void updateTableViewCell(int atIndex);

        HLTableViewDelegate* _delegate;
        HLTableViewDataSource* _dataSource;
        HLTableViewCell* dequeueReusableCellWithIdentifier(std::string CellIdentifier,int indexPath);
        void visit();
        void setDelegate(HLTableViewDelegate* delegate);
        void setDataSource(HLTableViewDataSource* dataSource);
        int getHeightForRow(int index);
        static HLTableViewCell* CreatCellFromTemplate(HLTableView* parentView,std::string identifier);
        HLTableViewCell* getTableViewCelFromIndex(int atIndex);

    private:
        void initMembers();
        std::map<std::string, std::vector<HLTableViewCell*> > cellsDequeueReusable;

        void updateReloadData(HLTimer* timer, float dt);
        bool FindCellIdentifierFromCellsDequeue(std::string stringIdentifier);

        void anyScrollChanged(HLScrollView* scrollView);
        std::vector<int> m_vHeightForRow;
        int mNumberOfRowsInSection;
        int m_offsetX;
        int m_offsetY;
        void didSelectRowEventEnd(HLPoint pressPoint);
        void tableCellBackgroundReset();
        void didSelectRowEventBegin(HLPoint pressPoint);

        virtual void onTouches(HLTouchEvent* touch);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        void onMouseEvent(HLMouseEvent* mouseEvent);
#endif
        
        virtual void deaccelerateScrolling(HLTimer* timer, float dt);


        void setTableViewCellNotActivation(int atIndex);
        int getCurrentHeight(int atIndex);
        int getCellIndexFromOffset(int offest);

    };
    
    
}

NS_HL_END
#endif /* defined(__HoolaiEngine__HLTableView__) */
