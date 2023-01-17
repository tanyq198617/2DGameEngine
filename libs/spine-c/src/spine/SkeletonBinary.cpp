#include <spine/SkeletonBinary.h>
#include <stdio.h>
#include <spine/extension.h>
#include <spine/AtlasAttachmentLoader.h>
#include <vector>

typedef struct {
	spSkeletonBinary super;
	int ownsLoader;
} _spSkeletonBinary;

enum class spBinaryTimelineType {
    SCALE = 0,
    ROTATE = 1,
    TRANSLATE  = 2,
    ATTACHMENT = 3,
    COLOR = 4,
    FLIPX = 5,
    FLIPY = 6
};

void spSkeletonBinary_dispose( spSkeletonBinary* self )
{
	if (SUB_CAST(_spSkeletonBinary, self)->ownsLoader) spAttachmentLoader_dispose(self->attachmentLoader);
	FREE(self);
}

spSkeletonBinary* spSkeletonBinary_createWithLoader( spAttachmentLoader* attachmentLoader )
{
	spSkeletonBinary* self = SUPER(NEW(_spSkeletonBinary));
	self->attachmentLoader=attachmentLoader;
	self->scale=1;
    self->tempcolor=Color{0,0,0,0};

	return self;
}

spSkeletonBinary* spSkeletonBinary_create(spAtlas* atlas)
{
	spAtlasAttachmentLoader* attachmentLoader = spAtlasAttachmentLoader_create(atlas);
	spSkeletonBinary* self = spSkeletonBinary_createWithLoader(SUPER(attachmentLoader));
	SUB_CAST(_spSkeletonBinary, self)->ownsLoader = 1;
	return self;
}

spSkeletonData* spSkeletonBinary_readSkeletonData( spSkeletonBinary* self,  binary* m_binary )
{
	int timelinesCount=0;
	int i,n;
	float scale=self->scale;
	spSkeletonData* skeletonData=spSkeletonData_create();
	std::string m_version,m_hash;
	m_hash=readString();
	m_version=readString();
	
    MALLOC_STR(skeletonData->version, m_version.c_str());
    MALLOC_STR(skeletonData->hash, m_hash.c_str());
	skeletonData->width = readfloat();
	skeletonData->height = readfloat();

	bool nonessential = readBoolean();
    
    if (nonessential)
    {
        readString(); // image path ignore
    }
	// Bones.
	n=readInt(true);
	skeletonData->bones = MALLOC(spBoneData*, n);
    skeletonData->bonesCount = n;

	for (i = 0; i < n; i++) {
		//const char* name = readString().c_str();
		//std::string str = readString();
		auto name=readString();
		spBoneData* parent = 0;
		int parentIndex = readInt(true) - 1;
		if (parentIndex != -1)
		{
			parent = skeletonData->bones[parentIndex];
			
		}
		spBoneData* boneData = spBoneData_create(name.c_str(), parent);
		boneData->x = readfloat()* scale;
		boneData->y = readfloat() * scale;
		boneData->scaleX = readfloat();
		boneData->scaleY = readfloat();
		boneData->rotation = readfloat();
		boneData->length = readfloat()* scale;
        boneData->flipX = readBoolean();
        boneData->flipY = readBoolean();
		boneData->inheritScale = readBoolean();
		boneData->inheritRotation = readBoolean();
		if (nonessential) self->tempcolor = rgba8888ToColor(readInt());
		
		skeletonData->bones[i]=boneData;
	}
	//timelinesCount +=n;
    
    // IK constraints
    n=readInt(true);
    skeletonData->ikConstraintsCount = n;
    skeletonData->ikConstraints = MALLOC(spIkConstraintData*, n);
    for (i = 0; i < n; i++) {
        spIkConstraintData* ikConstraintData = spIkConstraintData_create(readString().c_str());
        int bonesCount = readInt(true);
        ikConstraintData->bonesCount = bonesCount;
        ikConstraintData->bones = MALLOC(spBoneData*, bonesCount);
        for (int ii = 0; ii < bonesCount; ii++)
        {
            ikConstraintData->bones[ii] = skeletonData->bones[readInt(true)];
        }
        ikConstraintData->target = skeletonData->bones[readInt(true)];
        ikConstraintData->mix = readfloat();
        ikConstraintData->bendDirection = (signed char)*read();
        skeletonData->ikConstraints[i] = ikConstraintData;
    }

	// Slots.
	n=readInt(true);
	skeletonData->slots = MALLOC(spSlotData*, n);
    skeletonData->slotsCount = n;
	for (i = 0; i < n; i++) {
		auto slotName=readString();
		spBoneData* boneData = skeletonData->bones[readInt(true)];
		spSlotData* slotData =spSlotData_create(slotName.c_str(), boneData);
        Color tempcolor=rgba8888ToColor(readInt());

		slotData->r=tempcolor.r;
		slotData->g=tempcolor.g;
		slotData->b=tempcolor.b;
		slotData->a=tempcolor.a;	
	
		spSlotData_setAttachmentName(slotData,readString().c_str());
		slotData->additiveBlending = readBoolean();
		skeletonData->slots[i] = slotData;
	}
	//timelinesCount +=n;
	
    // Default skin.
	int slotCount = readInt(true);
    spSkin* defaultSkin = spSkin_create("default");
    for (int i = 0; i < slotCount; i++) {
        int slotIndex = readInt(true);
        
        for (int ii = 0, nn = readInt(true); ii < nn; ii++) {
            
            auto str=readString();
            spSkin_addAttachment(defaultSkin, slotIndex, str.c_str(), br_readAttachment(self,defaultSkin,str,nonessential));
        }
    }

    n=readInt(true)+1;
    skeletonData->skins = MALLOC(spSkin*, n);
    skeletonData->skinsCount = n;

    if (defaultSkin != NULL) {
        skeletonData->defaultSkin = defaultSkin;
        skeletonData->skins[0] = defaultSkin;
    }
    
	// Skins.
    for (int i = 1; i < n; i++)
	{
		auto str = readString();
		const char* skin_name = str.c_str();
		int slotCount = readInt(true);
		if (slotCount == 0) return NULL;
		spSkin* skin = spSkin_create(skin_name);
		for (int j = 0; j < slotCount; j++) {
			int slotIndex = readInt(true);
			for (int ii = 0, nn = readInt(true); ii < nn; ii++) {
				auto str = readString();
				const char* name = str.c_str();
				spSkin_addAttachment(skin, slotIndex, name, br_readAttachment(self,skin,name,nonessential));
			}
		}
		skeletonData->skins[i]=skin;// = readSkin(self, readString().c_str(), nonessential);
	}
	//timelinesCount +=n;
	// Events.

	n=readInt(true);
	skeletonData->events = MALLOC(spEventData*, n);

	for (int i = 0; i < n; i++) {
		auto st=readString();
		spEventData* eventData = spEventData_create(st.c_str());
		eventData->intValue = readInt(false);
		eventData->floatValue = readfloat();
		std::string str=readString();
		MALLOC_STR(eventData->stringValue, str.c_str());
		skeletonData->events[skeletonData->eventsCount++] = eventData;	
	}
	//timelinesCount +=n;

	// Animations.
	n=readInt(true);
	//timelinesCount +=n;
	skeletonData->animations=MALLOC(spAnimation*,n);
    skeletonData->animationsCount = 0;
	for (int i = 0; i < n; i++)
	{
		auto str=readString();
		readAnimation(self,str, skeletonData,timelinesCount);
	}
	
	return skeletonData;
}

static spAnimation*  readAnimation (spSkeletonBinary* self,const std::string& name, spSkeletonData* skeletonData,int timelineCount) 
{
	//int i;
	std::vector<spTimeline*> m_timeline;
	m_timeline.reserve(200);
	spAnimation* animation;
	//spSkeletonBinary* frame;
	//int timelines ;
	float scale = self->scale;
	float duration = 0;
	animation =  NEW(spAnimation);
	MALLOC_STR(animation->name, name.c_str());
	//animation->timelineCount = timelineCount;
	//animation->timelines = MALLOC(spTimeline*, timelineCount);


    int n = readInt(true);
	animation->timelinesCount = n;
	skeletonData->animations[skeletonData->animationsCount] = animation;
	++skeletonData->animationsCount;
    
	// Slot timelines.
	for (int i = 0; i < n; i++) {
        int slotIndex = readInt(true);
        for (int ii = 0, nn = readInt(true); ii < nn; ii++) {
            int timelineType = *read();
            int frameCount = readInt(true);
            spTimelineType type = (spTimelineType)-1;
            if (timelineType==3)
            {
                type=SP_TIMELINE_ATTACHMENT;
            }
            else if (timelineType==4)
            {
                type=SP_TIMELINE_COLOR;
            }
            switch (type) {
            case SP_TIMELINE_COLOR:
                {
                    spColorTimeline* timeline = spColorTimeline_create(frameCount);
                    timeline->slotIndex = slotIndex;
                    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
                        float time = readfloat();
                        auto tempcolor=rgba8888ToColor(readInt());
                        self->tempcolor=tempcolor;
                        spColorTimeline_setFrame(timeline,frameIndex, time, self->tempcolor.r, self->tempcolor.g, self->tempcolor.b, self->tempcolor.a);
                        if (frameIndex < frameCount - 1) readCurve(frameIndex,SUPER(timeline));
                    }
                    
                    m_timeline.push_back(SUPER_CAST(spTimeline, timeline));
                    //animation->timelines[animation->timelineCount-1] = SUPER_CAST(spTimeline, timeline);
                    duration = timeline->frames[frameCount * 5 - 5];
                    if (duration > animation->duration) animation->duration = duration;
                    break;
                }
            case SP_TIMELINE_ATTACHMENT:
                {
                    spAttachmentTimeline* timeline =spAttachmentTimeline_create(frameCount);

                    timeline->slotIndex = slotIndex;
                    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
                    {
                        float f=readfloat();
                        auto str=readString();
                        spAttachmentTimeline_setFrame(timeline,frameIndex, f, str.c_str());
                    }

                
                    m_timeline.push_back(SUPER_CAST(spTimeline, timeline));
                    //animation->timelines[animation->timelineCount-1] = SUPER_CAST(spTimeline, timeline);
                    duration = timeline->frames[frameCount - 1];
                    if (duration > animation->duration) animation->duration = duration;
                    break;
                }
            default:
                break;
            }
        }
	}
    
    // Bone timelines.
    n = readInt(true);
    for (int i = 0; i < n; i++) {
        int boneIndex = readInt(true);
        for (int ii = 0, nn = readInt(true); ii < nn; ii++) {
            int timelineType = *read();
            int frameCount = readInt(true);
            switch ((spBinaryTimelineType)timelineType) {
                case spBinaryTimelineType::ROTATE:
                {
                    spRotateTimeline* timeline = spRotateTimeline_create(frameCount);
                    timeline->boneIndex = boneIndex;
                    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
                        float f1=readfloat();
                        float f2=readfloat();
                        spRotateTimeline_setFrame(timeline,frameIndex,f1,f2);

                        if (frameIndex < frameCount - 1) readCurve(frameIndex, SUPER(timeline));
                    }


                    m_timeline.push_back(SUPER_CAST(spTimeline, timeline));
                    //animation->timelines[animation->timelineCount-1] = SUPER_CAST(spTimeline, timeline);
                    duration = timeline->frames[frameCount * 2 - 2];
                    if (duration > animation->duration) 
                    {
                        animation->duration =duration;
                    }
                    break;
                }
                case spBinaryTimelineType::TRANSLATE:
                case spBinaryTimelineType::SCALE:
                {
                    spTranslateTimeline* timeline;
                    float timelineScale = 1;
                    if (timelineType == (int)spBinaryTimelineType::SCALE)
                    {
                        timeline =spScaleTimeline_create(frameCount);
                    }
                    else {
                        timeline = spTranslateTimeline_create(frameCount);
                        timelineScale = scale;
                    }
                    timeline->boneIndex = boneIndex;
                    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
                        float a1,a2,a3;
                        a1=readfloat();
                        a2=readfloat();
                        a3=readfloat();
                        spTranslateTimeline_setFrame(timeline,frameIndex, a1, a2 * timelineScale, a3  * timelineScale);
                        if (frameIndex < frameCount - 1) readCurve( frameIndex, SUPER(timeline));
                    }


                    m_timeline.push_back(SUPER_CAST(spTimeline, timeline));
                    //animation->timelines[animation->timelineCount-1] = SUPER_CAST(spTimeline, timeline);
                    duration = timeline->frames[frameCount * 3 - 3];
                    if (duration > animation->duration) animation->duration = duration;
                    
                    break;
                }
                case spBinaryTimelineType::FLIPX:
                case spBinaryTimelineType::FLIPY:
                {
                    int x = (timelineType == (int)spBinaryTimelineType::FLIPX);
                    spFlipTimeline *timeline = spFlipTimeline_create(frameCount, x);
                    timeline->boneIndex = boneIndex;
                    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
                    {
                        float a1 = readfloat();
                        int a2 = *read();
                        spFlipTimeline_setFrame(timeline, frameIndex, a1, a2);
                    }
                    m_timeline.push_back(SUPER_CAST(spTimeline, timeline));
                    duration = timeline->frames[frameCount * 2 - 2];
                    if (duration > animation->duration) animation->duration = duration;
                }
                    break;
                default:
                    printf("tineline type %d not supported\n", timelineType);
                    break;
                    
          }
      }
    }
    
    // IK timelines.
    n = readInt(true);
    for (int i = 0; i < n; i++) {
        int ikConstraintIndex = readInt(true);
        int frameCount = readInt(true);
        spIkConstraintTimeline* timeline = spIkConstraintTimeline_create(frameCount);
        timeline->ikConstraintIndex = ikConstraintIndex;
        for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
        {
            spIkConstraintTimeline_setFrame(timeline, frameIndex, readfloat(), readfloat(), (signed char)*read());
            if (frameIndex < frameCount - 1) readCurve(frameIndex, SUPER(timeline));

        }
        m_timeline.push_back(SUPER_CAST(spTimeline, timeline));
        duration = timeline->frames[frameCount * 3 - 3];
        if (duration > animation->duration) animation->duration = duration;
    }
    
		// FFD timelines.
    n = readInt(true);
    for (int i = 0; i < n; i++) {
        int verticesCount=0;
        float* tempVertices;
        spSkin* skin = skeletonData->skins[readInt(true)];
        for (int ii = 0, nn = readInt(true); ii < nn; ii++) {
            int slotIndex = readInt(true);
            
            for (int iii = 0, nnn = readInt(true); iii < nnn; iii++) {
                auto str=readString();
                spAttachment* attachment = spSkin_getAttachment(skin,slotIndex, str.c_str());
                int frameCount = readInt(true);

                if (attachment->type == SP_ATTACHMENT_MESH)
                    verticesCount = SUB_CAST(spMeshAttachment, attachment)->verticesCount;
                else //if (attachment->type == SP_ATTACHMENT_SKINNED_MESH)
                    verticesCount = SUB_CAST(spSkinnedMeshAttachment, attachment)->weightsCount / 3 * 2;

                spFFDTimeline* timeline = spFFDTimeline_create(frameCount,verticesCount);
                timeline->slotIndex = slotIndex;
                timeline->attachment = attachment;

                tempVertices = MALLOC(float, verticesCount);
                for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
                    float time = readfloat();
                    float* vertices;
                    int end = readInt(true);
                    if (end == 0) {
                        if (attachment->type == SP_ATTACHMENT_MESH)
                            vertices=SUB_CAST(spMeshAttachment, attachment)->vertices;
                        else {
                            vertices = tempVertices;
                            memset(vertices, 0, sizeof(float) * verticesCount);
                        }
                    } else {
                        vertices = tempVertices;
                        int start = readInt(true);
                        memset(vertices, 0, sizeof(float) * verticesCount);
                        end += start;
                        if (scale == 1) {
                            for (int v = start; v < end; v++)
                            {
                                //float a1=readfloat();
                                float m1=readfloat();
                                vertices[v] = m1;
                            }
                        } else {
                            for (int v = start; v < end; v++)
                            {
                                float m2=readfloat() * scale;
                                vertices[v] = m2;
                            }
                        }

                        if (attachment->type==SP_ATTACHMENT_MESH) {
                            float* meshVertices = SUB_CAST(spMeshAttachment, attachment)->vertices;
                            for (int v = 0; v < verticesCount; v++)
                            {
                                vertices[v] += meshVertices[v];
                            }
                        }
                    }
//                    memset(vertices, 0, sizeof(float)*verticesCount);
                    spFFDTimeline_setFrame(timeline,frameIndex, time, vertices);
                    if (frameIndex < frameCount - 1) readCurve( frameIndex, SUPER(timeline));
                }
                FREE(tempVertices);
                /*if (!animation->timelines)
                {
                    animation->timelines=MALLOC(spTimeline*,animation->timelineCount);
                }
                else
                {
                    auto temp=MALLOC(spTimeline*,animation->timelineCount-1);
                    memcpy(temp,animation->timelines,sizeof(spTimeline**)*(animation->timelineCount-1));
                    FREE(animation->timelines);
                    animation->timelines=MALLOC(spTimeline*,animation->timelineCount);
                    memcpy(animation->timelines,temp,sizeof(spTimeline**)*(animation->timelineCount));

                    FREE(temp);
                }*/

                m_timeline.push_back(SUPER_CAST(spTimeline, timeline));
                //animation->timelines[animation->timelineCount-1] = SUPER_CAST(spTimeline, timeline);
                duration = timeline->frames[frameCount - 1];
                if (duration > animation->duration) animation->duration = duration;
                
            }
        }
    }

    // Draw order timeline.
    int drawOrderCount = readInt(true);
    if (drawOrderCount > 0) {
        int slotCount = skeletonData->slotsCount;
        spDrawOrderTimeline* timeline =spDrawOrderTimeline_create(drawOrderCount,slotCount);		
        for (int i = 0; i < drawOrderCount; i++) {
            int offsetCount = readInt(true);
            int* drawOrder = new int[slotCount];
            for (int ii = slotCount - 1; ii >= 0; ii--)
                drawOrder[ii] = -1;
            int* unchanged = new int[slotCount - offsetCount];
            int originalIndex = 0, unchangedIndex = 0;
            for (int ii = 0; ii < offsetCount; ii++) {
                int slotIndex = readInt(true);
                // Collect unchanged items.
                while (originalIndex != slotIndex)
                    unchanged[unchangedIndex++] = originalIndex++;
                // Set changed items.
                drawOrder[originalIndex + readInt(true)] = originalIndex;
                originalIndex++;
            }
            // Collect remaining unchanged items.
            while (originalIndex < slotCount)
                unchanged[unchangedIndex++] = originalIndex++;
            // Fill in unchanged items.
            for (int ii = slotCount - 1; ii >= 0; ii--)
                if (drawOrder[ii] == -1) drawOrder[ii] = unchanged[--unchangedIndex];
            delete [] unchanged;
            spDrawOrderTimeline_setFrame(timeline,i, readfloat(), drawOrder);
            delete [] drawOrder;
        }

        m_timeline.push_back(SUPER_CAST(spTimeline, timeline));
        //animation->timelines[animation->timelineCount-1] = SUPER_CAST(spTimeline, timeline);
        duration = timeline->frames[drawOrderCount - 1];
        if (duration > animation->duration) animation->duration = duration;
    }

    // Event timeline.
    int eventCount = readInt(true);
    if (eventCount > 0) {
        spEventTimeline* timeline = spEventTimeline_create(eventCount);
        for (int i = 0; i < eventCount; i++) {
            float time = readfloat();
            spEventData* eventData = skeletonData->events[readInt(true)];
            spEvent* event = spEvent_create(eventData);
            event->intValue = readInt(false);
            event->floatValue =readfloat();
            auto m_bool=readBoolean();
            auto str=readString();
            event->stringValue = m_bool ? str.c_str() : eventData->stringValue;
            spEventTimeline_setFrame(timeline,i,time,event);
            
        }
        
        m_timeline.push_back(SUPER_CAST(spTimeline, timeline));
        //animation->timelines[animation->timelineCount-1] = SUPER_CAST(spTimeline, timeline);
        duration = timeline->frames[eventCount - 1];
        if (duration > animation->duration) animation->duration = duration;		
    }

    size_t size = m_timeline.size();
    animation->timelines=MALLOC(spTimeline*,size);
    animation->timelinesCount = (int)size;
    memcpy(animation->timelines, &m_timeline[0], size * sizeof(void*));

	return animation;
}

Color rgba8888ToColor(int value)
{
    Color color;
	color.r = ((value & 0xff000000) >> 24) / 255.f;
	color.g = ((value & 0x00ff0000) >> 16) / 255.f;
	color.b = ((value & 0x0000ff00) >> 8) / 255.f;
	color.a = ((value & 0x000000ff)) / 255.f;
	return color;
}


static float toColor (const char* value, int index) {
	char digits[3];
	char *error;
	int color;

	if (strlen(value) != 8) return -1;
	value += index * 2;

	digits[0] = *value;
	digits[1] = *(value + 1);
	digits[2] = '\0';
	color = strtoul(digits, &error, 16);
	if (*error != 0) return -1;
	return color / (float)255;
}


spfloat readFloatArray(float scale)
{
	int n = readInt(true);
	float* array = new float[n];
	if (scale == 1) {
		for (int i = 0; i < n; i++)
			array[i] = readfloat();
	} else {
		for (int i = 0; i < n; i++)
			array[i] = readfloat() * scale;
	}
	return spfloat(array,n);

}


spshort readShortArray ()  
{
	int n =readInt(true);
	short* array = new short[n];
	for (int i = 0; i < n; i++)
		array[i] = readShort();
	return spshort(array,n);
}

spInt readIntArray()
{
	int n = readInt(true);
	int* array = new int[n];
	for (int i = 0; i < n; i++)
		array[i] = readInt(true);
	return spInt(array,n);
}

spSkeletonData* spSkeletonBinary_readSkeletonDataFile( spSkeletonBinary* self, const char* path )
{
	spSkeletonData* skeletonData;

	skeletonData = spSkeletonBinary_readSkeletonData(self, Binary_Create(path));
	if (getBinary()!=nullptr)
	{
		binary_dispose(getBinary());
	}
	
	return skeletonData;
}

void readCurve ( int frameIndex, spCurveTimeline* timeline) 
{
	//spCurveTimeType type;
	int id=*read();
	switch (id) {
	case SP_CURVE_LINEAR:
		{
			//spCurveTimeline_setLinear(timeline,frameIndex);
			break;
		}
	case SP_CURVE_STEPPED:
		spCurveTimeline_setStepped(timeline, frameIndex);
		break;
	case SP_CURVE_BEZIER:
		spCurveTimeline_setCurve(timeline,frameIndex,readfloat(),readfloat(),readfloat(),readfloat());
		
		break;
	}
}



spAttachment* br_readAttachment(spSkeletonBinary* self, spSkin* skin, const std::string& attachmentName, bool nonessential)
{
	float scale = self->scale;
    std::string path;
	std::string str=readString();
	std::string name = str;
	if (str.empty())
	{
		name = attachmentName;
	}
	int b=*read();
	spAttachmentType type;
	switch (b)
	{
	case 0: 
		type=SP_ATTACHMENT_REGION;
		break;
	case 1:
		type=SP_ATTACHMENT_BOUNDING_BOX;
		break;
	case 2:
		type=SP_ATTACHMENT_MESH;
		break;
	case 3:
		type=SP_ATTACHMENT_SKINNED_MESH;
		break;
	default:
		break;
	}

	switch (type) {
	case SP_ATTACHMENT_REGION:
		{
			path = readString();
			
			if (path.empty())
			{
				//int len=name.length();
				//path = MALLOC(const char,sizeof(const char)*(name.length()));
                path = name;
			}
			spAttachment* attachment = spAttachmentLoader_newAttachment(self->attachmentLoader, skin,type,name.c_str(), path.c_str());
			//FREE(path);
			if (!attachment)
			{
				return NULL;
			}

			spRegionAttachment* region = SUB_CAST(spRegionAttachment, attachment);
            if (region == NULL) return NULL;
			MALLOC_STR(region->path, path.c_str());
			region->x=readfloat() * scale;
			region->y=readfloat()* scale;
			region->scaleX=readfloat();  
			region->scaleY=readfloat();
			region->rotation=readfloat();  
			region->width=readfloat() * scale;
			region->height=readfloat()* scale;
			auto tempcolor=rgba8888ToColor(readInt());
			region->r=tempcolor.r;
			region->g=tempcolor.g;
			region->b=tempcolor.b;
			region->a=tempcolor.a;
			spRegionAttachment_updateOffset(region);
			return attachment;
			//break;

		}
	case SP_ATTACHMENT_BOUNDING_BOX: 
		{
			spBoundingBoxAttachment* box = spBoundingBoxAttachment_create(name.c_str());

			if (box == NULL) return NULL;
			//box.setVertices(readFloatArray(input, scale));
			spfloat sp_float=readFloatArray(scale);
			//sp_float->m_float=new float[sp_float->len];
			box->verticesCount = sp_float.len;
            box->vertices = sp_float.m_float;

			return SUPER(box);
		}

	case SP_ATTACHMENT_MESH: {
		path = readString();
		//path = str.c_str();
		if (path.empty())
		{
			
			//path = MALLOC(const char,sizeof(const char)*(name.length()));
            path = name;
		}

		spAttachment* attachment = spAttachmentLoader_newAttachment(self->attachmentLoader, skin, type, name.c_str(), path.c_str());
		//FREE(path);
		if (!attachment)
		{
			return NULL;
		}

		spMeshAttachment* mesh = SUB_CAST(spMeshAttachment, attachment);
        if (mesh == nullptr) return NULL;
		MALLOC_STR(mesh->path, path.c_str());
		
		auto uvs = readFloatArray(1);
		auto triangles = readShortArray();
		auto vertices = readFloatArray(scale);
		mesh->verticesCount = vertices.len;
//        mesh->vertices = MALLOC(float, vertices.len);
        mesh->vertices = vertices.m_float;
		mesh->trianglesCount = triangles.len;
		mesh->triangles = MALLOC(int, triangles.len);
		for (int i = 0;i!=triangles.len; ++i){
			mesh->triangles[i] = triangles.m_short[i];
		}
        delete [] triangles.m_short;
        mesh->regionUVs = uvs.m_float;
		spMeshAttachment_updateUVs(mesh);
		auto tempcolor=rgba8888ToColor(readInt());
		mesh->r=tempcolor.r;
		mesh->g=tempcolor.g;
		mesh->b=tempcolor.b;
		mesh->a=tempcolor.a;	
		mesh->hullLength = readInt(true);
		if (nonessential) {
			auto entry=readIntArray();
			mesh->edgesCount = entry.len;
            mesh->edges = entry.m_int;
			mesh->width= readfloat() * scale;
			mesh->height= readfloat() * scale;
		}
		return SUPER(mesh);
		//break;
							 }
	case SP_ATTACHMENT_SKINNED_MESH: 
		{
			path = readString();
			//path = str.c_str();
			if (path.empty())
			{
				//path = MALLOC(const char,sizeof(const char)*(name.length()));
                path = name;
			}

			spAttachment* attachment = spAttachmentLoader_newAttachment(self->attachmentLoader, skin, type, name.c_str(), path.c_str());
			//FREE(path);
			if (!attachment)
			{
				return NULL;
			}
			
			spSkinnedMeshAttachment* mesh = SUB_CAST(spSkinnedMeshAttachment, attachment);
            if (mesh == NULL) return NULL;
			MALLOC_STR(mesh->path, path.c_str());
			
			auto uvs = readFloatArray(1);
			auto triangles = readShortArray();
			int vertexCount = readInt(true);
            mesh->uvsCount = uvs.len;
            mesh->regionUVs = uvs.m_float;
			// mesh->bones=MALLOC(int, uvs.len * 3) ;
			// mesh->weights = MALLOC(float, uvs.len * 3 * 3);
//			float* wei_float=new float[uvs.len * 3 * 3];
            std::vector<float>wei_float;
            wei_float.reserve(uvs.len * 3 * 3);
			//spfloat* weights =new spfloat(wei_float,uvs.len * 3 * 3);
//			int* bone_int=new int[uvs.len * 3];
            std::vector<int>bone_int;
            bone_int.reserve(uvs.len * 3);
			//spInt* bones = new spInt(bone_int,uvs.len* 3);
			for (int i = 0; i < vertexCount; i++) {
				int boneCount = (int)readfloat();
                bone_int.push_back(boneCount);
				for (int nn = i + boneCount * 4; i < nn; i += 4) {
                    bone_int.push_back((int)readfloat());
                    wei_float.push_back(readfloat()* self->scale);
                    wei_float.push_back(readfloat() * self->scale);
                    wei_float.push_back(readfloat());
				}
			}
			///////
            mesh->bonesCount = (int)bone_int.size();
			mesh->bones = MALLOC(int, bone_int.size());
            memcpy(mesh->bones, &bone_int[0], bone_int.size() * sizeof(int));
            mesh->weightsCount = (int)wei_float.size();
			mesh->weights = MALLOC(float, wei_float.size());
            memcpy(mesh->weights, &wei_float[0], wei_float.size()* sizeof(float));
			mesh->trianglesCount = triangles.len;
			mesh->triangles = MALLOC(int, triangles.len);
//            memset(mesh->triangles, 0, sizeof(int)*triangles.len);
			for (int i = 0;i!=triangles.len; ++i)
			{
				mesh->triangles[i] = triangles.m_short[i];
			}
            delete [] triangles.m_short;
			spSkinnedMeshAttachment_updateUVs(mesh);

			auto tempcolor=rgba8888ToColor(readInt());
			mesh->r=tempcolor.r;
			mesh->g=tempcolor.g;
			mesh->b=tempcolor.b;
			mesh->a=tempcolor.a;
			mesh->hullLength=readInt(true);
			if (nonessential) {
				spInt edge=readIntArray();
				mesh->edgesCount=edge.len;
                mesh->edges = edge.m_int;
				mesh->width=readfloat()* scale;
				mesh->height=readfloat()* scale;	
			}
			return SUPER(mesh);
			//break;
		}

	}
	return NULL;
}

