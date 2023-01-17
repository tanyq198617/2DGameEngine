//
//  HLHttpRequest.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-2-27.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLHTTPREQUEST_H__
#define __HLHTTPREQUEST_H__

#include "HLMacros.h"
#include "HLDelegate.h"
#include "HLObject.h"
#include <vector>
#if defined(TARGET_ANDROID) || defined(TARGET_WIN32)
#include "curl/curl.h"
#endif

NS_HL_BEGIN

class HLHttpRequest;

typedef CDelegate3<HLHttpRequest*, int/*response code*/, std::vector<char>/*response data*/> HTTPResponseHandler;
typedef CDelegate2<HLHttpRequest*, const char* /*errmsg*/> HTTPErrHandler;

class HLHttpRequest: public HLObject
{
    friend void* workingthread(void* p);
    friend void notifyError(HLHttpRequest*, std::string);
    friend void setResponseCode(HLHttpRequest*, int);
    friend void notifySucess(HLHttpRequest*, std::vector<char>&);
public:
    HTTPResponseHandler onResponse;
    HTTPErrHandler onError;
    
public:
    HLHttpRequest(std::string url);
    ~HLHttpRequest();
    const std::string& getUrl() const
    {
        return mUrl;
    }
    const std::string& getMethod() const
    {
        return mMethod;
    }
    const std::vector<char>& getBody() const
    {
        return mBody;
    }
    int getTimeoutInterval() const
    {
        return mTimeoutInterval;
    }
    int getTag() const
    {
        return mTag;
    }
    void setBody(const char* body, size_t len)
    {
        mBody.clear();
        mBody.resize(len);
        memcpy(&mBody[0], body, len);
    }
    void setMethod(std::string method)
    {
        mMethod = method;
    }
    void setTimeoutInterval(int interval)
    {
        mTimeoutInterval = interval;
    }
    void setTag(int tag)
    {
        mTag = tag;
    }
    void setUserData(HLObject* userData)
    {
        userData->retain();
        if (mUserData)
        {
            mUserData->release();
        }
        mUserData = userData;
    }
    HLObject* getUserData()
    {
        return mUserData;
    }
    void setHeader(std::string header, std::string value);
    static void setGlobalHeader(std::string header, std::string value);
	const std::map<std::string, std::string>& getHeaders() const;
    
    void startRequest();
    
private:
    void notifySuccessOnMainThread(HLObject*);
    void notifyErrorOnMainThread(HLObject*);
    void notifySuccess();
    void notifyError();
    
    std::string mUrl;
    std::vector<char> mBody;
    std::string mMethod;
    int mResCode;
    int mTag;
    int mTimeoutInterval;
    std::vector<char> mResponse;
    std::string mErrMsg;
    std::map<std::string, std::string> mHeaders;
    static std::map<std::string, std::string> mGlobalHeaders;
    HLObject* mUserData;
};

NS_HL_END

#endif
