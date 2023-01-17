//
//  HLStyledTextParser.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-6-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLStyledTextParser.h"
#include <string>
#include "HLStyledNode.h"
#include "HLStringUtil.h"
#if defined(TARGET_WIN32)
#define strcasecmp stricmp
#endif

NS_HL_BEGIN

void HLStyledTextParser::parseXHTML(std::string html)
{
    std::string doc = "<x>"+html+"</x>";
    HLXHTMLParser parser(doc, this);
    parser.parse();
}

void HLStyledTextParser::addNode(HLStyledNode* node)
{
    if (!rootNode)
    {
        rootNode = node;
        lastNode = node;
    }
    else if (topElement)
    {
        topElement->addChild(node);
    }
    else
    {
        lastNode->setNextSibling(node);
        lastNode = node;
    }
}

void HLStyledTextParser::pushNode(HLStyledNode* node)
{
    addNode(node);
    stack.push_back(node);
    topElement = node;
}

void HLStyledTextParser::popNode()
{
    if (!stack.empty())
    {
        stack.pop_back();
    }
    if (!stack.empty())
    {
        topElement = stack.back();
    }
    else
    {
        topElement = NULL;
    }
}

void HLStyledTextParser::flushCharacters()
{
    if (chars.length())
    {
        parseText(chars);
    }
    chars.clear();
}

void HLStyledTextParser::parseText(std::string str)
{
    size_t index = 0;
    size_t length = str.length();
    StringUtil::ReplaceAll(str, "&lt;", "<");
    StringUtil::ReplaceAll(str, "&gt;", ">");
    while (1)
    {
        size_t pos = str.find("\n", index, length - index);
        if (pos != std::string::npos)
        {
            std::string substr = str.substr(index, pos - index);
            if (substr.length())
            {
                HLStyledTextNode* node = new HLStyledTextNode();
                node->text = substr;
                addNode(node);
            }
            HLStyledLineBreakNode* node = new HLStyledLineBreakNode();
            addNode(node);
            index += substr.length() + 1;
        }
        else
        {
            std::string substr = str.substr(index);
            if (substr.length())
            {
                HLStyledTextNode* node = new HLStyledTextNode();
                node->text = substr;
                addNode(node);
                break;
            }
        }
    }
}

void HLStyledTextParser::parserDidStartDocument(HLXHTMLParser* parser)
{
    
}

void HLStyledTextParser::parserDidEndDocument(HLXHTMLParser* parser)
{
    
}

void HLStyledTextParser::parserDidStartElement(HLXHTMLParser* parser, std::string elementName, const std::map<std::string, std::string>& attributes)
{
    flushCharacters();
    
    if (!strcasecmp(elementName.c_str(), "br"))
    {
        HLStyledLineBreakNode* node = new HLStyledLineBreakNode();
        pushNode(node);
    }
    else if (!strcasecmp(elementName.c_str(), "b"))
    {
        HLStyledBoldNode* node = new HLStyledBoldNode();
        pushNode(node);
    }
    else if (!strcasecmp(elementName.c_str(), "u"))
    {
        HLStyledUnderlineNode* node = new HLStyledUnderlineNode();
        pushNode(node);
    }
    else if (!strcasecmp(elementName.c_str(), "a"))
    {
        HLStyledLinkNode* node = new HLStyledLinkNode();
        std::map<std::string, std::string>::const_iterator itr = attributes.find("href");
        if (itr != attributes.end())
        {
            node->href = itr->second;
        }
        pushNode(node);
    }
    else if (!strcasecmp(elementName.c_str(), "font"))
    {
        HLStyledFontNode* node = new HLStyledFontNode();
        std::map<std::string, std::string>::const_iterator itr = attributes.find("face");
        if (itr != attributes.end())
        {
            node->face = itr->second;
        }
        itr = attributes.find("size");
        if (itr != attributes.end())
        {
            node->size = atoi(itr->second.c_str());
        }
        else
        {
            node->size = 0;
        }
        itr = attributes.find("color");
        if (itr != attributes.end())
        {
            std::string str = itr->second;
            if (str[0] == '#')
                str = str.substr(1);
            int color = 0;
            int len = (int)str.length();
            for (int i = 0; i < len; ++i)
            {
                int t;
                if (str[i]>='a'&&str[i]<='f')
                {
                    t = 10 + str[i] - 'a';
                }
                else if (str[i]>='A'&&str[i]<='F')
                {
                    t = 10 + str[i] - 'A';
                }
                else
                {
                    t = str[i] - '0';
                }
                color *= 16;
                color += t;
            }
            if (len > 6)
            {
                node->color.r = (color>>24)/255.f;
                node->color.g = ((color&0xff0000)>>16)/255.f;
                node->color.b = ((color&0xff00)>>8)/255.f;
                node->color.a = (color&0xff)/255.f;
            }
            else
            {
                node->color.r = (color>>16)/255.f;
                node->color.g = ((color&0xff00)>>8)/255.f;
                node->color.b = (color&0xff)/255.f;
                node->color.a = 1;
            }
        }
        else
        {
            node->color.r = -1;
        }
        pushNode(node);
    }
    else if (!strcasecmp(elementName.c_str(), "img"))
    {
        HLStyledImageNode* node = new HLStyledImageNode();
        std::map<std::string, std::string>::const_iterator itr = attributes.find("src");
        if (itr != attributes.end())
        {
            node->src = itr->second;
        }
        itr = attributes.find("width");
        if (itr != attributes.end())
        {
            node->size.width = atof(itr->second.c_str());
        }
        itr = attributes.find("height");
        if (itr != attributes.end())
        {
            node->size.height = atof(itr->second.c_str());
        }
        pushNode(node);
    }
}

void HLStyledTextParser::parserDidEndElement(HLXHTMLParser* parser, std::string elementName)
{
    flushCharacters();
    popNode();
}

void HLStyledTextParser::parserFoundCharacters(std::string string)
{
    chars += string;
}

NS_HL_END
