//
//  HLSpriteFrame.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-12.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLSpriteFrame.h"
#include "HLEntitySystem.h"
#include "HLSpriteComponent.h"
#include "HLFileUtil.h"
#include "rapidjson/document.h"
#include "HLTexture.h"
#include "GameEngineConfig.h"
#if USE_JS_SCRIPTING
#include "JSSpriteFrame.h"
#endif

NS_HL_BEGIN

std::map<std::string, HLDictionary*> HLSpriteFrame::spriteframemap;

HLSpriteFrame::~HLSpriteFrame()
{
    if (_texture)
        _texture->release();
#if USE_JS_SCRIPTING
    JSSpriteFrame::removeJSObject(this);
#endif
}

HLArray* HLSpriteFrame::getSpriteFrameArray(const char* file)
{
    HLASSERT(file, "file must no NULL");
    
//    char fullPath[MAX_PATH];
//    get_full_path(file, fullPath);
    
    HLArray *res = new HLArray();
    res->autorelease();
    
    size_t len = strlen(file);
    if (!strcmp(file+(len-4), ".bsf"))
    {
        HLFileData *data = HLResourceManager::getSingleton()->getFileData(file);
        if (!data)
        {
            HLLOG("file (%s) does not exist\n", file);
            return NULL;
        }
        
        int index = 5 + data->buffer[4];
        char tmp[50];
        
        HLDictionary *dict = getSpriteFrameDictionary(file);
        while (index < data->size)
        {
            unsigned char len = data->buffer[index];
            ++index;
            memcpy(tmp, data->buffer+index, len);
            tmp[len] = 0;
            res->addObject(dict->objectForKey(std::string(tmp)));
            index += len + 17;
        }
        delete data;
    }
    else
    {
        HLString *string = HLString::stringWithContentsOfFile(file);
		rapidjson::Document d;
		d.Parse(string->_value.c_str());
        
		rapidjson::Value& frames = d["frames"];
        
        HLDictionary *dict = getSpriteFrameDictionary(file);
        
		for (rapidjson::Value::ConstValueIterator itr = frames.Begin(); itr != frames.End(); ++itr)
		{
			std::string name = (*itr)["filename"].GetString();
			res->addObject(dict->objectForKey(name));
		}
    }
    return res;
}

HLDictionary* HLSpriteFrame::getSpriteFrameDictionary(const char* file)
{
    std::map<std::string, HLDictionary*>::iterator itr = spriteframemap.find(std::string(file));
    if (itr != spriteframemap.end())
    {
        return itr->second;
    }
    else
    {
//        char fullPath[MAX_PATH];
//        get_full_path(file, fullPath);
        
        HLDictionary *res = new HLDictionary();
        
        size_t len = strlen(file);
        if (!strcmp(file+(len-4), ".bsf"))
        {
            HLFileData *data = HLResourceManager::getSingleton()->getFileData(file);
            if (!data)
            {
                HLLOG("file (%s) does not exist\n", file);
                res->release();
                return NULL;
            }
            char tmp[MAX_PATH];
            get_file_path(file, tmp);
            unsigned char len1 = data->buffer[4];
            len = strlen(tmp);
            memcpy(tmp+len, data->buffer+5, len1);
            tmp[len+len1] = 0;
            HLTexture *texture = HLTexture::getTexture(tmp);
            if (!texture)
            {
                return NULL;
            }
            size_t index = 5+len1;
            
            while (index < data->size)
            {
                HLSpriteFrame *frame = new HLSpriteFrame();
                frame->autorelease();
                frame->_texture = texture;
                texture->retain();
                len1 = data->buffer[index];
                ++index;
                memcpy(tmp, data->buffer+index, len1);
                tmp[len1] = 0;
                frame->_name = tmp;
                index += len1;
                uint16_t num;
                num = *(uint16_t*)(data->buffer+index);
                LittleEndianToNative<2>(&num);
                frame->_textureRect.origin.x = num;
                index += 2;
                num = *(uint16_t*)(data->buffer+index);
                LittleEndianToNative<2>(&num);
                frame->_textureRect.origin.y = num;
                index += 2;
                num = *(uint16_t*)(data->buffer+index);
                LittleEndianToNative<2>(&num);
                frame->_textureRect.size.width = num;
                index += 2;
                num = *(uint16_t*)(data->buffer+index);
                LittleEndianToNative<2>(&num);
                frame->_textureRect.size.height = num;
                index += 2;
                num = *(uint16_t*)(data->buffer+index);
                LittleEndianToNative<2>(&num);
                frame->_offset.x = num;
                index += 2;
                num = *(uint16_t*)(data->buffer+index);
                LittleEndianToNative<2>(&num);
                frame->_offset.y = num;
                index += 2;
                num = *(uint16_t*)(data->buffer+index);
                LittleEndianToNative<2>(&num);
                frame->_sourceSize.width = num;
                index += 2;
                num = *(uint16_t*)(data->buffer+index);
                LittleEndianToNative<2>(&num);
                frame->_sourceSize.height = num;
                index += 2;
                frame->_rotated = data->buffer[index];
                ++index;
                res->setObjectForKey(frame->_name, frame);
            }
            delete data;
        }
        else
        {
            HLString *string = HLString::stringWithContentsOfFile(file);
            if (string == NULL) {
                res->release();
                return NULL;
            }
			rapidjson::Document d;
			d.Parse(string->_value.c_str());
            
            char path[MAX_PATH];
            get_file_path(file, path);
            strcat(path, d["meta"]["image"].GetString());
                    
            HLTexture *texture = HLTexture::getTexture(path);
            if (!texture)
            {
                return NULL;
            }
            
			rapidjson::Value& frames = d["frames"];

			for (rapidjson::Value::ConstValueIterator itr = frames.Begin(); itr != frames.End(); ++itr)
			{
				const rapidjson::Value& val = *itr;
				HLSpriteFrame *frame = new HLSpriteFrame();
				frame->autorelease();
				frame->_name = val["filename"].GetString();
				frame->_texture = texture;
				texture->retain();
				const rapidjson::Value& val1 = val["frame"];
				frame->_textureRect = HLRect(val1["x"].GetInt(), val1["y"].GetInt(), 
											val1["w"].GetInt(), val1["h"].GetInt());
				const rapidjson::Value& val2 = val["spriteSourceSize"];
				frame->_offset = HLPoint(val2["x"].GetInt(), val2["y"].GetInt());
				frame->_rotated = val["rotated"].GetBool();
				const rapidjson::Value& val3 = val["sourceSize"];
				frame->_sourceSize = HLSize(val3["w"].GetInt(), val3["h"].GetInt());
				res->setObjectForKey(frame->_name, frame);
			}
            
        }
        spriteframemap.insert(std::make_pair(std::string(file), res));
        
        return res;
    }
}

HLSpriteFrame* HLSpriteFrame::getSpriteFrame(const char* file, const char* name)
{
    HLDictionary* dict = getSpriteFrameDictionary(file);
    if (!dict)
    {
        return NULL;
    }
    return (HLSpriteFrame*)dict->objectForKey(name);
}

void HLSpriteFrame::purgeCachedData(const char* file)
{
    if (file)
    {
        std::map<std::string, HLDictionary*>::iterator itr = spriteframemap.find(std::string(file));
        if (itr != spriteframemap.end())
        {
            itr->second->release();
            spriteframemap.erase(itr);
        }
        return;
    }
    std::map<std::string, HLDictionary*>::iterator itr;
    std::vector<std::string> erases;
    for (itr = spriteframemap.begin(); itr != spriteframemap.end(); ++itr)
    {
        bool shouldpurge = true;
        for (auto itr1 = itr->second->_value.begin(); itr1 != itr->second->_value.end(); ++itr1)
        {
            if (itr1->second->retainCount() > 1) {
                shouldpurge = false;
                break;
            }
        }
        if (shouldpurge)
        {
            erases.push_back(itr->first);
            itr->second->release();
        }
    }
    for (auto i = erases.begin(); i != erases.end(); ++i)
    {
        spriteframemap.erase(*i);
    }
}

void HLSpriteFrame::setDisplayFrameForEntity(HLEntity* entity)
{
    HLASSERT(entity->hasComponent<HLSpriteComponent>(), "entity should have sprite component");
//    if (entity->hasComponent<HLSpriteComponent>()) 
//    {
        entity->setProperty<HLTexture*>("texture", _texture);
        entity->setProperty("textureRect", _textureRect);
        entity->setProperty("offsetPosFromBL", HLPoint(_offset.x, _sourceSize.height-_textureRect.size.height-_offset.y));
        entity->setProperty("untrimmedSize", _sourceSize);
        entity->setProperty("textureRotated", _rotated);
//    }
}
HLTexture* HLSpriteFrame::getTexture()
{
    return _texture;
}
NS_HL_END
