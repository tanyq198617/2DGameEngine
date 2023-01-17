//
//  HLClassRegister.h
//  HLAMFFramework
//
//  Created by tyq on 12-4-6.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLCLASSREGISTER_H__
#define __HLCLASSREGISTER_H__

#include <string>
#include <map>

template <class T> void* constructor() { return (void*)new T(); }

struct factory
{
    typedef void*(*constructor_t)();
    typedef std::map<std::string, constructor_t> map_type;
    map_type m_classes;
    
    template <class T>
    void register_class(const char* n)
    { m_classes.insert(std::make_pair(std::string(n), &constructor<T>)); }
    
    void* construct(std::string const& n)
    {
        map_type::iterator i = m_classes.find(n);
        if (i == m_classes.end()) return 0; // or throw or whatever you want
        return i->second();
    }
};

extern factory g_factory;

#define REGISTER_CLASS(n) g_factory.register_class<n>(#n)
#define REGISTER_CLASS_AS(n, n1) g_factory.register_class<n>(n1)

#endif
