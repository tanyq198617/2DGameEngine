//
//  HLHttpDownloadManager.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-7-22.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLHttpDownloadManager.h"
#include "HLResourceManager.h"
#include "HLProfileUtil.h"
#include "HLStringUtil.h"
#include <errno.h>
#include <sys/xattr.h>
#include <sys/stat.h>

static NSMutableArray* downloadRequestArr = nil;

using namespace hoolai;

@interface HLHttpDownloadConnectionDelegate : NSObject
{
    std::string path;
    FILE* fp;
    CDelegate3<std::string, float, float> delegate;
    size_t totalSize;
    size_t currentSize;
    size_t downloaded;
    HLTimeProfiler profiler;
}

-(id)initWithUrl:(std::string)url path:(std::string)path delegate:(HTTPDownloadListener)delegate;

@end

@implementation HLHttpDownloadConnectionDelegate

-(id)initWithUrl:(std::string)url path:(std::string)filepath delegate:(HTTPDownloadListener)del
{
    self = [super init];
    if (self)
    {
        delegate = del;
        fp = NULL;
        NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];
        if(filepath.length())
        {
            path = filepath;
        }
        else
        {
            NSString* str = [[[NSString stringWithUTF8String:url.c_str()] lastPathComponent] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
            path = [str UTF8String];
        }
        if (path[0] != '/')
        {
            path = HLResourceManager::getSingleton()->getWritablePath() + path;
        }
        path += ".tmp";
        size_t startpos = 0;
        currentSize = 0;
        if (access(path.c_str(), 0) == 0)
        {
            fp = fopen(path.c_str(), "rb");
            fseek(fp, 0, SEEK_END);
            startpos = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            fclose(fp);
            fp = NULL;
        }
        if (startpos)
            [request addValue:[NSString stringWithFormat:@"bytes=%ld-", startpos] forHTTPHeaderField:@"Range"];
        
        NSURLConnection* conn = [NSURLConnection connectionWithRequest:request delegate:self];
        [conn start];
        
        if (!downloadRequestArr)
        {
            downloadRequestArr = [[NSMutableArray alloc] initWithCapacity:2];
        }
        [downloadRequestArr addObject:self];
    }
    return self;
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    delegate([[error localizedDescription] UTF8String], -1, 0);
    [connection cancel];
    [downloadRequestArr removeObject:self];
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    if ([(NSHTTPURLResponse*)response statusCode] == 404)
    {
        delegate("404 Not Found", -1, 0);
        [connection cancel];
        [downloadRequestArr removeObject:self];
        return;
    }
    NSDictionary *headers = [(NSHTTPURLResponse*)response allHeaderFields];
    totalSize = [[headers objectForKey:@"Content-Length"] intValue];
    NSString* ContentRange = [headers objectForKey:@"Content-Range"];
    NSString* LastModified = [headers objectForKey:@"Last-Modified"];
    
    if (ContentRange && [ContentRange length] >= 8 && !strncasecmp([ContentRange UTF8String], "bytes */", 8))
    {
        return;
    }
    
    if (LastModified) // check if we need redownload
    {
        std::string version1 = StringUtil::MD5([LastModified UTF8String]);
        if (access(path.c_str(), 0) == 0 && ContentRange)
        {
            char version[33];
            version[32] = 0;
            getxattr(path.c_str(), "hlversion", version, 32, 0, 0);
            
            if (strcmp((const char*)version, version1.c_str()))
            {
                remove(path.c_str());
                [connection cancel];
                
                NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL:[response URL]];
//                [request addValue:@"bytes=0-" forHTTPHeaderField:@"Range"];
                NSURLConnection* conn = [NSURLConnection connectionWithRequest:request delegate:self];
                [conn start];
                return;
            }
        }
        else if (access(path.substr(0, path.length()-4).c_str(), 0) == 0)
        {
            do {
                std::string path1 = path.substr(0, path.length()-4);
                struct stat statbuff;
                unsigned long filesize = -1;
                if(stat(path1.c_str(), &statbuff) >= 0)
                {
                    filesize = statbuff.st_size;
                }
                if (filesize > 0)
                {
                    if (ContentRange)
                    {
                        NSRange range = [ContentRange rangeOfString:@"/"];
                        range.location++;
                        range.length = [ContentRange length] - range.location;
                        totalSize = [[ContentRange substringWithRange:range] intValue];
                    }
                    if (filesize != totalSize)
                    {
                        remove(path1.c_str());
                        fp = fopen(path.c_str(), "wb");
                        fclose(fp);
                        fp = NULL;
                        setxattr(path.c_str(), "hlversion", version1.c_str(), 32, 0, 0);
                        break;
                    }
                }
                
                char version[33] = {0};
                getxattr(path1.c_str(), "hlversion", version, 32, 0, 0);
                
                if (strcmp((const char*)version, version1.c_str()))
                {
                    remove(path1.c_str());
                    fp = fopen(path.c_str(), "wb");
                    fclose(fp);
                    fp = NULL;
                    setxattr(path.c_str(), "hlversion", version1.c_str(), 32, 0, 0);
                }
                else
                {
                    [connection cancel];
                    delegate(path, 1.f, downloaded/profiler.elapsed());
                    [downloadRequestArr removeObject:self];
                    return;
                }
            } while (0);
        }
        else
        {
            fp = fopen(path.c_str(), "wb");
            fclose(fp);
            fp = NULL;
            setxattr(path.c_str(), "hlversion", version1.c_str(), 32, 0, 0);
        }
    }
    
    if (ContentRange)
    {
        NSRange range;
        range.location = NSNotFound;
        range.length = 0;
        int i = 0;
        for (i = 0; i < [ContentRange length]; ++i)
        {
            unichar c = [ContentRange characterAtIndex:i];
            if (c >= '0' && c <= '9')
            {
                range.location = i;
                break;
            }
        }
        for (; i < [ContentRange length]; ++i)
        {
            unichar c = [ContentRange characterAtIndex:i];
            if (c < '0' || c > '9')
            {
                range.length = i - range.location;
                break;
            }
        }
        if (range.location != NSNotFound && range.length > 0)
        {
            currentSize = [[ContentRange substringWithRange:range] intValue];
            if (currentSize > 0)
            {
                range = [ContentRange rangeOfString:@"/"];
                range.location++;
                range.length = [ContentRange length] - range.location;
                totalSize = [[ContentRange substringWithRange:range] intValue];
                fp = fopen(path.c_str(), "ab");
            }
        }
        if (!fp)
        {
            currentSize = 0;
            fp = fopen(path.c_str(), "ab");
        }
    }
    else // Redownload from start
    {
        currentSize = 0;
        fp = fopen(path.c_str(), "ab");
    }
    
    if (!fp)
    {
        if (errno)
        {
            HLLOG("%s", strerror(errno));
            delegate(strerror(errno), -1, 0);
        }
        else
        {
            delegate("[HLHttpDownloadManager] error: cannot create file: unknown error", -1, 0);
        }
        [connection cancel];
        [downloadRequestArr removeObject:self];
    }
    profiler.reset();
    downloaded = 0;
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    if (!fp)
    {
        NSLog(@"%@",[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding]);
        delegate("[HLHttpDownloadManager] error: no file to store data", -1, 0);
        [connection cancel];
        [downloadRequestArr removeObject:self];
        return;
    }
    currentSize += [data length];
    downloaded += [data length];
    fwrite([data bytes], [data length], 1, fp);
    if (currentSize < totalSize)
    {
        delegate(path, currentSize/(float)totalSize, downloaded/profiler.elapsed());
    }
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    if (fp)
    {
        fclose(fp);
        fp = NULL;
    }
    std::string path1 = path.substr(0, path.size()-4);
    if (access(path1.c_str(), 0) == 0)
    {
        remove(path1.c_str());
    }
    if (rename(path.c_str(), path1.c_str()) < 0)
    {
        delegate("[HLHttpDownloadManager]error: rename file failed", -1, 0);
    }
    else
    {
        delegate(path, 1.f, downloaded/profiler.elapsed());
    }
    [downloadRequestArr removeObject:self];
}

- (void)dealloc
{
    if (fp)
    {
        fclose(fp);
    }
}

@end

NS_HL_BEGIN

void HLHttpDownloadManager::addDownloadTask(std::string url, std::string path, HTTPDownloadListener listener)
{
    if (![[HLHttpDownloadConnectionDelegate alloc] initWithUrl:url path:path delegate:listener])
    {
        NSLog(@"alloc HLHttpConnectionDelegate failed");
    }
}

NS_HL_END
