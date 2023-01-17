//
//  HLHttpRequest.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-2-27.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLHttpRequest.h"
#include "HLHttpNetwork.h"

NS_HL_BEGIN

std::map<std::string, std::string> HLHttpRequest::mGlobalHeaders;

HLHttpRequest::HLHttpRequest(std::string url):mUrl(url), mTag(0), mTimeoutInterval(60), mUserData(NULL)
{
    mMethod = "GET";
    mHeaders = mGlobalHeaders;
}

HLHttpRequest::~HLHttpRequest()
{
    if (mUserData)
    {
        mUserData->release();
    }
}

void HLHttpRequest::setHeader(std::string header, std::string value)
{
    std::map<std::string, std::string>::iterator itr = mHeaders.find(header);
    if (itr != mHeaders.end())
    {
        mHeaders.erase(itr);
    }
	mHeaders.insert(std::make_pair(header, value));
}

void HLHttpRequest::setGlobalHeader(std::string header, std::string value)
{
    std::map<std::string, std::string>::iterator itr = mGlobalHeaders.find(header);
    if (itr != mGlobalHeaders.end())
    {
        mGlobalHeaders.erase(itr);
    }
	mGlobalHeaders.insert(std::make_pair(header, value));
}

const std::map<std::string, std::string>& HLHttpRequest::getHeaders() const
{
    return mHeaders;
}

void HLHttpRequest::startRequest()
{
    HLHttpNetwork::getSingleton()->addRequest(this);
}

void HLHttpRequest::notifySuccessOnMainThread(HLObject*)
{
    onResponse(this, mResCode, mResponse);
}

void HLHttpRequest::notifyErrorOnMainThread(HLObject*)
{
    onError(this, mErrMsg.c_str());
}

void HLHttpRequest::notifySuccess()
{
    performFuncOnMainThread((CallFuncO)&HLHttpRequest::notifySuccessOnMainThread, NULL);
}

void HLHttpRequest::notifyError()
{
    performFuncOnMainThread((CallFuncO)&HLHttpRequest::notifyErrorOnMainThread, NULL);
}

NS_HL_END
