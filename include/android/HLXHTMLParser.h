//
//  HLXHTMLParser.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-6-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLXHTMLPARSER_H__
#define __HLXHTMLPARSER_H__

#include "HLMacros.h"
#include <string>
#include <map>

NS_HL_BEGIN

class HLXHTMLParser;

class HLXHTMLParserDelegate
{
public:
    virtual void parserDidStartDocument(HLXHTMLParser* parser)=0;
    virtual void parserDidEndDocument(HLXHTMLParser* parser)=0;
    virtual void parserDidStartElement(HLXHTMLParser* parser, std::string elementName, const std::map<std::string, std::string>& attributes)=0;
    virtual void parserDidEndElement(HLXHTMLParser* parser, std::string elementName)=0;
    virtual void parserFoundCharacters(std::string string)=0;
};

class HLXHTMLParser
{
public:
    HLXHTMLParserDelegate* delegate;
    HLXHTMLParser(std::string s, HLXHTMLParserDelegate* del):str(s),delegate(del){}
    void parse();
private:
    std::string str;
};

NS_HL_END

#endif
