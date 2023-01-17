//
//  JSGUIManager.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSGUIManager.h"
#include "HLGUIManager.h"
#include "JSWidget.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSGUIManager, gui::HLGUIManager>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSGUIManager, gui::HLGUIManager>::proto = NULL;

namespace gui
{
    
    void JSGUIManager::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
    {
        if (!jsclass)
        {
            jsclass = (JSClass *)calloc(1, sizeof(JSClass));
        }
        jsclass->name = name;
        jsclass->addProperty = JS_PropertyStub;
        jsclass->delProperty = JS_DeletePropertyStub;
        jsclass->getProperty = JS_PropertyStub;
        jsclass->setProperty = JS_StrictPropertyStub;
        jsclass->enumerate = JS_EnumerateStub;
        jsclass->resolve = JS_ResolveStub;
        jsclass->convert = JS_ConvertStub;
        jsclass->finalize = NULL;
        jsclass->flags = JSCLASS_HAS_PRIVATE;
        
        static JSPropertySpec properties[] = {
            {0, 0, 0, 0, 0}
        };
        
        static JSFunctionSpec funcs[] = {
            JS_FS_END
        };
        
        static JSFunctionSpec st_funcs[] = {
            JS_FN("getRootWidget", JSGUIManager::jsGetRootWidget, 0, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FS_END
        };
        
		proto = JS_InitClass(cx,globalObj,NULL,jsclass,JSGUIManager::jsConstructor,0,properties,funcs,NULL,st_funcs);
    }
    
    JSBool JSGUIManager::jsGetRootWidget(JSContext *cx, uint32_t argc, jsval *vp)
    {
		HLGUIManager::getSingleton()->getRootWidget();
        //JS_SET_RVAL(cx, vp, JSVAL_VOID);
		jsval ret = OBJECT_TO_JSVAL(JSRootWidget::getOrCreateWrapper(cx, HLGUIManager::getSingleton()->getRootWidget())->jsobject);
		JS_SET_RVAL(cx, vp, ret);
        return JS_TRUE;
    }
    
}

NS_HL_END
