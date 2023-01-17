//
//  HLBMFont.cpp
//  HoolaiEngine
//
//  Created by tyq on 12-8-28.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#include "HLBMFont.h"
#include <iostream>
#include <string>
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "HLTexture.h"
#include "HLFileUtil.h"
#include "HLProgram.h"
#include "HLGLConfiguration.h"

using namespace std;

NS_HL_BEGIN


//Unicode<<-change->>UTF8 caiwei--2013-04-26 begin
/* convert unicode to UTF-8 */
#define UNICODE_TO_UTF8(unicode, e)                                             \
do                                                                              \
{                                                                               \
    if(e && unicode != 0)                                                       \
    {                                                                           \
        if(unicode < 0x80) *e++ = unicode;                                      \
        else if(unicode < 0x800)                                                \
        {                                                                       \
            /*<11011111> < 000 0000 0000>*/                                     \
            *e++ = ((unicode >> 6) & 0x1f)|0xc0;                                \
            *e++ = (unicode & 0x3f)|0x80;                                       \
        }                                                                       \
        else if(unicode < 0x10000)                                              \
        {                                                                       \
            /*<11101111> <0000 0000 0000 0000>*/                                \
            *e++ = ((unicode >> 12) & 0x0f)|0xe0;                               \
            *e++ = ((unicode >> 6) & 0x3f)|0x80;                                \
            *e++ = (unicode & 0x3f)|0x80;                                       \
        }                                                                       \
        else if(unicode < 0x200000)                                             \
        {                                                                       \
            /*<11110111> <0 0000 0000 0000 0000 0000>*/                         \
            *e++ = ((unicode >> 18) & 0x07)|0xf0;                               \
            *e++ = ((unicode >> 12) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 6) & 0x3f)|0x80;                                \
            *e++ = (unicode & 0x3f)|0x80;                                       \
        }                                                                       \
        else if(unicode < 0x4000000)                                            \
        {                                                                       \
            /*<11111011> <00 0000 0000 0000 0000 0000 0000>*/                   \
            *e++ = ((unicode >> 24) & 0x03)|0xf8 ;                              \
            *e++ = ((unicode >> 18) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 12) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 6) & 0x3f)|0x80;                                \
            *e++ = (unicode & 0x3f)|0x80;                                       \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            /*<11111101> <0000 0000 0000 0000 0000 0000 0000 0000>*/            \
            *e++ = ((unicode >> 30) & 0x01)|0xfc;                               \
            *e++ = ((unicode >> 24) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 18) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 12) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 6) & 0x3f)|0x80;                                \
            *e++ = (unicode & 0x3f)|0x80;                                       \
        }                                                                       \
    }                                                                           \
}while(0)

/* convert UTF-8 to unicode */
#define UTF8_TO_UNICODE(p, e, n)                                                \
do                                                                              \
{                                                                               \
    if(p)                                                                       \
    {                                                                           \
        if(*p >= 0xfc)                                                          \
        {                                                                       \
            /*6:<11111100>*/                                                    \
            e = (p[0] & 0x01) << 30;                                            \
            e |= (p[1] & 0x3f) << 24;                                           \
            e |= (p[2] & 0x3f) << 18;                                           \
            e |= (p[3] & 0x3f) << 12;                                           \
            e |= (p[4] & 0x3f) << 6;                                            \
            e |= (p[5] & 0x3f);                                                 \
            n = 6;                                                              \
        }                                                                       \
        else if(*p >= 0xf8)                                                     \
        {                                                                       \
            /*5:<11111000>*/                                                    \
            e = (p[0] & 0x03) << 24;                                            \
            e |= (p[1] & 0x3f) << 18;                                           \
            e |= (p[2] & 0x3f) << 12;                                           \
            e |= (p[3] & 0x3f) << 6;                                            \
            e |= (p[4] & 0x3f);                                                 \
            n = 5;                                                              \
        }                                                                       \
        else if(*p >= 0xf0)                                                     \
        {                                                                       \
            /*4:<11110000>*/                                                    \
            e = (p[0] & 0x07) << 18;                                            \
            e |= (p[1] & 0x3f) << 12;                                           \
            e |= (p[2] & 0x3f) << 6;                                            \
            e |= (p[3] & 0x3f);                                                 \
            n = 4;                                                              \
        }                                                                       \
        else if(*p >= 0xe0)                                                     \
        {                                                                       \
            /*3:<11100000>*/                                                    \
            e = (p[0] & 0x0f) << 12;                                            \
            e |= (p[1] & 0x3f) << 6;                                            \
            e |= (p[2] & 0x3f);                                                 \
            n = 3;                                                              \
        }                                                                       \
        else if(*p >= 0xc0)                                                     \
        {                                                                       \
            /*2:<11000000>*/                                                    \
            e = (p[0] & 0x1f) << 6;                                             \
            e |= (p[1] & 0x3f);                                                 \
            n = 2;                                                              \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            e = p[0];                                                           \
            n = 1;                                                              \
        }                                                                       \
    }                                                                           \
}while(0)

/* convert unicode to UTF-8 */
unsigned char *UnicodetoUTF8(int unicode, unsigned char *p);
unsigned char *UnicodetoUTF8(int unicode, unsigned char *p)
{
    unsigned char *e = NULL;

    if((e = p))
    {
        if(unicode < 0x80) *e++ = unicode;
        else if(unicode < 0x800)
        {
            /*<11011111> < 000 0000 0000>*/
            *e++ = ((unicode >> 6) & 0x1f)|0xc0;
            *e++ = (unicode & 0x3f)|0x80;
        }
        else if(unicode < 0x10000)
        {
            /*<11101111> <0000 0000 0000 0000>*/
            *e++ = ((unicode >> 12) & 0x0f)|0xe0;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80;
        }
        else if(unicode < 0x200000)
        {
            /*<11110111> <0 0000 0000 0000 0000 0000>*/
            *e++ = ((unicode >> 18) & 0x07)|0xf0;
            *e++ = ((unicode >> 12) & 0x3f)|0x80;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80;
        }
        else if(unicode < 0x4000000)
        {
            /*<11111011> <00 0000 0000 0000 0000 0000 0000>*/
            *e++ = ((unicode >> 24) & 0x03)|0xf8 ;
            *e++ = ((unicode >> 18) & 0x3f)|0x80;
            *e++ = ((unicode >> 12) & 0x3f)|0x80;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80;
        }
        else
        {
            /*<11111101> <0000 0000 0000 0000 0000 0000 0000 0000>*/
            *e++ = ((unicode >> 30) & 0x01)|0xfc;
            *e++ = ((unicode >> 24) & 0x3f)|0x80;
            *e++ = ((unicode >> 18) & 0x3f)|0x80;
            *e++ = ((unicode >> 12) & 0x3f)|0x80;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80;
        }
    }
    return e;
}

/* convert UTF-8 to unicode */
int UTF8toUnicode(unsigned char *ch, int *unicode);
int UTF8toUnicode(unsigned char *ch, int *unicode)
{
    unsigned char *p = NULL;
    int e = 0, n = 0;

    if((p = ch) && unicode)
    {
        if(*p >= 0xfc)
        {
            /*6:<11111100>*/
            e = (p[0] & 0x01) << 30;
            e |= (p[1] & 0x3f) << 24;
            e |= (p[2] & 0x3f) << 18;
            e |= (p[3] & 0x3f) << 12;
            e |= (p[4] & 0x3f) << 6;
            e |= (p[5] & 0x3f);
            n = 6;
        }
        else if(*p >= 0xf8)
        {
            /*5:<11111000>*/
            e = (p[0] & 0x03) << 24;
            e |= (p[1] & 0x3f) << 18;
            e |= (p[2] & 0x3f) << 12;
            e |= (p[3] & 0x3f) << 6;
            e |= (p[4] & 0x3f);
            n = 5;
        }
        else if(*p >= 0xf0)
        {
            /*4:<11110000>*/
            e = (p[0] & 0x07) << 18;
            e |= (p[1] & 0x3f) << 12;
            e |= (p[2] & 0x3f) << 6;
            e |= (p[3] & 0x3f);
            n = 4;
        }
        else if(*p >= 0xe0)
        {
            /*3:<11100000>*/
            e = (p[0] & 0x0f) << 12;
            e |= (p[1] & 0x3f) << 6;
            e |= (p[2] & 0x3f);
            n = 3;
        }
        else if(*p >= 0xc0)
        {
            /*2:<11000000>*/
            e = (p[0] & 0x1f) << 6;
            e |= (p[1] & 0x3f);
            n = 2;
        }
        else
        {
            e = p[0];
            n = 1;
        }
        *unicode = e;
    }
    return n;
}
//Unicode<<-change->>UTF8 caiwei--2013-04-26 end

#define UTF8_COMPUTE(Char, Mask, Len)        \
if (Char < 128)                \
{                        \
Len = 1;                    \
Mask = 0x7f;                \
}                        \
else if ((Char & 0xe0) == 0xc0)        \
{                        \
Len = 2;                    \
Mask = 0x1f;                \
}                        \
else if ((Char & 0xf0) == 0xe0)        \
{                        \
Len = 3;                    \
Mask = 0x0f;                \
}                        \
else if ((Char & 0xf8) == 0xf0)        \
{                        \
Len = 4;                    \
Mask = 0x07;                \
}                        \
else if ((Char & 0xfc) == 0xf8)        \
{                        \
Len = 5;                    \
Mask = 0x03;                \
}                        \
else if ((Char & 0xfe) == 0xfc)        \
{                        \
Len = 6;                    \
Mask = 0x01;                \
}                        \
else                        \
Len = -1;

#define UTF8_GET(Result, Chars, Count, Mask, Len)    \
(Result) = (Chars)[0] & (Mask);            \
for ((Count) = 1; (Count) < (Len); ++(Count))        \
{                            \
if (((Chars)[(Count)] & 0xc0) != 0x80)        \
{                        \
(Result) = -1;                \
break;                    \
}                        \
(Result) <<= 6;                    \
(Result) |= ((Chars)[(Count)] & 0x3f);        \
}

const char utf8_skip_data[256] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5,
    5, 5, 5, 6, 6, 1, 1
};

const char *const g_utf8_skip = utf8_skip_data;

#define utf8_next_char(p) (char *)((p) + g_utf8_skip[*(unsigned char *)(p)])

/*
 * g_utf8_strlen:
 * @p: pointer to the start of a UTF-8 encoded string.
 * @max: the maximum number of bytes to examine. If @max
 *       is less than 0, then the string is assumed to be
 *       null-terminated. If @max is 0, @p will not be examined and
 *       may be %NULL.
 *
 * Returns the length of the string in characters.
 *
 * Return value: the length of the string in characters
 **/
int
utf8_strlen (const char * p, int max)
{
    int len = 0;
    const char *start = p;

    if (!(p != NULL || max == 0))
    {
        return 0;
    }

    if (max < 0)
    {
        while (*p)
        {
            p = utf8_next_char (p);
            ++len;
        }
    }
    else
    {
        if (max == 0 || !*p)
            return 0;

        p = utf8_next_char (p);

        while (p - start < max && *p)
        {
            ++len;
            p = utf8_next_char (p);
        }

        /* only do the last len increment if we got a complete
         * char (don't count partial chars)
         */
        if (p - start == max)
            ++len;
    }

    return len;
}

/* Author:caiwei
 * g_utf8_strlen2:
 * @p: pointer to the start of a UTF-8 encoded string.
 * @max: the maximum number of bytes to examine. If @max
 *       is less than 0, then the string is assumed to be
 *       null-terminated. If @max is 0, @p will not be examined and
 *       may be %NULL.
 *
 * Returns the length of the string in characters.
 *
 * Return value: the length of the string in characters--(a Chinese char = 2 length)
 **/
int utf8_strlen2 (const char * p)
{
    int len = 0;
    char* m_p = (char*)p;
    
    if (p == NULL)
    {
        return 0;
    }
    while (*p)
    {
        m_p = (char*)p;
        p = utf8_next_char (p);
        ++len;
        if(p - m_p > 1)
        {
            ++len;
        }
    }
    
    return len;
}


/*
 * g_utf8_get_char:
 * @p: a pointer to Unicode character encoded as UTF-8
 *
 * Converts a sequence of bytes encoded as UTF-8 to a Unicode character.
 * If @p does not point to a valid UTF-8 encoded character, results are
 * undefined. If you are not sure that the bytes are complete
 * valid Unicode characters, you should use g_utf8_get_char_validated()
 * instead.
 *
 * Return value: the resulting character
 **/
unsigned int
utf8_get_char (const char * p)
{
    int i, mask = 0, len;
    unsigned int result;
    unsigned char c = (unsigned char) *p;

    UTF8_COMPUTE (c, mask, len);
    if (len == -1)
        return (unsigned int) - 1;
    UTF8_GET (result, p, i, mask, len);

    return result;
}

/*
 * utf16_from_utf8:
 * @str_old: pointer to the start of a C string.
 *
 * Creates a utf8 string from a cstring.
 *
 * Return value: the newly created utf8 string.
 * */
uint16_t* utf16_from_utf8(const char* str_old)
{
    int len = utf8_strlen(str_old, -1);

    uint16_t* str_new = new uint16_t[len + 1];
    str_new[len] = 0;

    for (int i = 0; i < len; ++i)
    {
        str_new[i] = utf8_get_char(str_old);
        str_old = utf8_next_char(str_old);
    }

    return str_new;
}

int wcslen(const unsigned short* str)
{
    int i=0;
    while(*str++) i++;
    return i;
}

std::map<std::string, HLBMFont*> HLBMFont::fontCache;

bool HLBMFont::parseFontFile(const char *fntFile)
{
    _scale = get_file_scale(fntFile);
    char fullPath[MAX_PATH];
    get_full_path(fntFile, fullPath);

    HLFileData filedata(fullPath, "r");

//    char *contents = (char *)filedata.buffer;
//    char *position = contents;
    char *line = strtok( ( char * )filedata.buffer, "\n" );;

    while (line)
    {
//        char * pos = strchr(position, '\n');
//        if (!pos)
//        {
//            break;
//        }
//        int len = pos - position;
//        if (len > 256)
//        {
//            line = (char *)realloc(line, len + 1);
//        }
//        memcpy(line, position, len);
//        line[len] = '\0';
//        position = pos + 1;

        if (!strncmp(line, "info face", 9))
        {
            parseInfoArguments(line);
        }
        else if (!strncmp(line, "common lineHeight", 17))
        {
            parseCommonArguments(line);
        }
        else if (!strncmp(line, "page id", 7))
        {
            parseImageFileName(line, fntFile);
        }
        else if (!strncmp(line, "chars c", 7))
        {

        }
        else if (!strncmp(line, "char", 4))
        {
            BMFontDef fontDef;
            parseCharacterDefinition(line, &fontDef);

            _chars.insert(make_pair(fontDef.charId, fontDef));
        }
        else if (!strncmp(line, "kerning first", 13))
        {
            parseKerningEntry(line);
        }

        line = strtok( NULL, "\n" );
    }

    return true;
}

void HLBMFont::parseCharacterDefinition(const char *line, BMFontDef * charDef)
{
    char leftLine[100];
    sscanf(line, "char id=%u x=%f y=%f width=%f height=%f xoffset=%hd yoffset=%hd xadvance=%hd %s", &charDef->charId, &charDef->rect.origin.x, &charDef->rect.origin.y, &charDef->rect.size.width, &charDef->rect.size.height, &charDef->xOffset, &charDef->yOffset, &charDef->xAdvance, leftLine);
}

void HLBMFont::parseInfoArguments(const char *line)
{
    const char * pos1 = strstr(line, "padding=");
    const char * pos2 = strchr(pos1, ' ');
    int len = pos2 - pos1;
    char * value = (char *)malloc(len + 1);
    strncpy(value, pos1, len);
    sscanf(value, "padding=%d,%d,%d,%d", &_padding.top, &_padding.right, &_padding.bottom, &_padding.left);
    free(value);
}

void HLBMFont::parseCommonArguments(const char *line)
{
    short packed;
    sscanf(line, "common lineHeight=%hd base=%hd scaleW=%hd scaleH=%hd pages=%hd packed=%hd", &_lineHeight, &_base, &_width, &_height, &_pages, &packed);

    HLASSERT(_pages == 1, "HLBMFont: only supports 1 page currently");
    HLASSERT(_width < HLGLConfiguration::getSingleton()->getMaxTextureSize(), "HLBMFont: page can't be larger than supported");
    HLASSERT(_height < HLGLConfiguration::getSingleton()->getMaxTextureSize(), "HLBMFont: page can't be larger than supported");
}

void HLBMFont::parseImageFileName(const char *line, const char * fntFile)
{
    const char * pos = strstr(line, "file=");
    char * fileName = (char *)malloc(strlen(pos));
    int i = 0;
    bool f = false;
    for (; *pos != '\0'; pos++)
    {
        if (*pos == '"')
        {
            if (f)
            {
                fileName[i] = '\0';
                break;
            }
            else
            {
                f = true;
            }
        }
        else
        {
            if (f)
            {
                fileName[i++] = *pos;
            }
        }
    }
    char relPath[MAX_PATH] = {0};
    get_file_path(fntFile, relPath);
    strcat(relPath, fileName);
    _texture = HLTexture::getTexture(relPath);
    _texture->retain();
    
    free(fileName);
}

void HLBMFont::parseKerningEntry(const char *line)
{
    KerningInfo kerning;
    sscanf(line, "kerning first=%hd second=%hd amount=%hd", &kerning.first, &kerning.second, &kerning.amount);
    unsigned int first = kerning.first;
    unsigned int second = kerning.second;
    _kerningInfo.insert(make_pair((first<<16) | (second&0xffff), kerning));
}

int HLBMFont::getKerningPair(int first, int second)
{
//    if (_kernCount)
//    {
//        for (int j = 0; j < _kernCount; j++)
//        {
//            if (_kerningInfo[j].first == first && _kerningInfo[j].second == second)
//            {
//                return _kerningInfo[j].amount;
//            }
//        }
//    }
    map<unsigned int, KerningInfo>::iterator itr = _kerningInfo.find((first<<16) | (second&0xffff));
    if (itr != _kerningInfo.end())
    {
        return itr->second.amount;
    }
    return 0;
}

float HLBMFont::getStringWidth(const char *str)
{
    float total = 0;
    BMFontDef *f;

    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        f = &_chars[str[i]];
        total += f->xAdvance;
    }

    return total;
}

HLBMFont::~HLBMFont()
{
    if (_texture)
    {
        _texture->release();
    }
}

typedef struct
{
    float x,y;
    float texx,texy;
    GLubyte r,g,b,a;
} vlist;

vlist texlst[1024*3]; // 512 characters

void Render_String(int len)
{
    //Draw Text Array, with 2D, two coordinates per vertex.
    glEnableVertexAttribArray(kVertexAttrib_Position);
    glEnableVertexAttribArray(kVertexAttrib_Color);
    glEnableVertexAttribArray(kVertexAttrib_TexCoords);

    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(vlist), &texlst[0].x);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(vlist), &texlst[0].texx);
    glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vlist), &texlst[0].r);

    glDrawArrays(GL_TRIANGLES, 0, len*6);
}

float HLBMFont::widthForText(const char *fmt,...)
{
    char text[512];

    va_list ap;
    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

    unsigned short* str = utf16_from_utf8(text);

    int nextFontPositionX = 0;
    int nextFontPositionY = 0;
    unsigned short prev = -1;
    int kerningAmount = 0;

    int longestLine = 0;
    unsigned int totalHeight = 0;

    float width = 0;

    unsigned int quantityOfLines = 1;

    unsigned int stringLen = wcslen(str);
    if (stringLen == 0)
    {
        return 0;
    }

    for (unsigned int i = 0; i < stringLen - 1; ++i)
    {
        unsigned short c = str[i];
        if (c == '\n')
        {
            quantityOfLines++;
        }
    }

    totalHeight = _lineHeight * quantityOfLines;
    nextFontPositionY += 0-(_lineHeight - _lineHeight * quantityOfLines);
#ifdef TARGET_IOS
    float fscale = _scale / CONTENT_SCALE_FACTOR;
#else
    float fscale = _scale * HLDirector2D::getSingleton()->getWinSizeScale();
#endif

    float advh = 1.0f/_width;
    float advv = 1.0f/_height;

    for (unsigned int i= 0; i < stringLen; i++)
    {
        unsigned short c = str[i];

        if (c == '\n')
        {
            nextFontPositionX = 0;
            nextFontPositionY -= _lineHeight;
            continue;
        }
        unsigned int key = c;
        BMFontDef fontDef = _chars.find(key)->second;

        HLRect rect = fontDef.rect;

        int yOffset = _lineHeight - fontDef.yOffset;
        HLPoint fontPos((float)nextFontPositionX + fontDef.xOffset,  (float)nextFontPositionY + yOffset - rect.size.height);

        // update kerning
        if (stringLen > 1 && i < stringLen && str[i+1] != '\n')
        {
            kerningAmount = getKerningPair(str[i], str[i+1]);
        }
        nextFontPositionX += fontDef.xAdvance + kerningAmount;
        width = MAX(width, nextFontPositionX);
        prev = c;

        if (longestLine < nextFontPositionX)
        {
            longestLine = nextFontPositionX;
        }
    }

    delete [] str;

    return width;
}

void HLBMFont::print(float x, float y, const char *fmt,...)
{
    char text[512];

    va_list ap;
    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

    unsigned short* str = utf16_from_utf8(text);

    int nextFontPositionX = x;
    int nextFontPositionY = y;
    unsigned short prev = -1;
    int kerningAmount = 0;

    int longestLine = 0;
    unsigned int totalHeight = 0;

    unsigned int quantityOfLines = 1;

    unsigned int stringLen = wcslen(str);
    if (stringLen == 0)
    {
        return;
    }

    for (unsigned int i = 0; i < stringLen - 1; ++i)
    {
        unsigned short c = str[i];
        if (c == '\n')
        {
            quantityOfLines++;
        }
    }

    totalHeight = _lineHeight * quantityOfLines;
    nextFontPositionY += 0-(_lineHeight - _lineHeight * quantityOfLines);
//#if GAME_PROJECTION == PROJECTION_2D
//    float fscale = _scale * HLDirector2D::sharedDirector()->getWinSizeScale();
//#else
//    float fscale = _scale * HLDirector3D::sharedDirector()->getWinSizeScale();
//#endif
#ifdef TARGET_IOS
    float fscale = _scale / CONTENT_SCALE_FACTOR;
#else
    float fscale = _scale * HLDirector2D::getSingleton()->getWinSizeScale();
#endif

    float advh = 1.0f/_width;
    float advv = 1.0f/_height;

    for (unsigned int i= 0; i < stringLen; i++)
    {
        unsigned short c = str[i];

        if (c == '\n')
        {
            nextFontPositionX = 0;
            nextFontPositionY -= _lineHeight;
            continue;
        }

        unsigned int key = c;
        BMFontDef fontDef = _chars.find(key)->second;

        HLRect rect = fontDef.rect;

        int yOffset = _lineHeight - fontDef.yOffset;
        HLPoint fontPos((float)nextFontPositionX + fontDef.xOffset,  (float)nextFontPositionY + yOffset - rect.size.height);

        // tl
        texlst[i*6].x = fontPos.x * fscale;
        texlst[i*6].y = (fontPos.y + rect.size.height) * fscale;
        texlst[i*6].texx = advh * rect.origin.x;
        texlst[i*6].texy = advv * rect.origin.y;
        texlst[i*6].r = _color.r;
        texlst[i*6].g = _color.g;
        texlst[i*6].b = _color.b;
        texlst[i*6].a = _color.a;

        // bl
        texlst[i*6+1].x = fontPos.x * fscale;
        texlst[i*6+1].y = fontPos.y * fscale;
        texlst[i*6+1].texx = advh * rect.origin.x;
        texlst[i*6+1].texy = advv * (rect.origin.y + rect.size.height);
        texlst[i*6+1].r = _color.r;
        texlst[i*6+1].g = _color.g;
        texlst[i*6+1].b = _color.b;
        texlst[i*6+1].a = _color.a;

        // tr
        texlst[i*6+2].x = (fontPos.x + rect.size.width) * fscale;
        texlst[i*6+2].y = (fontPos.y + rect.size.height) * fscale;
        texlst[i*6+2].texx = advh * (rect.origin.x + rect.size.width);
        texlst[i*6+2].texy = advv * rect.origin.y;
        texlst[i*6+2].r = _color.r;
        texlst[i*6+2].g = _color.g;
        texlst[i*6+2].b = _color.b;
        texlst[i*6+2].a = _color.a;

        // br
        texlst[i*6+3].x = (fontPos.x + rect.size.width) * fscale;
        texlst[i*6+3].y = fontPos.y * fscale;
        texlst[i*6+3].texx = advh * (rect.origin.x + rect.size.width);
        texlst[i*6+3].texy = advv * (rect.origin.y + rect.size.height);
        texlst[i*6+3].r = _color.r;
        texlst[i*6+3].g = _color.g;
        texlst[i*6+3].b = _color.b;
        texlst[i*6+3].a = _color.a;

        // tr
        texlst[i*6+4].x = (fontPos.x + rect.size.width) * fscale;
        texlst[i*6+4].y = (fontPos.y + rect.size.height) * fscale;
        texlst[i*6+4].texx = advh * (rect.origin.x + rect.size.width);
        texlst[i*6+4].texy = advv * rect.origin.y;
        texlst[i*6+4].r = _color.r;
        texlst[i*6+4].g = _color.g;
        texlst[i*6+4].b = _color.b;
        texlst[i*6+4].a = _color.a;

        // bl
        texlst[i*6+5].x = fontPos.x * fscale;
        texlst[i*6+5].y = fontPos.y * fscale;
        texlst[i*6+5].texx = advh * rect.origin.x;
        texlst[i*6+5].texy = advv * (rect.origin.y + rect.size.height);
        texlst[i*6+5].r = _color.r;
        texlst[i*6+5].g = _color.g;
        texlst[i*6+5].b = _color.b;
        texlst[i*6+5].a = _color.a;

        // update kerning
        if (stringLen > 1 && i < stringLen && str[i+1] != '\n')
        {
            kerningAmount = getKerningPair(str[i], str[i+1]);
        }
        nextFontPositionX += fontDef.xAdvance + kerningAmount;
        prev = c;

        if (longestLine < nextFontPositionX)
        {
            longestLine = nextFontPositionX;
        }
    }

    delete [] str;

    hlglBindTexture(GL_TEXTURE_2D, _texture->getName());

    Render_String(stringLen);
}

NS_HL_END
