//
//  JSViewLoader.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSViewLoader.h"
#include "HLViewLoader.h"
#include "HLView.h"
#include "HLButton.h"
#include "HLResourceManager.h"

NS_HL_BEGIN

namespace gui
{
    void JSViewLoader::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
    {
        JSObject* obj = JS_NewObject(cx, NULL, NULL, NULL);
        JS::RootedValue val(cx, OBJECT_TO_JSVAL(obj));
        JS_SetProperty(cx, globalObj, name, val);
        
        JS_DefineFunction(cx, obj, "loadView", JSViewLoader::jsLoadView, 0, JSPROP_PERMANENT | JSPROP_READONLY);
    }
    
    JSBool JSViewLoader::jsLoadView(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc >= 2)
        {
            char* file;
            JSObject* jsobj;
            HLView* parent = NULL;
            JSString* s;
            if (argc >= 3)
            {
                JSObject* jsobj1;
                JS_ConvertArguments(cx, 3, JS_ARGV(cx, vp), "Soo", &s, &jsobj, &jsobj1);
                JSCppWrapperBase* t = (JSCppWrapperBase*)JS_GetPrivate(jsobj1);
                if (!t)
                {
                    return JS_FALSE;
                }
                parent = (HLView*)t->object;
            }
            else
            {
                JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "So", &s, &jsobj);
            }
            file = JS_EncodeStringToUTF8(cx, s);
            HLViewLoader loader;
            JSViewLoader jsloader;
            jsloader.obj = jsobj;
            std::string file1 = file;
            file1 = file1.substr(0, file1.find_last_of('.')+1);
            file1 += "uids";
            if (!jsloader.loadFile(file1.c_str()))
            {
                JS_ReportError(cx, "error with uids(%s) file.", file1.c_str());
                return JS_FALSE;
            }
            loader.onAssignVariable = newDelegate(&jsloader, &JSViewLoader::assignVariable);
            loader.onResolveButtonAction = newDelegate(&jsloader, &JSViewLoader::resovleAction);
            HLView* res = loader.loadFile(file, parent);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res->getOrCreateJSWrapper(cx)->jsobject));
            JS_free(cx, file);
            return JS_TRUE;
        }
        
        return JS_FALSE;
    }
    
    bool JSViewLoader::loadFile(const char* file)
    {
//        char fullpath[MAX_PATH];
//        get_full_path(file, fullpath);
//        HLFileData filedata(fullpath, "rb");
        HLFileData* filedata = HLResourceManager::getSingleton()->getFileData(file);
        if (!filedata)
        {
            HLLOG("file %s not exist!", file);
            return false;
        }
        const unsigned char* buffer = filedata->buffer;
        int pos = 0;
        uint8_t count = buffer[pos++];
        for (int i = 0; i < count; ++i)
        {
            uint8_t id = buffer[pos++];
            idmap.insert(std::make_pair(id, std::string((const char*)&buffer[pos])));
            while (buffer[pos++] != 0);
        }
        count = buffer[pos++];
        for (int i = 0; i < count; ++i)
        {
            uint8_t id = buffer[pos++];
            actmap.insert(std::make_pair(id, std::string((const char*)&buffer[pos])));
            while (buffer[pos++] != 0);
        }
        delete filedata;
        return true;
    }
    
    void JSViewLoader::assignVariable(int id, HLView* view)
    {
        JSContext* cx = JSScriptingCore::getSingleton()->getGlobalContext();
        std::map<uint8_t, std::string>::iterator res = idmap.find((uint8_t)id);
        if (res != idmap.end())
        {
            JS::RootedValue value(cx, OBJECT_TO_JSVAL(view->getOrCreateJSWrapper(cx)->jsobject));
            JS_SetProperty(cx, obj, res->second.c_str(), value);
        }
    }
    
    void JSViewLoader::resovleAction(int id, HLButton* button)
    {
        JSContext* cx = JSScriptingCore::getSingleton()->getGlobalContext();
        std::map<uint8_t, std::string>::iterator res = actmap.find((uint8_t)id);
        if (res != actmap.end())
        {
            JSBool found; JS_HasProperty(cx, obj, res->second.c_str(), &found);
            if (found)
            {
                JS::RootedValue fval(cx);
                JS_GetProperty(cx, obj, res->second.c_str(), &fval);
                button->onButtonClick = newDelegate<HLButton*>(obj, fval);
            }
        }
    }
}

NS_HL_END
