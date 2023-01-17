//
//  HLObject.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-5-4.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLOBJECT_H__
#define __HLOBJECT_H__

#include "HLMacros.h"
#include <string>
#include <vector>
#include <map>
#include <stdarg.h>
#include <stdlib.h>

#define kMaxStringLen (1024*100)

NS_HL_BEGIN

class HLObject;

typedef void (HLObject::*CallFuncO)(HLObject*);

struct ThreadScheduleObject
{
    HLObject *target;
    CallFuncO selector;
    HLObject *object;
    ThreadScheduleObject(HLObject *target, CallFuncO selector, HLObject *object)
    {
        this->target = target;
        this->selector = selector;
        this->object = object;
    }
};

typedef enum
{
    kHLObjectString,
    kHLObjectNumber,
    kHLObjectArray,
    kHLObjectDictionary,
    kHLObjectObject
} HLObjectType;

class HLObject
{
protected:
    unsigned int _refCount;
    HLObjectType _type;
public:
    HLObject() 
    {
        _refCount = 1;
        _type = kHLObjectObject;
    }
    virtual ~HLObject() 
    {
        
    }
    
    void retain()
    {
        _refCount++;
    }
    
    void release()
    {
        _refCount--;
        if (_refCount==0) 
        {
            delete this;
        }
    }
    
    void autorelease();
    
    unsigned int retainCount() 
    {
        return _refCount;
    }
    
    HLObjectType getType() {return _type;}
    
    void performFuncOnMainThread(CallFuncO func, HLObject * param);
};

class HLString : public HLObject 
{
public:
    std::string _value;
    
    HLString():_value("") 
    {
        _type = kHLObjectString;
    }
    
    HLString(std::string& str):_value(str) 
    {
        _type = kHLObjectString;
    }
    
    HLString(const char * str):_value(str) 
    {
        _type = kHLObjectString;
    }
    
    HLString(HLString& str):_value(str._value) 
    {
        _type = kHLObjectString;
    }
    
    HLString(unsigned char* pData, unsigned long len) 
    {
        _value = std::string((const char*)pData, len);
        _type = kHLObjectString;
    }
    
    HLString& operator= (const HLString& other)
    {
        _value = other._value;
        return *this;
    }
    
    static HLString* stringWithFormat(const char* format, ...) 
    {
        va_list ap;
        va_start(ap, format);
        char* pBuf = (char*)malloc(kMaxStringLen);
        if (pBuf != NULL)
        {
            vsnprintf(pBuf, kMaxStringLen, format, ap);
            std::string str = pBuf;
            free(pBuf);
            HLString *ret = new HLString(str);
            ret->autorelease();
            return ret;
        }
        va_end(ap);
        return NULL;
    }
    
    static HLString* stringWithData(unsigned char* data, unsigned long len)
    {
        HLString* ret = NULL;
        if (data&&len>0) 
        {
            char* pStr = (char*)malloc(len+1);
            if (pStr) 
            {
                pStr[len] = '\0';
                memcpy(pStr, data, len);
                ret = new HLString(pStr);
                ret->autorelease();
                free(pStr);
            }
        }
        return ret;
    }
    
    static HLString* stringWithContentsOfFile(const char* file);
    
    size_t length() const
    {
        return _value.length();
    }
    
    const char* cString() const
    {
        return _value.c_str();
    }
    
    int intValue() const
    {
        if (_value.length() == 0) {
            return 0;
        }
        return atoi(_value.c_str());
    }
    
    float floatValue() const
    {
        if (_value.length() == 0) {
            return 0.f;
        }
        return (float)atof(_value.c_str());
    }
    
    double doubleValue() const
    {
        if (_value.length() == 0) {
            return 0.f;
        }
        return atof(_value.c_str());
    }
    
    bool boolValue() const
    {
        if (_value.length() == 0) {
            return false;
        }
        if (0 == strcmp(_value.c_str(), "0") || 0 == strcmp(_value.c_str(), "false"))
        {
            return false;
        }
        return true;
    }
};

typedef enum 
{
    kHLNumberTypeBool,
    kHLNumberTypeInt,
    kHLNumberTypeDouble,
    kHLNumberTypeFloat,
    kHLNumberTypeLongLong
} HLNumberType;

class HLNumber:public HLObject 
{
public:
    HLNumberType _numtype;
    union {
        bool boolValue;
        int intValue;
        double doubleValue;
        float floatValue;
        long long longlongValue;
    } _value;
    
    HLNumber(int value)
    {
        _numtype = kHLNumberTypeInt;
        _value.intValue = value;
        _type = kHLObjectNumber;
    }
    
    HLNumber(double value) 
    {
        _numtype = kHLNumberTypeDouble;
        _value.doubleValue = value;
        _type = kHLObjectNumber;
    }
    
    HLNumber(float value)
    {
        _numtype = kHLNumberTypeFloat;
        _value.floatValue = value;
        _type = kHLObjectNumber;
    }
    
    HLNumber(long long value) 
    {
        _numtype = kHLNumberTypeLongLong;
        _value.longlongValue = value;
        _type = kHLObjectNumber;
    }
    
    HLNumber(bool value) 
    {
        _numtype = kHLNumberTypeBool;
        _value.boolValue = value;
        _type = kHLObjectNumber;
    }
    
    int intValue() const
    {
        switch (_numtype) {
            case kHLNumberTypeInt:
                return _value.intValue;
            case kHLNumberTypeDouble:
                return (int)_value.doubleValue;
            case kHLNumberTypeFloat:
                return (int)_value.floatValue;
            case kHLNumberTypeLongLong:
                return (int)_value.longlongValue;
            case kHLNumberTypeBool:
                return _value.boolValue?1:0;
            default:
                break;
        }
    }
    
    float floatValue() const
    {
        switch (_numtype) {
            case kHLNumberTypeInt:
                return (float)_value.intValue;
            case kHLNumberTypeDouble:
                return (float)_value.doubleValue;
            case kHLNumberTypeFloat:
                return _value.floatValue;
            case kHLNumberTypeLongLong:
                return (float)_value.longlongValue;
            case kHLNumberTypeBool:
                return _value.boolValue?1.:0.;
            default:
                break;
        }
    }
    
    double doubleValue() const
    {
        switch (_numtype) {
            case kHLNumberTypeInt:
                return (double)_value.intValue;
            case kHLNumberTypeDouble:
                return _value.doubleValue;
            case kHLNumberTypeFloat:
                return (double)_value.floatValue;
            case kHLNumberTypeLongLong:
                return (double)_value.longlongValue;
            case kHLNumberTypeBool:
                return _value.boolValue?1.f:0.f;
            default:
                break;
        }
    }
    
    long long longlongValue() const
    {
        switch (_numtype) {
            case kHLNumberTypeInt:
                return (long long)_value.intValue;
            case kHLNumberTypeDouble:
                return (long long)_value.doubleValue;
            case kHLNumberTypeFloat:
                return (long long)_value.floatValue;
            case kHLNumberTypeLongLong:
                return _value.longlongValue;
            case kHLNumberTypeBool:
                return _value.boolValue?1:0;
            default:
                break;
        }
    }
    
    bool boolValue() const
    {
        switch (_numtype) {
            case kHLNumberTypeInt:
                return (bool)_value.intValue;
            case kHLNumberTypeDouble:
                return (bool)_value.doubleValue;
            case kHLNumberTypeFloat:
                return (bool)_value.floatValue;
            case kHLNumberTypeLongLong:
                return (bool)_value.longlongValue;
            case kHLNumberTypeBool:
                return _value.boolValue;
            default:
                break;
        }
    }
    
    HLString* stringValue() const
    {
        switch (_numtype) {
            case kHLNumberTypeInt:
                return HLString::stringWithFormat("%d", _value.intValue);
            case kHLNumberTypeDouble:
                return HLString::stringWithFormat("%lf", _value.doubleValue);
            case kHLNumberTypeFloat:
                return HLString::stringWithFormat("%f", _value.floatValue);
            case kHLNumberTypeLongLong:
                return HLString::stringWithFormat("%ld", _value.longlongValue);
            case kHLNumberTypeBool:
                return HLString::stringWithFormat("%s", _value.boolValue?"true":"false");
            default:
                break;
        }
    }
};

class HLArray:public HLObject
{
public:
    std::vector<HLObject*> _value;
    
    HLArray()
    {
        _type = kHLObjectArray;
    }
    
    HLArray(int size)
    {
        _value.reserve(size);
        _type = kHLObjectArray;
    }
    
    ~HLArray()
    {
        removeAllObjects();
    }
    
    void addObject(HLObject * o) 
    {
        o->retain();
        _value.push_back(o);
    }
    
    void removeObject(HLObject *o) 
    {
        std::vector<HLObject *>::iterator itr;
        for (itr = _value.begin(); itr != _value.end(); itr++) 
        {
            if (*itr == o) 
            {
                _value.erase(itr);
                o->release();
                return;
            }
        }
    }
    
    void removeObjectAtIndex(int index) 
    {
        HLObject *o = _value[index];
        o->release();
        _value.erase(_value.begin() + index);
    }
    
    HLObject* objectAtIndex(int index) 
    {
        return _value[index];
    }
    
    void removeAllObjects() 
    {
        std::vector<HLObject *>::iterator itr;
        for (itr = _value.begin(); itr != _value.end(); itr++) 
        {
            (*itr)->release();
        }
        _value.clear();
    }
    
    size_t size()
    {
        return _value.size();
    }
};

class HLDictionary:public HLObject
{
public:
    std::map<std::string, HLObject*> _value;
    
    HLDictionary()
    {
        _type = kHLObjectDictionary;
    }
    
    ~HLDictionary()
    {
        removeAllObjects();
    }
    
    void removeAllObjects()
    {
        std::map<std::string, HLObject*>::iterator itr;
        for (itr = _value.begin(); itr != _value.end(); itr++) {
            (itr->second)->release();
        }
        _value.clear();
    }
    
    HLObject* objectForKey(std::string key)
    {
        std::map<std::string, HLObject*>::iterator itr = _value.find(key);
        if (itr != _value.end()) 
        {
            return itr->second;
        }
        return NULL;
    }
    
    void setObjectForKey(std::string key, HLObject *ob)
    {
        ob->retain();
        std::map<std::string, HLObject*>::iterator itr = _value.find(key);
        if (itr != _value.end()) 
        {
            itr->second->release();
            _value.erase(itr);
        }
        _value.insert(make_pair(key, ob));
    }
    
    size_t size()
    {
        return _value.size();
    }
};

NS_HL_END

#endif
