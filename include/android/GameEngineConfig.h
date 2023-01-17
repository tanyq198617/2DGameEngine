//
//  config.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-8-13.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HOOLAIENGINE_CONFIG_H__
#define __HOOLAIENGINE_CONFIG_H__

#define PROJECTION_2D 1
#define PROJECTION_3D 2

#define GAME_PROJECTION PROJECTION_2D

#define ENABLE_GL_STATE_CACHE 1

#if defined(ANDROID)
#define TARGET_ANDROID
#elif defined(__IPHONE_OS_VERSION_MAX_ALLOWED)
#define TARGET_IOS
#elif defined(__MAC_OS_X_VERSION_MAX_ALLOWED)
#define TARGET_MACOS
#define USE_TOUCH_EVENT
#elif defined(WIN32)
#define TARGET_WIN32
#define USE_TOUCH_EVENT
#endif

#if defined(TARGET_IOS)
#define ENABLE_RETINA_RESOURCE_SUFFIX 0
#else
#define ENABLE_RETINA_RESOURCE_SUFFIX 0
#endif

#define USE_JS_SCRIPTING 1

#if defined(TARGET_ANDROID)
#define HL_USE_VAO 0
#else
#define HL_USE_VAO 1
#endif

#if defined(TARGET_WIN32)
#include <stdint.h>

#include <WinSock2.h>
#include <windows.h>
#endif

#endif
