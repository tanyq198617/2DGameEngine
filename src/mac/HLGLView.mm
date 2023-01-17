//
//  EAGLView.m
//  gltest
//
//  Created by tyq on 12-5-2.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import "HLGLView.h"
#import <QuartzCore/QuartzCore.h>
#import "OpenGL_Internal.h"
#include "GameEngineConfig.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "HLMouseDispatcher.h"

extern NSThread *runningThread;

static HLGLView *view = 0;

namespace hoolai
{
    void _setMultiTouchEnabled(bool enable)
    {
    }
}

@implementation HLGLView

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    view = self;
    return self;
}

- (id) initWithFrame:(NSRect)frameRect
{
	self = [self initWithFrame:frameRect shareContext:nil];
	return self;
}

- (id) initWithFrame:(NSRect)frameRect shareContext:(NSOpenGLContext*)context
{
    NSOpenGLPixelFormatAttribute attribs[] =
    {
        NSOpenGLPFAAccelerated,
        //		NSOpenGLPFANoRecovery,
//		NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAColorSize, 32,
		NSOpenGLPFADepthSize, 24,
        
#if 0
		// Must specify the 3.2 Core Profile to use OpenGL 3.2
		NSOpenGLPFAOpenGLProfile,
		NSOpenGLProfileVersion3_2Core,
#endif
        
		0
    };
    
	NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
    
	if (!pixelFormat)
		NSLog(@"No OpenGL pixel format");
    
	if( (self = [super initWithFrame:frameRect pixelFormat:pixelFormat]) ) {
        
		if( context )
			[self setOpenGLContext:context];
        
//#if GAME_PROJECTION == PROJECTION_2D
//        hoolai::HLDirector2D::sharedDirector()->setContentScaleFactor(2);
//#else
//        hoolai::HLDirector3D::sharedDirector()->setContentScaleFactor(2);
//#endif
	}
    
	return self;
}

- (void) update
{
	// XXX: Should I do something here ?
	[super update];
}

- (void) prepareOpenGL
{
	// XXX: Initialize OpenGL context
    
	[super prepareOpenGL];
	
	// Make this openGL context current to the thread
	// (i.e. all openGL on this thread calls will go to this context)
	[[self openGLContext] makeCurrentContext];
	
	// Synchronize buffer swaps with vertical refresh rate
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    //	GLint order = -1;
    //	[[self openGLContext] setValues:&order forParameter:NSOpenGLCPSurfaceOrder];
}

- (NSUInteger) depthFormat
{
	return 24;
}

- (void) reshape
{
	// We draw on a secondary thread through the display link
	// When resizing the view, -reshape is called automatically on the main thread
	// Add a mutex around to avoid the threads accessing the context simultaneously when resizing
    
    if (view != self)
    {
        return;
    }
    
	[self lockOpenGLContext];
    
	NSRect rect = [self bounds];
    
//	CCDirector *director = [CCDirector sharedDirector];
//	[director reshapeProjection: NSSizeToCGSize(rect.size) ];
//    
//	// avoid flicker
//	[director drawScene];
//    //	[self setNeedsDisplay:YES];
    
    hoolai::HLSize size(rect.size.width, rect.size.height);
#if GAME_PROJECTION == PROJECTION_2D
    hoolai::HLDirector2D::getSingleton()->reshapeProjection(size);
#else
    hoolai::HLDirector3D::sharedDirector()->reshapeProjection(size);
#endif
	
	[self unlockOpenGLContext];
}

-(void) lockOpenGLContext
{
	NSOpenGLContext *glContext = [self openGLContext];
	NSAssert( glContext, @"FATAL: could not get openGL context");
    
	[glContext makeCurrentContext];
	CGLLockContext((CGLContextObj)[glContext CGLContextObj]);
}

-(void) unlockOpenGLContext
{
	NSOpenGLContext *glContext = [self openGLContext];
	NSAssert( glContext, @"FATAL: could not get openGL context");
    
	CGLUnlockContext((CGLContextObj)[glContext CGLContextObj]);
}

+ (id) sharedGLView
{
    return view;
}

- (void)dispatchEvent:(NSEvent *)theEvent
{
    hoolai::HLMouseEvent evt;
    evt.clickCount = (int)[theEvent clickCount];
    NSPoint location = [theEvent locationInWindow];
    switch (theEvent.type)
    {
        case NSLeftMouseDown:
            evt.type = hoolai::kHLMouseDown;
            break;
        case NSMouseMoved:
            evt.type = hoolai::kHLMouseMove;
            break;
        case NSLeftMouseDragged:
            evt.type = hoolai::kHLMouseDrag;
            break;
        case NSLeftMouseUp:
            evt.type = hoolai::kHLMouseUp;
            break;
        case NSRightMouseDown:
            evt.type = hoolai::kHLRightMouseDown;
            break;
        case NSRightMouseDragged:
            evt.type = hoolai::kHLRightMouseDrag;
            break;
        case NSRightMouseUp:
            evt.type = hoolai::kHLRightMouseUp;
            break;
        default:
            break;
    }
//    evt.location = hoolai::(location.x, location.y);
//#if GAME_PROJECTION == PROJECTION_2D
    evt.location = hoolai::HLDirector2D::getSingleton()->convertToGLPoint(hoolai::HLPoint(location.x, hoolai::HLDirector2D::getSingleton()->getWinSizeInPixels().height - location.y));
//#else
//    evt.location = hoolai::HLDirector3D::sharedDirector()->convertToGLPoint(hoolai::HLPoint(location.x, location.y));
//#endif
    
    hoolai::HLMouseDispatcher::sharedMouseDispatcher()->onMouseEvent(evt);
}

- (void)mouseDown:(NSEvent *)theEvent
{
    if (runningThread)
        [self performSelector:@selector(dispatchEvent:) onThread:runningThread withObject:theEvent waitUntilDone:NO];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    if (runningThread)
        [self performSelector:@selector(dispatchEvent:) onThread:runningThread withObject:theEvent waitUntilDone:NO];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    if (runningThread)
        [self performSelector:@selector(dispatchEvent:) onThread:runningThread withObject:theEvent waitUntilDone:NO];
}

- (void)mouseUp:(NSEvent *)theEvent
{
    if (runningThread)
        [self performSelector:@selector(dispatchEvent:) onThread:runningThread withObject:theEvent waitUntilDone:NO];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
    if (runningThread)
        [self performSelector:@selector(dispatchEvent:) onThread:runningThread withObject:theEvent waitUntilDone:NO];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
    if (runningThread)
        [self performSelector:@selector(dispatchEvent:) onThread:runningThread withObject:theEvent waitUntilDone:NO];
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
    if (runningThread)
        [self performSelector:@selector(dispatchEvent:) onThread:runningThread withObject:theEvent waitUntilDone:NO];
}

- (void)otherMouseDown:(NSEvent *)theEvent {
}

- (void)otherMouseDragged:(NSEvent *)theEvent {
}

- (void)otherMouseUp:(NSEvent *)theEvent {
}

- (void)mouseEntered:(NSEvent *)theEvent {
}

- (void)mouseExited:(NSEvent *)theEvent {
}

-(void) scrollWheel:(NSEvent *)theEvent {
}

-(BOOL) becomeFirstResponder
{
	return YES;
}

-(BOOL) acceptsFirstResponder
{
	return YES;
}

-(BOOL) resignFirstResponder
{
	return YES;
}

- (void)flagsChanged:(NSEvent *)theEvent
{
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

//- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event 
//{
//#if GAME_PROJECTION == PROJECTION_2D
//    hoolai::HLTouchDispatcher::sharedTouchDispatcher()->hitTest(hoolai::HLPoint(point.x, hoolai::HLDirector2D::sharedDirector()->getWinSize().height - point.y));
//#else
//    hoolai::HLTouchDispatcher::sharedTouchDispatcher()->hitTest(hoolai::HLPoint(point.x, hoolai::HLDirector3D::sharedDirector()->getWinSize().height - point.y));
//#endif
//    return self;
//}

- (NSView *)hitTest:(NSPoint)aPoint
{    
//    hoolai::HLMouseDispatcher::sharedMouseDispatcher()->hitTest(hoolai::HLPoint(aPoint.x, aPoint.y));
//#if GAME_PROJECTION == PROJECTION_2D
//    hoolai::HLMouseDispatcher::sharedMouseDispatcher()->hitTest(hoolai::HLDirector2D::getSingleton()->convertToGLPoint(hoolai::HLPoint(aPoint.x, hoolai::HLDirector2D::getSingleton()->getWinSizeInPixels().height - aPoint.y)));
//#else
//    hoolai::HLMouseDispatcher::sharedMouseDispatcher()->hitTest(hoolai::HLDirector3D::sharedDirector()->convertToGLPoint(hoolai::HLPoint(aPoint.x, aPoint.y)));
//#endif
    return self;
}

- (void)touchesBeganWithEvent:(NSEvent *)theEvent {
}

- (void)touchesMovedWithEvent:(NSEvent *)theEvent {
}

- (void)touchesEndedWithEvent:(NSEvent *)theEvent {
}

- (void)touchesCancelledWithEvent:(NSEvent *)theEvent {
}

@end
