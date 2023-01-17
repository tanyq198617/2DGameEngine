//
//  HLHttpNetwork.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-2-27.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLHttpNetwork.h"
#include "GameEngineConfig.h"

#if defined(TARGET_MACOS) || defined(TARGET_IOS)
#include "semaphore_darwin.h"
#else
#include <semaphore.h>
#endif

NS_HL_BEGIN

#define MAX_BUF 	 65536

char *wr_buf;
size_t wr_buf_size = 0;
size_t wr_index;

sem_t avail;

/*
 * Write data callback function (called within the context of
 * curl_easy_perform.
 */
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
	size_t segsize = size * nmemb;
    
	/* Check to see if this data exceeds the size of our buffer. If so,
	 * set the user-defined context value and return 0 to indicate a
	 * problem to curl.
	 */
	if (wr_index + segsize >= wr_buf_size) {
		wr_buf_size = wr_index + segsize + 1;
		if (wr_buf) {
			wr_buf = (char*) realloc((void*) wr_buf, wr_buf_size);
		} else {
			wr_buf = (char*) malloc(wr_buf_size);
		}
	}
    
	/* Copy the data from the curl buffer into our buffer */
	memcpy((void *) &wr_buf[wr_index], buffer, (size_t) segsize);
    
	/* Update the write index */
	wr_index += segsize;
    
	/* Null terminate the buffer */
	wr_buf[wr_index] = 0;
    
	/* Return the number of bytes received, indicating to curl that all is okay */
	return segsize;
}

void* workingthread(void* p);

HLRequestQueue::HLRequestQueue()
{
    pthread_mutex_init(&mutex, NULL);
}

void HLRequestQueue::insertRequest(HLHttpRequest* request)
{
    pthread_mutex_lock(&mutex);
	mList.push_back(request);
    pthread_mutex_unlock(&mutex);
}

HLHttpRequest* HLRequestQueue::removeRequest()
{
    pthread_mutex_lock(&mutex);
	if (mList.size() > 0)
    {
		HLHttpRequest* request = mList.front();
		mList.pop_front();
        pthread_mutex_unlock(&mutex);
		return request;
	}
    pthread_mutex_unlock(&mutex);
	return NULL;
}

HLHttpNetwork::HLHttpNetwork()
{
	int ret = sem_init(&avail, 0, 0);
    
	ret = pthread_create(&workingtid, NULL, workingthread, this);
	if (-1 == ret)
    {
        printf("Error on creating network thread!\n");
        abort();
	}
	curl_global_init(CURL_GLOBAL_ALL);
}

HLHttpNetwork::~HLHttpNetwork()
{
    sem_destroy(&avail);
}

void HLHttpNetwork::addRequest(HLHttpRequest* request)
{
    request->retain();
	mQueue.insertRequest(request);
    sem_post(&avail);
}

HLHttpRequest* HLHttpNetwork::getRequest()
{
    return mQueue.removeRequest();
}

void set_share_handle(CURL* curl_handle)
{
    static CURLSH* share_handle = NULL;
    if (!share_handle)
    {
        share_handle = curl_share_init();
        curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
    }
    curl_easy_setopt(curl_handle, CURLOPT_SHARE, share_handle);
    curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);
}

void* workingthread(void* p)
{
    HLHttpNetwork* network = (HLHttpNetwork*)p;
    while (1)
    {
        sem_wait(&avail);
        HLHttpRequest* request = network->getRequest();
        if (request)
        {
            CURL *curl = curl_easy_init();
            set_share_handle(curl);
            if (!curl)
            {
                request->mErrMsg = "curl easy init error";
                request->notifyError();
                continue;
            }
            HLLOG("http request url: %s", request->getUrl().c_str());
            curl_easy_setopt(curl, CURLOPT_URL, request->getUrl().c_str());
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, request->getTimeoutInterval());
            
            int wr_error = 0;
			wr_index = 0;
			// memset(wr_buf, 0, MAX_BUF);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &wr_error);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            if (!strcmp(request->getMethod().c_str(), "POST")
                || !strcmp(request->getMethod().c_str(), "post"))
            {
				curl_easy_setopt(curl, CURLOPT_POST, 1);
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, &request->getBody()[0]);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request->getBody().size());
			}
            const std::map<std::string, std::string>& headers = request->getHeaders();
            struct curl_slist* cHeaders = NULL;
            for (std::map<std::string, std::string>::const_iterator itr = headers.begin(); itr != headers.end(); ++itr)
            {
                cHeaders = curl_slist_append(cHeaders, (itr->first + std::string(":") + itr->second).c_str());
            }
            if (cHeaders)
            {
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER,
                                 cHeaders);
			}
            int ret = curl_easy_perform(curl);
            HLLOG("http request finished %d", ret);
			if (ret != 0)
            {
				const char *msg;
				switch (ret)
                {
                    case CURLE_UNSUPPORTED_PROTOCOL:
                        msg = "CURLE_UNSUPPORTED_PROTOCOL";
                        break;
                    case CURLE_FAILED_INIT:
                        msg = "CURLE_FAILED_INIT";
                        break;
                    case CURLE_URL_MALFORMAT:
                        msg = "CURLE_URL_MALFORMAT";
                        break;
                    case CURLE_URL_MALFORMAT_USER:
                        msg = "CURLE_URL_MALFORMAT_USER";
                        break;
                    case CURLE_COULDNT_RESOLVE_PROXY:
                        msg = "CURLE_COULDNT_RESOLVE_PROXY";
                        break;
                    case CURLE_COULDNT_RESOLVE_HOST:
                        msg = "CURLE_COULDNT_RESOLVE_HOST";
                        break;
                    case CURLE_COULDNT_CONNECT:
                        msg = "CURLE_COULDNT_CONNECT";
                        break;
                    case CURLE_OUT_OF_MEMORY:
                        msg = "CURLE_OUT_OF_MEMORY";
                        break;
                    case CURLE_OPERATION_TIMEDOUT:
                        msg = "CURLE_OPERATION_TIMEDOUT";
                        break;
                    default:
                        msg = "UNKOWN_ERROR";
				}
                request->mErrMsg = msg;
                request->notifyError();
                if (cHeaders)
                {
                    curl_slist_free_all(cHeaders);
                }
                curl_easy_cleanup(curl);
                request->release();
                continue;
            }
            int retcode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
            {
                request->mResCode = retcode;
                if (retcode == 200)
                {
                    request->mResponse.resize(wr_index);
                    memcpy(&request->mResponse[0], wr_buf, wr_index);
                }
                request->notifySuccess();
            }
            if (cHeaders)
            {
                curl_slist_free_all(cHeaders);
            }
            curl_easy_cleanup(curl);
            request->release();
        }
    }
    return NULL;
}

NS_HL_END
