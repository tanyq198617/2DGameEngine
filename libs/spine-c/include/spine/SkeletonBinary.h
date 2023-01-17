
#ifndef _SPINE_BINARY_H
#define _SPINE_BINARY_H
#pragma  once

#include <spine/Attachment.h>
#include <spine/AttachmentLoader.h>
#include <spine/SkeletonData.h>
#include <spine/Atlas.h>
#include <spine/Animation.h>
#include <string>
#include "Binary.h"

/*
#ifdef __cplusplus
extern "C" {
#endif*/

typedef struct newFloat
{
	float* m_float;
	int len;
	newFloat(float* n_float,int length)
	{
		m_float=n_float;
		len=length;
	};

}spfloat;

typedef struct newShort
{
	short* m_short;
	int len;
	newShort(short* n_short,int length)
	{
		m_short=n_short;
		len=length;
	};

}spshort;

typedef struct newInt
{
	int* m_int;
	int len;
	newInt(int* n_int,int length)
	{
		m_int=n_int;
		len=length;
	};

}spInt;

typedef enum 
{
	 SP_CURVE_LINEAR,
	 SP_CURVE_STEPPED,
	 SP_CURVE_BEZIER
}spCurveTimeType;

typedef struct {
    float r;
    float g;
    float b;
    float a;
} Color;

struct spSkeletonBinary
{
	//spTimeline binary_timeline;
	//spCurveTimeType binary_curve_timeline;
	Color tempcolor;
	float scale;
	spAttachmentLoader* attachmentLoader;
};

spSkeletonBinary* spSkeletonBinary_createWithLoader (spAttachmentLoader* attachmentLoader);
spSkeletonBinary* spSkeletonBinary_create (spAtlas* atlas);
void spSkeletonBinary_dispose (spSkeletonBinary* self);

spSkeletonData* spSkeletonBinary_readSkeletonData(spSkeletonBinary* self, binary* m_binary );
spSkeletonData* spSkeletonBinary_readSkeletonDataFile (spSkeletonBinary* self, const char* path);
Color rgba8888ToColor(int value);
spfloat readFloatArray(float scale);
spInt readIntArray ();
spshort readShortArray ();

static spAnimation* readAnimation (spSkeletonBinary* self,const std::string& name, spSkeletonData* skeletonData,int timelineCount);
void readCurve ( int frameIndex, spCurveTimeline* timeline);
static float toColor (const char* value, int index);
//spAttachment* br_readAttachment(spSkeletonBinary* self, spSkin* skin, const char* attachmentName, bool nonessential);
spAttachment* br_readAttachment(spSkeletonBinary* self, spSkin* skin, const std::string& attachmentName, bool nonessential);

/*

#ifdef SPINE_SHORT_NAMES

typedef s_float spfloat;

typedef s_int spInt;

typedef s_short spshort;

typedef s_curve_type spCurveTimeType;

typedef s_skeleton_binary spSkeletonBinary;

#define CURVE_LINEAR SP_CURVE_LINEAR

#define CURVE_STEPPED SP_CURVE_STEPPED

#define CURVE_BEZIER SP_CURVE_BEZIER

#define  _spSkeletonBinary_createWithLoader(...) spSkeletonBinary_createWithLoader((__VA_ARGS__))

#define  _spSkeletonBinary_create(...) spSkeletonBinary_create((__VA_ARGS__))

#define  _spSkeletonBinary_dispose(...) spSkeletonBinary_dispose((__VA_ARGS__))

#define  _spSkeletonBinary_readSkeletonData(...) spSkeletonBinary_readSkeletonData((__VA_ARGS__))

#define  _spSkeletonBinary_readSkeletonDataFile(...) spSkeletonBinary_readSkeletonDataFile((__VA_ARGS__))

#define  _readFloatArray(...) readFloatArray((__VA_ARGS__))

#define  _rgba8888ToColor(...) rgba8888ToColor((__VA_ARGS__))

#define  _readIntArray(...) readIntArray((__VA_ARGS__))

#define  _readAnimation(...) readAnimation((__VA_ARGS__))

#define  _readShortArray(...) readShortArray((__VA_ARGS__))

#define  _readCurve(...) readCurve((__VA_ARGS__))

#define  _toColor(...) toColor((__VA_ARGS__))

#endif

#ifdef __cplusplus
}
#endif*/

#endif