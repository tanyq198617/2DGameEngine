//
//  HLApplication.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-5-30.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLApplication.h"
#include "HLDirector2D.h"
#include <stdio.h>

extern bool appWillTerminate;

NS_HL_BEGIN

static HLApplication* pSharedApplication = NULL;

HLApplication::HLApplication(HLApplicationDelegate *delegate)
{
    _delegate = delegate;
    pSharedApplication = this;
    _currLanguage = kLanguageUnknown;
#if defined(TARGET_WIN32)
	setAnimationInterval(1 / 60.0);
#endif
}

HLApplication::~HLApplication()
{
    pSharedApplication = NULL;
    delete _delegate;
}

HLApplication* HLApplication::sharedApplication()
{
    //    HLASSERT(pSharedApplication, "no application created");
    return pSharedApplication;
}

void HLApplication::applicationDidFinishLaunching()
{
    _delegate->applicationDidFinishLaunching();
}
void HLApplication::applicationDidEnterBackground()
{
    _delegate->applicationDidEnterBackground();
    HLDirector2D::getSingleton()->stopAnimation();
}
void HLApplication::applicationWillEnterForeground()
{
    _delegate->applicationWillEnterForeground();
    HLDirector2D::getSingleton()->startAnimation();
}
void HLApplication::applicationWillTerminate()
{
    appWillTerminate = true;
    _delegate->applicationWillTerminate();
}

NS_HL_END
