//
//  HLXHTMLParser.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-6-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLXHTMLParser.h"
#include "HLStringUtil.h"

NS_HL_BEGIN

struct Range
{
    size_t location;
    size_t length;
};

void HLXHTMLParser::parse()
{
    if (!delegate)
    {
        return;
    }
    size_t length = str.length();
    delegate->parserDidStartDocument(this);
    for (size_t i = 0; i < length; )
    {
        if (str[i] == '<')
        {
            if (str[i+1] == '/') // end element
            {
                Range range;
                range.location = i;
                range.length = 0;
                for (size_t j = i; ; ++j)
                {
                    if (str[j]=='>')
                    {
                        range.length = j+1-i;
                        break;
                    }
                }
                HLASSERT(range.length>0, "error on parse html");
                {
                    Range range1 = range;
                    range1.location += 2;
                    range1.length -= 3;
                    std::string elname = str.substr(range1.location, range1.length);
                    elname = StringUtil::Trim(elname);
                    delegate->parserDidEndElement(this, elname);
                }
                i += range.length;
                continue;
            }
            else // start element
            {
                size_t j = i + 1;
                std::string elname;
                std::map<std::string, std::string> attrDic;
                while (str[j] == ' ')
                {
                    ++j;
                }
                Range range;
                range.location = j;
                range.length = 0;
                while (str[j] != ' ' && str[j] != '>' && str[j] != '/') // handle element name
                {
                    ++j;
                }
                range.length = j - range.location;
                elname = str.substr(range.location, range.length);
                
                while (1)
                {
                    std::string attrName;
                    std::string attrValue;
                    while (str[j] == ' ') // skip white space
                    {
                        ++j;
                    }
                    if (str[j] == '/')
                    {
                        break;
                    }
                    if (str[j] == '>')
                    {
                        break;
                    }
                    range.location = j;
                    while (str[j] != '=')
                    {
                        ++j;
                    }
                    range.length = j - range.location;
                    attrName = str.substr(range.location, range.length);
                    attrName = StringUtil::Trim(attrName);
                    ++j;
                    while (str[j] == ' ')
                    {
                        ++j;
                    }
                    range.location = j;
                    size_t startChar = 0;
                    if (str[j] == '\"' || str[j] == '\'')
                    {
                        startChar = j;
                    }
                    while (str[j] != ' ' && str[j] != '>')
                    {
                        if (startChar)
                        {
                            if (startChar < j && str[j] == str[startChar])
                            {
                                ++j;
                                break;
                            }
                        }
                        else if (str[j] == '/')
                        {
                            break;
                        }
                        ++j;
                    }
                    range.length = j - range.location;
                    attrValue = str.substr(range.location, range.length);
                    attrValue = StringUtil::Trim(attrValue);
                    if (attrValue[0] == '\"' || attrValue[0] == '\'')
                    {
                        range.location = 1;
                        range.length = attrValue.length() - 2;
                        attrValue = attrValue.substr(range.location, range.length);
                    }
                    attrDic.insert(make_pair(attrName, attrValue));
                }
                delegate->parserDidStartElement(this, elname, attrDic);
                if (str[j] == '/')
                {
                    while (str[j] != '>')
                    {
                        ++j;
                    }
                    delegate->parserDidEndElement(this, elname);
                }
                i = j + 1;
                continue;
            }
            
        }
        else // character
        {
            size_t j = i;
            while (str[j] != '<')
            {
                ++j;
            }
            Range range;
            range.location = i;
            range.length = j - i;
            delegate->parserFoundCharacters(str.substr(range.location, range.length));
            i = j;
        }
    }
    delegate->parserDidEndDocument(this);
}

NS_HL_END
