//
//  EAGLView.h
//  gltest
//
//  Created by tyq on 12-5-2.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>

@interface HLGLView : NSOpenGLView 

/** initializes the CCGLView with a frame rect and an OpenGL context */
- (id)initWithFrame:(NSRect)frameRect shareContext:(NSOpenGLContext*)context;

/** uses and locks the OpenGL context */
-(void) lockOpenGLContext;

/** unlocks the openGL context */
-(void) unlockOpenGLContext;

/** returns the depth format of the view in BPP */
- (NSUInteger) depthFormat;

+ (id) sharedGLView;
    
@end
