//
//  HLHttpNetwork.mm
//  HoolaiEngine
//
//  Created by tyq on 13-3-29.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLHttpNetwork.h"
#include "HLProfileUtil.h"

static NSMutableArray* requestArr = nil;

hoolai::HLTimeProfiler profiler;

namespace hoolai
{
void notifyError(hoolai::HLHttpRequest* request, std::string err)
{
    request->mErrMsg = err;
    request->notifyErrorOnMainThread(NULL);
}

void setResponseCode(hoolai::HLHttpRequest* request, int code)
{
    request->mResCode = code;
}

void notifySucess(hoolai::HLHttpRequest* request, std::vector<char>& data)
{
    request->mResponse = data;
    request->notifySuccessOnMainThread(NULL);
}
}

@interface HLHttpConnectionDelegate : NSObject
{
    hoolai::HLHttpRequest* mRequest;
    NSMutableData* mData;
}

@end

@implementation HLHttpConnectionDelegate

- (id)initWithRequest:(hoolai::HLHttpRequest*)req
{
    self = [super init];
    mRequest = req;
    mRequest->retain();
    NSString* urlstr = [[NSString alloc] initWithUTF8String:mRequest->getUrl().c_str()];
    NSLog(@"url::%@",urlstr);
    
    NSURL* url = [NSURL URLWithString:urlstr];
    NSMutableURLRequest* req1 = [[NSMutableURLRequest alloc] initWithURL:url];
    [req1 setHTTPMethod:[NSString stringWithUTF8String:mRequest->getMethod().c_str()]];
    [req1 setTimeoutInterval:req->getTimeoutInterval()];
    const std::map<std::string, std::string>& headers = mRequest->getHeaders();
    for (std::map<std::string, std::string>::const_iterator itr = headers.begin(); itr != headers.end(); ++itr)
    {
        [req1 setValue:[NSString stringWithUTF8String:itr->second.c_str()] forHTTPHeaderField:[NSString stringWithUTF8String:itr->first.c_str()]];
    }
    if (!strcmp(mRequest->getMethod().c_str(), "POST")
            || !strcmp(mRequest->getMethod().c_str(), "post"))
    {
        NSData* data = [NSData dataWithBytes:&mRequest->getBody()[0] length:mRequest->getBody().size()];
        [req1 setHTTPBody:data];
    }
    
    profiler.reset();
    NSURLConnection* conn = [NSURLConnection connectionWithRequest:req1 delegate:self];
    [conn start];
    return self;
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    hoolai::notifyError(mRequest, [[error localizedDescription] UTF8String]);
    HLLOG("%s", [[error localizedDescription] UTF8String]);
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    HLLOG("connection response: %lf", profiler.elapsed());
    hoolai::setResponseCode(mRequest, (int)[(NSHTTPURLResponse*)response statusCode]);
    mData = [[NSMutableData alloc] init];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    [mData appendData:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    std::vector<char> data;
    data.resize([mData length]);
    memcpy(&data[0], [mData bytes], [mData length]);
    hoolai::notifySucess(mRequest, data);
    
    [requestArr removeObject:self];
}

- (BOOL)connection:(NSURLConnection *)connection canAuthenticateAgainstProtectionSpace:(NSURLProtectionSpace *)protectionSpace{
    return [protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust];
}

- (void)connection:(NSURLConnection *)connection didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge{
    [challenge.sender useCredential:[NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust] forAuthenticationChallenge:challenge];
}

- (void)dealloc
{
    mRequest->release();
}

@end

NS_HL_BEGIN

HLHttpNetwork::HLHttpNetwork()
{
    
}

HLHttpNetwork::~HLHttpNetwork()
{
    
}

void HLHttpNetwork::addRequest(HLHttpRequest* request)
{
    HLHttpConnectionDelegate* conn = [[HLHttpConnectionDelegate alloc] initWithRequest:request];
    if (!requestArr)
    {
        requestArr = [[NSMutableArray alloc] initWithCapacity:2];
    }
    [requestArr addObject:conn];
}

NS_HL_END
