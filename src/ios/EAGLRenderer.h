//
//  EAGLRenderer.h
//  gltest
//
//  Created by tyq on 12-5-2.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/QuartzCore.h>

@interface EAGLRenderer : NSObject {
    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth_;
    GLint backingHeight_;
    
    unsigned int    samplesToUse_;
    BOOL            multiSampling_;
    
    unsigned int    depthFormat_;
    unsigned int    pixelFormat_;
    
    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
    GLuint defaultFramebuffer_;
    GLuint colorRenderbuffer_;
    GLuint depthBuffer_;
    
    
    //buffers for MSAA
    GLuint msaaFramebuffer_;
    GLuint msaaColorbuffer_;
    
    EAGLContext *context_;
}

- (id) initWithDepthFormat:(unsigned int)depthFormat withPixelFormat:(unsigned int)pixelFormat withSharegroup:(EAGLSharegroup*)sharegroup withMultiSampling:(BOOL) multiSampling withNumberOfSamples:(unsigned int) requestedSamples;

/** Color Renderbuffer */
@property (nonatomic,readonly) GLuint colorRenderbuffer;

/** Default Renderbuffer */
@property (nonatomic,readonly) GLuint defaultFramebuffer;

/** MSAA Framebuffer */
@property (nonatomic,readonly) GLuint msaaFramebuffer;

/** MSAA Color Buffer */
@property (nonatomic,readonly) GLuint msaaColorbuffer;

/** EAGLContext */
@property (nonatomic,readonly) EAGLContext* context;

- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;

- (EAGLContext*) context;
- (CGSize) backingSize;

- (unsigned int) colorRenderBuffer;
- (unsigned int) defaultFrameBuffer;
- (unsigned int) msaaFrameBuffer;
- (unsigned int) msaaColorBuffer;

@end
