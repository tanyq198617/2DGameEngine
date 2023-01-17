//
//  UnitLogicTest.m
//  UnitLogicTest
//
//  Created by tyq on 12-5-16.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import "UnitLogicTest.h"
#include "HLStringUtil.h"

using namespace hoolai;

//class TestObject: public hoolai::HLObject {
//private:
//    bool * mflag;
//public:
//    TestObject(bool * flag) {
//        mflag = flag;
//    }
//    virtual ~TestObject() {
//        *mflag = true;
//    }
//};

@implementation UnitLogicTest

- (void)setUp
{
    [super setUp];
    
    // Set-up code here.
}

- (void)tearDown
{
    // Tear-down code here.
    
    [super tearDown];
}

//- (void)testAutoreleasePool
//{
//    bool flag = false;
//    bool flag2 = false;
////    {
//        hoolai::HLAutoreleasePool pool;
//        TestObject *test = new TestObject(&flag);
//        {
//            hoolai::HLAutoreleasePool pool1;
//            TestObject *test2 = new TestObject(&flag2);
//            test2->autorelease();
//        }
//        test->autorelease();
////    }
//    STAssertTrue(!flag&&flag2, @"object failed autorelease");
//}

- (void)testStringUtilBase64
{
    const char* str = "1234ABC你好helloworld";
    
    std::string encoded = hoolai::StringUtil::Base64Encode((const unsigned char*)str, strlen(str));
    NSLog(@"%s", encoded.c_str());
    int len;
    std::vector<unsigned char>data = hoolai::StringUtil::Base64Decode(encoded.c_str(), len);
    std::string decoded = std::string((const char *)&data[0], len);
    NSLog(@"%@", [NSString stringWithUTF8String:decoded.c_str()]);
    STAssertTrue(!strcmp(str, decoded.c_str()), @"testStringUtilBase64 failed");
}

- (void)testUrlEncodeNDecode
{
    std::string str = "1234ABC你好helloworld_&:#";
    std::string encoded = hoolai::StringUtil::UrlEncode(str);
    NSLog(@"%s", encoded.c_str());
    std::string decoded = hoolai::StringUtil::UrlDecode(encoded);
    STAssertTrue(str==decoded, @"testUrlEncodeNDecode failed");
}

@end
