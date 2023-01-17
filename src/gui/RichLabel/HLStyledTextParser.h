//
//  HLStyledTextParser.h
//  HoolaiEngine
//
//  Created by tyq on 13-6-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLSTYLEDTEXTPARSER_H__
#define __HLSTYLEDTEXTPARSER_H__

#include "HLMacros.h"
#include <vector>
#include "HLXHTMLParser.h"

NS_HL_BEGIN

class HLStyledNode;
class HLStyledElement;

class HLStyledTextParser: public HLXHTMLParserDelegate
{
public:
    HLStyledTextParser():rootNode(NULL),topElement(NULL),lastNode(NULL){}
    void parseXHTML(std::string html);
    HLStyledNode* getRootNode() {return rootNode;}
    
    virtual void parserDidStartDocument(HLXHTMLParser* parser);
    virtual void parserDidEndDocument(HLXHTMLParser* parser);
    virtual void parserDidStartElement(HLXHTMLParser* parser, std::string elementName, const std::map<std::string, std::string>& attributes);
    virtual void parserDidEndElement(HLXHTMLParser* parser, std::string elementName);
    virtual void parserFoundCharacters(std::string string);
private:
    void addNode(HLStyledNode* node);
    void pushNode(HLStyledNode* node);
    void popNode();
    void flushCharacters();
    void parseText(std::string str);
    HLStyledNode* rootNode;
    HLStyledNode* topElement;
    HLStyledNode* lastNode;
    std::string chars;
    std::vector<HLStyledNode*> stack;
};

NS_HL_END

#endif
