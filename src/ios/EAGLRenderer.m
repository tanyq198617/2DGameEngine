//
//  EAGLRenderer.m
//  gltest
//
//  Created by tyq on 12-5-2.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import "EAGLRenderer.h"
#import "OpenGL_Internal.h"
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#ifdef DEBUG
#define CHECK_GL_ERROR() ({ GLenum __error = glGetError(); if(__error) printf("OpenGL error 0x%04X in file %s function %s line %d\n", __error, __FILE__, __FUNCTION__, __LINE__); })
#else
#define CHECK_GL_ERROR()
#endif

@implementation EAGLRenderer

@synthesize context=context_;
@synthesize defaultFramebuffer=defaultFramebuffer_;
@synthesize colorRenderbuffer=colorRenderbuffer_;
@synthesize msaaColorbuffer=msaaColorbuffer_;
@synthesize msaaFramebuffer=msaaFramebuffer_;

// Create an OpenGL ES 2.0 context
- (id) initWithDepthFormat:(unsigned int)depthFormat withPixelFormat:(unsigned int)pixelFormat withSharegroup:(EAGLSharegroup*)sharegroup withMultiSampling:(BOOL) multiSampling withNumberOfSamples:(unsigned int) requestedSamples
{
    self = [super init];
    if (self)
    {
        if( ! sharegroup )
            context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        else
            context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:sharegroup];
        
        if (!context_ || ![EAGLContext setCurrentContext:context_] )
        {
            return nil;
        }
        
        depthFormat_ = depthFormat;
        pixelFormat_ = pixelFormat;
        multiSampling_ = multiSampling;
        
        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffers(1, &defaultFramebuffer_);
        NSAssert( defaultFramebuffer_, @"Can't create default frame buffer");
        
        glGenRenderbuffers(1, &colorRenderbuffer_);
        NSAssert( colorRenderbuffer_, @"Can't create default render buffer");
        
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer_);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer_);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer_);
        
        if (multiSampling_)
        {
            GLint maxSamplesAllowed;
            glGetIntegerv(GL_MAX_SAMPLES_APPLE, &maxSamplesAllowed);
            samplesToUse_ = MIN(maxSamplesAllowed,requestedSamples);
            
            /* Create the MSAA framebuffer (offscreen) */
            glGenFramebuffers(1, &msaaFramebuffer_);
            NSAssert( msaaFramebuffer_, @"Can't create default MSAA frame buffer");
            glBindFramebuffer(GL_FRAMEBUFFER, msaaFramebuffer_);
            
        }
        
        CHECK_GL_ERROR();
    }
    
    return self;
}

- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer
{
    // Allocate color buffer backing based on the current layer size
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer_);
    
    if( ! [context_ renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer] )
    {
        NSLog(@"failed to call context");
    }
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth_);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight_);
    
    NSLog(@"surface size: %dx%d", (int)backingWidth_, (int)backingHeight_);
    
    if (multiSampling_)
    {
        if ( msaaColorbuffer_) {
            glDeleteRenderbuffers(1, &msaaColorbuffer_);
            msaaColorbuffer_ = 0;
        }
        
        /* Create the offscreen MSAA color buffer.
         After rendering, the contents of this will be blitted into ColorRenderbuffer */
        
        //msaaFrameBuffer needs to be binded
        glBindFramebuffer(GL_FRAMEBUFFER, msaaFramebuffer_);
        glGenRenderbuffers(1, &msaaColorbuffer_);
        NSAssert(msaaFramebuffer_, @"Can't create MSAA color buffer");
        
        glBindRenderbuffer(GL_RENDERBUFFER, msaaColorbuffer_);
        
        glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, samplesToUse_, pixelFormat_ , backingWidth_, backingHeight_);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaColorbuffer_);
        
        GLenum error;
        if ( (error=glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
        {
            NSLog(@"Failed to make complete framebuffer object 0x%X", error);
            return NO;
        }
    }
    
    CHECK_GL_ERROR();
    
    if (depthFormat_)
    {
        if( ! depthBuffer_ ) {
            glGenRenderbuffers(1, &depthBuffer_);
            NSAssert(depthBuffer_, @"Can't create depth buffer");
        }
        
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
        
        if( multiSampling_ )
            glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, samplesToUse_, depthFormat_,backingWidth_, backingHeight_);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, depthFormat_, backingWidth_, backingHeight_);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
        
        if (depthFormat_ == GL_DEPTH24_STENCIL8_OES) {
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
		}
        
        // bind color buffer
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer_);        
    }
    
    CHECK_GL_ERROR();
    
    GLenum error;
    if( (error=glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
    {
        NSLog(@"Failed to make complete framebuffer object 0x%X", error);
        return NO;
    }
    
    return YES;
}

-(CGSize) backingSize
{
    return CGSizeMake( backingWidth_, backingHeight_);
}

- (NSString*) description
{
    return [NSString stringWithFormat:@"<%@ = %08X | size = %ix%i>", [self class], self, backingWidth_, backingHeight_];
}

- (unsigned int) colorRenderBuffer
{
    return colorRenderbuffer_;
}

- (unsigned int) defaultFrameBuffer
{
    return defaultFramebuffer_;
}

- (unsigned int) msaaFrameBuffer
{
    return msaaFramebuffer_;
}

- (unsigned int) msaaColorBuffer
{
    return msaaColorbuffer_;
}

- (void)dealloc
{
    NSLog(@"deallocing %@", self);
    
    // Tear down GL
    if (defaultFramebuffer_) {
        glDeleteFramebuffers(1, &defaultFramebuffer_);
        defaultFramebuffer_ = 0;
    }
    
    if (colorRenderbuffer_) {
        glDeleteRenderbuffers(1, &colorRenderbuffer_);
        colorRenderbuffer_ = 0;
    }
    
    if( depthBuffer_ ) {
        glDeleteRenderbuffers(1, &depthBuffer_ );
        depthBuffer_ = 0;
    }
    
    if ( msaaColorbuffer_)
    {
        glDeleteRenderbuffers(1, &msaaColorbuffer_);
        msaaColorbuffer_ = 0;
    }
    
    if ( msaaFramebuffer_)
    {
        glDeleteRenderbuffers(1, &msaaFramebuffer_);
        msaaFramebuffer_ = 0;
    }
    
    // Tear down context
    if ([EAGLContext currentContext] == context_)
        [EAGLContext setCurrentContext:nil];
    
    context_ = nil;
}

@end
