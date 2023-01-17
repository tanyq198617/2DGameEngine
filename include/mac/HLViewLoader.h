//
//  HLViewLoader.h
//  HoolaiEngine
//
//  Created by tyq on 13-2-25.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLVIEWLOADER_H__
#define __HLVIEWLOADER_H__

#include "HLMacros.h"
#include "HLDelegate.h"

NS_HL_BEGIN

#define VIEWLOADER_ASSIGN_VARIABLE(TYPE, NAME, ID, OBJECT) \
    if(VAR_##NAME == ID) {\
        NAME = static_cast<TYPE>(OBJECT); \
    }

#define VIEWLOADER_ASSIGN_ACTION(CLASS, METHOD, ID, BUTTON) \
    if(ACTION_##METHOD == ID) {\
        BUTTON->onButtonClick = newDelegate(this, &CLASS::METHOD);\
    }

namespace gui
{
    namespace model
    {
        class View;
    }
    
    class HLView;
    class HLButton;
    
    typedef CDelegate2<int /*variable id*/, HLView* /*view object*/> VariableHandler;
    typedef CDelegate2<int /*action id*/, HLButton* /*view object*/> ButtonActionHandler;
    typedef CDelegate3<std::string /*class name*/, HLView** /*view object to create*/, HLView* /*parent*/> CustomClassHandler;
    typedef CDelegate1<HLView* /*view object*/> ViewLoaderHandler;
    class HLViewLoader
    {
    public:
        VariableHandler onAssignVariable;
        CustomClassHandler onCreateCustomView;
        ButtonActionHandler onResolveButtonAction;
        ViewLoaderHandler onViewLoaded;
        
        HLView* loadFile(const char* file, HLView* parent = NULL);
        
    private:
        HLView* loadView(const model::View& modelView, HLView* parent);
        
        std::string filepath;
    };
    
}

NS_HL_END

#endif
