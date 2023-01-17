//
//  HLResourceManager.h
//  HoolaiEngine
//
//  Created by tyq on 13-1-17.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLRESOURCEMANAGER_H__
#define __HLRESOURCEMANAGER_H__

#include <iostream>
#include <vector>
#include <map>
#include "HLMacros.h"
#include "HLSingleton.h"
#include "HLFileUtil.h"

NS_HL_BEGIN

class HLResourcePackageLoader;

class HLResourceManager:public HLSingleton<HLResourceManager>
{
    friend class HLSingleton<HLResourceManager>;
    friend class HLResourcePackageLoader;
public:
    HLFileData* getFileData(const char* file);
    bool fileExists(const char* file);
    void addResourcePackage(const char* file);
    const char* getSysPathForFile(const char* file);
    void getSysPathForFile(const char* file, char path[]);
    float getResourceScale();
    unsigned long getResourceVersion();
    void setResourceScale(float scale);
    void addFileSearchPath(const char* path);
    const char* getWritablePath();
    void initialize() {initResourcePaths();}
    void updateResourcePackage(const char* package, const char* patchfile);
    void removeResourcePackage(const char* file);
    unsigned long getResourceVersionOfPackage(const char* package);
    bool hasResourcePackage(const char* package);
    
#if defined(TARGET_IOS)
    const char* getBundleFullPath();
#endif
    
private:
    HLResourceManager():mResourceScale(1.0f),mResourceVersion(0),mResLoader(NULL) {}
    HLResourceManager(HLResourceManager const&){}
    virtual ~HLResourceManager();
    
    void initResourcePaths();
    
    void initResourcePathsNative();
    HLFileData* getFileDataInAppBundle(const char* file);
    bool fileInAppBundle(const char* file);
#if defined(TARGET_ANDROID)
    bool copyFileInAssetToWritablePath(const char* file);
#endif
    
    float mResourceScale;
    unsigned long mResourceVersion;
    
    std::vector<std::string> mFileSearchPaths;
    std::map<std::string, HLResourcePackageLoader*> mResLoaders;
    
    HLResourcePackageLoader* mResLoader;
    
    std::string mWritablePath;
    std::string mResPackageFile;
};

NS_HL_END

#endif
