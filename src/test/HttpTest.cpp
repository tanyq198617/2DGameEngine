//
//  HttpTest.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-2-28.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HttpTest.h"
#include "HLHttpNetwork.h"
#include <vector>

void onHttpResponse(hoolai::HLHttpRequest* request, int responseCode, std::vector<char> data)
{
    printf("response code: %d\n", responseCode);
    std::string res(&data[0], data.size());
    HLLOG("===========%s", res.c_str());
}

void HttpTest::test()
{
    hoolai::HLHttpRequest* request = new hoolai::HLHttpRequest("http://220.228.163.60/user/v2/user/shengQuLoginServer?site=fz_001&appKey=ekVETXlBQ1ZUTkVJeVVqT3hBak8wRURJNUVESTI5bVRnVVdkVTFpTXdBek4=&udid=355868054357984&sessionId=MEVETXlBQ1ZUTkVJM1VqTzFJak8xRURJeUlESWlWbVJnUVhZVDF5TjJJRE93WVRN&idfa=&phone=android");
    request->autorelease();
    request->onResponse = newDelegate(onHttpResponse);
    request->startRequest();
    
//    request = new hoolai::HLHttpRequest("http://www.google.com.hk");
//    request->autorelease();
//    request->onResponse = newDelegate(onHttpResponse);
//    request->startRequest();
}
