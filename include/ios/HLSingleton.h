//
//  HLSingleton.h
//  HoolaiEngine
//
//  Created by zhao shuan on 1/3/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLSINGLETON_H__
#define __HLSINGLETON_H__

#include "HLMacros.h"

NS_HL_BEGIN

template <typename T> class HLSingleton
{
private:
    HLSingleton(const HLSingleton<T> &);
    
    HLSingleton& operator=(const HLSingleton<T> &);

protected:
    HLSingleton(void){}
    
    virtual ~HLSingleton(void){}

public:
    static T* getSingleton(void)
    {
        static T instance;
        return &instance;
    }
};

NS_HL_END

#endif
