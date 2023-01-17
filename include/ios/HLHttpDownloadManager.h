//
//  HLHttpDownloadManager.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-7-22.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLHTTPDOWNLOADMANAGER_H__
#define __HLHTTPDOWNLOADMANAGER_H__

#include "HLDelegate.h"
#include "HLSingleton.h"

NS_HL_BEGIN

typedef IDelegate3<std::string, float/*progress*/, float/*speed*/>* HTTPDownloadListener;

class HLHttpDownloadManager: public HLSingleton<HLHttpDownloadManager>
{
    friend class HLSingleton<HLHttpDownloadManager>;
public:
    void addDownloadTask(std::string url, std::string path, HTTPDownloadListener listener);
};

NS_HL_END

#endif
