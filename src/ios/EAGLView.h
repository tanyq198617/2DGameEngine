//
//  EAGLView.h
//  gltest
//
//  Created by tyq on 12-5-2.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "EAGLRenderer.h"

@interface EAGLView : UIView {
    EAGLRenderer * renderer_;
    __unsafe_unretained EAGLContext *context_;
    NSString *pixelformat_;
    GLuint depthFormat_;
    BOOL preserveBackbuffer_;
    
    CGSize size_;
    BOOL discardFramebufferSupported_;
    
    //fsaa addition
    BOOL multisampling_;
    unsigned int requestedSamples_;
@private
    NSString *              markedText_;
    CGRect                  caretRect_;
}

/** pixel format: it could be RGBA8 (32-bit) or RGB565 (16-bit) */
@property(nonatomic,readonly) NSString* pixelFormat;
/** depth format of the render buffer: 0, 16 or 24 bits*/
@property(nonatomic,readonly) GLuint depthFormat;

/** returns surface size in pixels */
@property(nonatomic,readonly) CGSize surfaceSize;

/** OpenGL context */
@property(nonatomic,unsafe_unretained,readonly) EAGLContext *context;

@property(nonatomic,readwrite) BOOL multiSampling;

+ (id) sharedEGLView;
- (id) initWithFrame:(CGRect)frame;
- (id) initWithFrame:(CGRect)frame pixelFormat:(NSString*)format;
- (id) initWithFrame:(CGRect)frame pixelFormat:(NSString*)format depthFormat:(GLuint)depth preserveBackbuffer:(BOOL)retained sharegroup:(EAGLSharegroup*)sharegroup multiSampling:(BOOL)sampling numberOfSamples:(unsigned int)nSamples supportsRetina:(BOOL)retina;
-(void) swapBuffers;
- (CGRect) convertRectFromViewToSurface:(CGRect)rect;
- (CGPoint) convertPointFromViewToSurface:(CGPoint)point;

@end
