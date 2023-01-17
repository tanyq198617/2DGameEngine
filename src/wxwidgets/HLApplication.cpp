//
//  HLApplication.cpp
//  HoolaiAnimationEditor
//
//  Created by zhao shuan on 12-9-26.
//
//

#include "HLApplication.h"
#include "pthread.h"

NS_HL_BEGIN

void openURL(std::string url)
{
    
}

static HLApplication* pSharedApplication = NULL;

HLApplication::HLApplication(HLApplicationDelegate *delegate)
{
    _delegate = delegate;
    pSharedApplication = this;
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

int HLApplication::run()
{
    return 0;
}

void HLApplication::setAnimationInterval(double interval)
{
}

void HLApplication::swapBuffers()
{
}

NS_HL_END