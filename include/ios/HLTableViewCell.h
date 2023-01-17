//
//  HLTableViewCell.h
//  HoolaiEngine
//
//  Created by liumingliang on 13-2-1.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLTABLEVIEWCELL_H__
#define __HLTABLEVIEWCELL_H__

#include <iostream>
#include "HLView.h"
#include "HLLabel.h"
#include "HLButton.h"
NS_HL_BEGIN

namespace gui
{
    
class HLTableViewCell: public hoolai::gui::HLView
{
    public:
    typedef enum
    {
        kTableViewCellStateNormal,
        kTableViewCellStateSelected,
    } eHLTableViewCell;
    
    HLLabel *testLabel;
    HLButton *testButton;

    HLTableViewCell(HLView* _parent,std::string indentifier,hoolai::HLRect rect);
    HLTableViewCell(HLView* _parent,std::string indentifier);

    inline void setIdentifier(std::string indentifier)
    {
        mIdentifier=indentifier;
    }
    inline std::string getIsIdentifier()
    {
        return mIdentifier;
    }
    inline void setIsActivation(bool _isActivation)
    {
        isActivation=_isActivation;
    }
    inline bool getIsActivation()
    {
        return isActivation;
    }
    inline void setTableCellTag(int _tableCellTag)
    {
        tableCellTag=_tableCellTag;
    }
    inline int getTableCellTag()
    {
        return tableCellTag;
    }
    inline bool getSelected()
    {
        if(HLButton::kButtonStateNormal==m_BackgroundButton->getButtonState())
       {
           return true;
       }
        return false;
    }
    inline void setSelected(bool selected)
    {
        m_BackgroundButton->setSelected(selected);
    }
    inline void setBackgroundButtonVisible(bool Visible)
    {
        m_BackgroundButton->setVisible(Visible);
    }
//    inline void setBackgroundView(HLButton* button)
//    {
//        m_BackgroundButton=button;
//    }
//    inline HLButton* getBackgroundView()
//    {
//        return m_BackgroundButton;
//    }

    HLButton* m_BackgroundButton;

private:

    void initMembers();

    std::string mIdentifier;
    bool isActivation;
    int tableCellTag;
//    int mSelectedState;
//    HLSpriteFrame* mNormalBackgroundTexture;
//    HLSpriteFrame* mSelectedBackgroundTexture;
//
//    HLImageView* mBackgroundImageView;
//    void statusChanged();
//    void setNormalBackgroundImage(HLTexture* tex, HLRect capInsets);
//    void setNormalBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets);
    
    
};
    
}

NS_HL_END

    
#endif /* defined(__HoolaiEngine__HLTableViewCell__) */
