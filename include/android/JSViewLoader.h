//
//  JSViewLoader.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSVIEWLOADER_H__
#define __JSVIEWLOADER_H__

#include "JSCPPWrapper.h"
#include <string>

NS_HL_BEGIN

namespace gui
{
    class HLView;
    class HLButton;
    
    class JSViewLoader
    {
    public:
        static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
        static JSBool jsLoadView(JSContext *cx, uint32_t argc, jsval *vp); /*file, jsobject[, parentview]*/
        
    private:
        JSObject* obj;
        std::map<uint8_t, std::string>idmap;
        std::map<uint8_t, std::string>actmap;
        
        bool loadFile(const char* file);
        
        void assignVariable(int id, HLView* view);
        void resovleAction(int id, HLButton* button);
    };
}

NS_HL_END

#endif
