//
//  HLTableViewCell.cpp
//  HoolaiEngine
//
//  Created by liumingliang on 13-2-1.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLTableViewCell.h"
#include "HLLabel.h"
NS_HL_BEGIN
using namespace hoolai;
using namespace gui;

namespace gui
{
  
    
    HLTableViewCell::HLTableViewCell(HLView* _parent,std::string indentifier,hoolai::HLRect rect):HLView(_parent,rect),testLabel(NULL),testButton(NULL),m_BackgroundButton(NULL)
    {
        setIdentifier(indentifier);

           initMembers();
    }
    
    HLTableViewCell::HLTableViewCell(HLView* _parent,std::string indentifier):HLView(_parent),testLabel(NULL),testButton(NULL),m_BackgroundButton(NULL)
    {
        setIdentifier(indentifier);

        initMembers();
    }
    void HLTableViewCell::initMembers()
    {
        setIsActivation(true);
//        setBackgroundButtonVisible(false);
    }
    
    
   
};


NS_HL_END
