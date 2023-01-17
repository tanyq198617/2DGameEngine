//
//  HLStyledNode.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-6-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLStyledNode.h"

NS_HL_BEGIN

HLStyledNode::~HLStyledNode()
{
    if (nextSibling)
    {
        delete nextSibling;
    }
    if (firstChild)
    {
        delete firstChild;
    }
}

HLStyledNode* HLStyledNode::findLastSibling(HLStyledNode* sibling)
{
    while (sibling)
    {
        if (!sibling->nextSibling)
        {
            return sibling;
        }
        sibling = sibling->nextSibling;
    }
    return NULL;
}

HLStyledNode* HLStyledNode::ancestorOrSelfWithClass(const char* cls)
{
    if (!strcmp(getType(), cls))
    {
        return this;
    }
    else
    {
        return parentNode->ancestorOrSelfWithClass(cls);
    }
}

void HLStyledNode::setNextSibling(HLStyledNode* node)
{
    if (node != nextSibling)
    {
        if (nextSibling)
        {
            delete nextSibling;
        }
        nextSibling = node;
        node->parentNode = parentNode;
    }
}

void HLStyledNode::addChild(HLStyledNode* child)
{
    if (!firstChild)
    {
        firstChild = child;
        lastChild = findLastSibling(child);
    }
    else
    {
        lastChild->nextSibling = child;
        lastChild = findLastSibling(child);
    }
    child->parentNode = this;
}

NS_HL_END
