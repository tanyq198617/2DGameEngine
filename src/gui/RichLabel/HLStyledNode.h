//
//  HLStyledNode.h
//  HoolaiEngine
//
//  Created by tyq on 13-6-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLSTYLEDNODE_H__
#define __HLSTYLEDNODE_H__

#include "HLMacros.h"
#include <iostream>
#include "HLUtil.h"
#include "HLGeometry.h"

NS_HL_BEGIN

class HLStyledNode
{
public:
    HLStyledNode():nextSibling(NULL),parentNode(NULL),firstChild(NULL),lastChild(NULL) {}
    virtual ~HLStyledNode();
    HLStyledNode *nextSibling;
    HLStyledNode *parentNode;
    HLStyledNode *firstChild;
    HLStyledNode *lastChild;
    
    HLStyledNode* findLastSibling(HLStyledNode* sibling);
    void setNextSibling(HLStyledNode* node);
    
    void addChild(HLStyledNode* child);
    
    HLStyledNode* ancestorOrSelfWithClass(const char* cls);
    
    virtual const char* getType() {return "";}
};

class HLStyledTextNode: public HLStyledNode
{
public:
    std::string text;
    
    virtual const char* getType() {return "TEXT";}
};

class HLStyledBoldNode: public HLStyledNode
{
public:
    virtual const char* getType() {return "BOLD";}
};

class HLStyledUnderlineNode: public HLStyledNode
{
public:
    virtual const char* getType() {return "UNDERLINE";}
};

class HLStyledLineBreakNode: public HLStyledNode
{
public:
    virtual const char* getType() {return "LINEBREAK";}
};

class HLStyledFontNode: public HLStyledNode
{
public:
    std::string face;
    int size;
    color4F color;
    virtual const char* getType() {return "FONT";}
};

class HLStyledLinkNode: public HLStyledNode
{
public:
    std::string href;
    virtual const char* getType() {return "LINK";}
};

class HLStyledImageNode: public HLStyledNode
{
public:
    std::string src;
    HLSize size;
    virtual const char* getType() {return "IMG";}
};

NS_HL_END

#endif
