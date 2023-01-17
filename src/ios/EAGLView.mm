//
//  EAGLView.m
//  gltest
//
//  Created by tyq on 12-5-2.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import "EAGLView.h"
#import <QuartzCore/QuartzCore.h>
#import "OpenGL_Internal.h"
#include "GameEngineConfig.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "HLTouchDispatcher.h"

static EAGLView *view = 0;

namespace hoolai
{
    void _setMultiTouchEnabled(bool enable)
    {
        [view setMultipleTouchEnabled:enable?YES:NO];
    }
}

@interface EAGLView (Private)
- (BOOL) setupSurfaceWithSharegroup:(EAGLSharegroup*)sharegroup;
- (unsigned int) convertPixelFormat:(NSString*) pixelFormat;
@end

@implementation EAGLView

@synthesize surfaceSize=size_;
@synthesize pixelFormat=pixelformat_, depthFormat=depthFormat_;
@synthesize context=context_;
@synthesize multiSampling=multiSampling_;

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (id) initWithFrame:(CGRect)frame
{
    return [self initWithFrame:frame pixelFormat:kEAGLColorFormatRGB565 depthFormat:0 preserveBackbuffer:NO sharegroup:nil multiSampling:NO numberOfSamples:0 supportsRetina:YES];
}

- (id) initWithFrame:(CGRect)frame pixelFormat:(NSString*)format 
{
    return [self initWithFrame:frame pixelFormat:format depthFormat:0 preserveBackbuffer:NO sharegroup:nil multiSampling:NO numberOfSamples:0 supportsRetina:YES];
}

- (id) initWithFrame:(CGRect)frame pixelFormat:(NSString*)format depthFormat:(GLuint)depth preserveBackbuffer:(BOOL)retained sharegroup:(EAGLSharegroup*)sharegroup multiSampling:(BOOL)sampling numberOfSamples:(unsigned int)nSamples supportsRetina:(BOOL)retina
{
    if((self = [super initWithFrame:frame]))
    {
        pixelformat_ = format;
        depthFormat_ = depth;
        multisampling_ = sampling;
        requestedSamples_ = nSamples;
        preserveBackbuffer_ = retained;
        markedText_ = nil;
        self.backgroundColor = [UIColor blackColor];
        if( ! [self setupSurfaceWithSharegroup:sharegroup] ) {
            return nil;
        }
        if ([self respondsToSelector:@selector(setContentScaleFactor:)] && retina) {
            self.contentScaleFactor = [[UIScreen mainScreen] scale];
        }
#if GAME_PROJECTION == PROJECTION_2D
        hoolai::HLDirector2D::getSingleton()->setContentScaleFactor(self.contentScaleFactor);
#else
        hoolai::HLDirector3D::sharedDirector()->setContentScaleFactor(self.contentScaleFactor);
#endif
        
        view = self;
        self.multipleTouchEnabled = YES;
    }
    
    return self;
}

-(BOOL) setupSurfaceWithSharegroup:(EAGLSharegroup*)sharegroup
{
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
    
    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:preserveBackbuffer_], kEAGLDrawablePropertyRetainedBacking,
                                    pixelformat_, kEAGLDrawablePropertyColorFormat, nil];
    
    
    renderer_ = [[EAGLRenderer alloc] initWithDepthFormat:depthFormat_
                                          withPixelFormat:[self convertPixelFormat:pixelformat_]
                                           withSharegroup:sharegroup
                                        withMultiSampling:multiSampling_
                                      withNumberOfSamples:requestedSamples_];
    
    NSAssert(renderer_, @"OpenGL ES 2.O is required.");
    if (!renderer_)
        return NO;
    
    context_ = [renderer_ context];
    
    
    CHECK_GL_ERROR();
    
    return YES;
}

- (unsigned int) convertPixelFormat:(NSString*) pixelFormat
{
    // define the pixel format
    GLenum pFormat;
    
    
    if([pixelFormat isEqualToString:@"EAGLColorFormat565"]) 
        pFormat = GL_RGB565;
    else 
        pFormat = GL_RGBA8_OES;
    
    return pFormat;
}

- (CGPoint) convertPointFromViewToSurface:(CGPoint)point
{
    CGRect bounds = [self bounds];
    
    CGPoint ret;
    ret.x = (point.x - bounds.origin.x) / bounds.size.width * size_.width;
    ret.y =  (point.y - bounds.origin.y) / bounds.size.height * size_.height;
    
    return ret;
}

- (CGRect) convertRectFromViewToSurface:(CGRect)rect
{
    CGRect bounds = [self bounds];
    
    CGRect ret;
    ret.origin.x = (rect.origin.x - bounds.origin.x) / bounds.size.width * size_.width;
    ret.origin.y = (rect.origin.y - bounds.origin.y) / bounds.size.height * size_.height;
    ret.size.width = rect.size.width / bounds.size.width * size_.width;
    ret.size.height = rect.size.height / bounds.size.height * size_.height;
    
    return ret;
}

- (void) swapBuffers
{
    // IMPORTANT:
    // - preconditions
    //    -> context_ MUST be the OpenGL context
    //    -> renderbuffer_ must be the the RENDER BUFFER
    
#ifdef __IPHONE_4_0
    
    if (multiSampling_)
    {
        /* Resolve from msaaFramebuffer to resolveFramebuffer */
        //hlglDisable(GL_SCISSOR_TEST);     
        glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, [renderer_ msaaFrameBuffer]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, [renderer_ defaultFrameBuffer]);
        glResolveMultisampleFramebufferAPPLE();
    }
    
    if( discardFramebufferSupported_)
    {    
        if (multiSampling_)
        {
            if (depthFormat_)
            {
                GLenum attachments[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
                glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 2, attachments);
            }
            else
            {
                GLenum attachments[] = {GL_COLOR_ATTACHMENT0};
                glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, attachments);
            }
            
            glBindRenderbuffer(GL_RENDERBUFFER, [renderer_ colorRenderBuffer]);
            
        }    
        
        // not MSAA
        else if (depthFormat_ ) {
            GLenum attachments[] = { GL_DEPTH_ATTACHMENT};
            glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, attachments);
        }
    }
    
#endif // __IPHONE_4_0
    
    if(![context_ presentRenderbuffer:GL_RENDERBUFFER])
    {
        NSLog(@"Error to swap renderbuffer");
        //         CCLOG(@"cocos2d: Failed to swap renderbuffer in %s\n", __FUNCTION__);
    }
    
#if defined(DEBUG)
    CHECK_GL_ERROR();
#endif
    
    // We can safely re-bind the framebuffer here, since this will be the
    // 1st instruction of the new main loop
    if( multiSampling_ )
        glBindFramebuffer(GL_FRAMEBUFFER, [renderer_ msaaFrameBuffer]);    
}

+ (id) sharedEGLView
{
    return view;
}

- (void) layoutSubviews
{
    [renderer_ resizeFromLayer:(CAEAGLLayer*)self.layer];
    size_ = [renderer_ backingSize];
    
    hoolai::HLSize size(size_.width, size_.height);
#if GAME_PROJECTION == PROJECTION_2D
    hoolai::HLDirector2D::getSingleton()->reshapeProjection(size);
#else
    hoolai::HLDirector3D::sharedDirector()->reshapeProjection(size);
#endif
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

bool __shouldDispatchTouch = true;

- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event 
{
    UIView * v = [super hitTest:point withEvent:event];
    if (v != self)
    {
        __shouldDispatchTouch = false;
        return v;
    }
    __shouldDispatchTouch = true;
    hoolai::HLPoint p;
    p.x = point.x;
    p.y = point.y;
    p *= CONTENT_SCALE_FACTOR;
    
//#if GAME_PROJECTION == PROJECTION_2D
//    p = hoolai::HLDirector2D::getSingleton()->convertToGLPoint(p);
//    hoolai::HLTouchDispatcher::sharedTouchDispatcher()->hitTest(hoolai::HLPoint(p.x, hoolai::HLDirector2D::sharedDirector()->getWinSize().height - p.y));
//#else
//    hoolai::HLPoint p = hoolai::HLDirector3D::sharedDirector()->convertToGLPoint(*((hoolai::HLPoint*)((void*)&point)));
//    hoolai::HLTouchDispatcher::sharedTouchDispatcher()->hitTest(hoolai::HLPoint(p.x, hoolai::HLDirector3D::sharedDirector()->getWinSize().height - p.y));
//#endif
//#if GAME_PROJECTION == PROJECTION_2D
//     hoolai::HLTouchDispatcher::sharedTouchDispatcher()->hitTest(p);
//#else
//    hoolai::HLTouchDispatcher::sharedTouchDispatcher()->hitTest(p);
//#endif
    return self;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!__shouldDispatchTouch)
    {
        return;
    }
    hoolai::HLTouch tmp;
    std::vector<hoolai::HLTouch> aTouches;
//#if GAME_PROJECTION == PROJECTION_2D
//    float winHeight = hoolai::HLDirector2D::getSingleton()->getWinSize().height;
//#else
//    float winHeight = hoolai::HLDirector3D::sharedDirector()->getWinSize().height;
//#endif
    for (UITouch *touch in touches)
    {
        CGPoint p = [touch locationInView:self];
        hoolai::HLPoint p1;
        p1.x = p.x;
        p1.y = p.y;
        p1 *= CONTENT_SCALE_FACTOR;
        tmp.location = hoolai::HLDirector2D::getSingleton()->convertToGLPoint(p1);
        tmp._id = (int64_t)touch;
        tmp.tapCount = touch.tapCount;
        aTouches.push_back(tmp);
    }
    hoolai::HLTouchDispatcher::sharedTouchDispatcher()->touchesBegin(aTouches);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!__shouldDispatchTouch)
    {
        return;
    }
    hoolai::HLTouch tmp;
    std::vector<hoolai::HLTouch> aTouches;
//#if GAME_PROJECTION == PROJECTION_2D
//    float winHeight = hoolai::HLDirector2D::getSingleton()->getWinSize().height;
//#else
//    float winHeight = hoolai::HLDirector3D::sharedDirector()->getWinSize().height;
//#endif
    for (UITouch *touch in touches)
    {
        CGPoint p = [touch locationInView:self];
        hoolai::HLPoint p1;
        p1.x = p.x;
        p1.y = p.y;
        p1 *= CONTENT_SCALE_FACTOR;
        tmp.location = hoolai::HLDirector2D::getSingleton()->convertToGLPoint(p1);
        tmp._id = (int64_t)touch;
        tmp.tapCount = touch.tapCount;
        aTouches.push_back(tmp);
    }
    hoolai::HLTouchDispatcher::sharedTouchDispatcher()->touchesMove(aTouches);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!__shouldDispatchTouch)
    {
        return;
    }
    hoolai::HLTouch tmp;
    std::vector<hoolai::HLTouch> aTouches;
//#if GAME_PROJECTION == PROJECTION_2D
//    float winHeight = hoolai::HLDirector2D::getSingleton()->getWinSize().height;
//#else
//    float winHeight = hoolai::HLDirector3D::sharedDirector()->getWinSize().height;
//#endif
    for (UITouch *touch in touches)
    {
        CGPoint p = [touch locationInView:self];
        hoolai::HLPoint p1;
        p1.x = p.x;
        p1.y = p.y;
        p1 *= CONTENT_SCALE_FACTOR;
        tmp.location = hoolai::HLDirector2D::getSingleton()->convertToGLPoint(p1);
        tmp._id = (int64_t)touch;
        tmp.tapCount = touch.tapCount;
        aTouches.push_back(tmp);
    }
    hoolai::HLTouchDispatcher::sharedTouchDispatcher()->touchesEnd(aTouches);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!__shouldDispatchTouch)
    {
        return;
    }
    hoolai::HLTouch tmp;
    std::vector<hoolai::HLTouch> aTouches;
//#if GAME_PROJECTION == PROJECTION_2D
//    float winHeight = hoolai::HLDirector2D::getSingleton()->getWinSize().height;
//#else
//    float winHeight = hoolai::HLDirector3D::sharedDirector()->getWinSize().height;
//#endif
    for (UITouch *touch in touches)
    {
        CGPoint p = [touch locationInView:self];
        hoolai::HLPoint p1;
        p1.x = p.x;
        p1.y = p.y;
        p1 *= CONTENT_SCALE_FACTOR;
        tmp.location = hoolai::HLDirector2D::getSingleton()->convertToGLPoint(p1);
        tmp._id = (int64_t)touch;
        tmp.tapCount = touch.tapCount;
        aTouches.push_back(tmp);
    }
    hoolai::HLTouchDispatcher::sharedTouchDispatcher()->touchesCancel(aTouches);
}

@end
