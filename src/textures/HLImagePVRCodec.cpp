//
//  HLImagePVRCodec.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-3-25.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLImagePVRCodec.h"
#include "HLUtil.h"
#include "HLGLConfiguration.h"
#include <stdint.h>

NS_HL_BEGIN

struct PixelFormatDescription {
    GLuint glInternalFormat;
    GLuint glFormat;
    GLuint glType;
    GLuint bpp;
    bool compressed;
};

extern PixelFormatDescription _pixelFormats[];

typedef struct
{
    uint32_t headerLength;
    uint32_t height;
    uint32_t width;
    uint32_t numMipmaps;
    uint32_t flags;
    uint32_t dataLength;
    uint32_t bpp;
    uint32_t bitmaskRed;
    uint32_t bitmaskGreen;
    uint32_t bitmaskBlue;
    uint32_t bitmaskAlpha;
    uint32_t pvrTag;
    uint32_t numSurfs;
} PVRTexHeader;

#define PVRMIPMAP_MAX 16

enum {
    kPVRTextureFlagMipmap         = (1<<8),        // has mip map levels
    kPVRTextureFlagTwiddle        = (1<<9),        // is twiddled
    kPVRTextureFlagBumpmap        = (1<<10),       // has normals encoded for a bump map
    kPVRTextureFlagTiling         = (1<<11),       // is bordered for tiled pvr
    kPVRTextureFlagCubemap        = (1<<12),       // is a cubemap/skybox
    kPVRTextureFlagFalseMipCol    = (1<<13),       // are there false coloured MIP levels
    kPVRTextureFlagVolume         = (1<<14),       // is this a volume texture
    kPVRTextureFlagAlpha          = (1<<15),       // v2.1 is there transparency info in the texture
    kPVRTextureFlagVerticalFlip   = (1<<16),       // v2.1 is the texture vertically flipped
};

enum
{
	kPVRTexturePixelTypeRGBA_4444= 0x10,
	kPVRTexturePixelTypeRGBA_5551,
	kPVRTexturePixelTypeRGBA_8888,
	kPVRTexturePixelTypeRGB_565,
	kPVRTexturePixelTypeRGB_555,				// unsupported
	kPVRTexturePixelTypeRGB_888,
	kPVRTexturePixelTypeI_8,
	kPVRTexturePixelTypeAI_88,
	kPVRTexturePixelTypePVRTC_2,
	kPVRTexturePixelTypePVRTC_4,
	kPVRTexturePixelTypeBGRA_8888,
	kPVRTexturePixelTypeA_8,
};

enum {
    kInternalPVRTextureFormat,
    kInternalTexturePixelFormat,
};

static const unsigned int tableFormats[][2] = {
    
	// - PVR texture format
	// - texture format constant
	{ kPVRTexturePixelTypeRGBA_4444, kTexturePixelFormat_RGBA4444	},
	{ kPVRTexturePixelTypeRGBA_5551, kTexturePixelFormat_RGB5A1	},
	{ kPVRTexturePixelTypeRGBA_8888, kTexturePixelFormat_RGBA8888	},
	{ kPVRTexturePixelTypeRGB_565,	 kTexturePixelFormat_RGB565	},
	{ kPVRTexturePixelTypeRGB_888,	 kTexturePixelFormat_RGB888	},
	{ kPVRTexturePixelTypeA_8,		 kTexturePixelFormat_A8		},
	{ kPVRTexturePixelTypeI_8,		 kTexturePixelFormat_I8		},
	{ kPVRTexturePixelTypeAI_88,	 kTexturePixelFormat_AI88	},
#ifdef GL_IMG_texture_compression_pvrtc
#if GL_IMG_texture_compression_pvrtc
	{ kPVRTexturePixelTypePVRTC_2,	 kTexturePixelFormat_PVRTC2	},
	{ kPVRTexturePixelTypePVRTC_4,	 kTexturePixelFormat_PVRTC4	},
#endif
#endif
    { kPVRTexturePixelTypeBGRA_8888, kTexturePixelFormat_RGBA8888	},
};

static char gPVRTexIdentifier[5] = "PVR!";
#define PVR_TEXTURE_FLAG_TYPE_MASK    0xff
#define MAX_TABLE_ELEMENTS (sizeof(tableFormats) / sizeof(tableFormats[0]))

HLImagePVRCodec::~HLImagePVRCodec()
{
    if (mData)
    {
        delete []mData;
    }
}

bool HLImagePVRCodec::loadWithFile(std::string file)
{
    hasPremultipliedAlpha = false;
    
    unsigned char* data = NULL;
    int len = 0;
    
    std::string lowerCase(file);
    for (int i=0; i<lowerCase.length(); i++)
    {
        lowerCase[i] = tolower(lowerCase[i]);
    }
//    char fullPath[MAX_PATH];
//    get_full_path(file.c_str(), fullPath);
    HLFileData* fdata = HLResourceManager::getSingleton()->getFileData(file.c_str());
    
    // read file data, uncompress if neccesary
    if (lowerCase.find(".ccz") != std::string::npos)
    {
        len = inflateCCZFile(fdata, &data);
    }
//    else if (lowerCase.find(".gz") != std::string::npos)
//    {
//        len = inflateGZipFile(fdata, &data);
//    }
    else
    {
        len = (int)fdata->size;
        data = new unsigned char[len];
        memcpy(data, fdata->buffer, len);
    }
    delete fdata;
    
    if (!len)
    {
        HLLOG("file %s load error", file.c_str());
        if (data)
            delete []data;
        return false;
    }
    
    // parse pvr
    bool success = false;
    PVRTexHeader *header = NULL;
    uint32_t dataLength = 0, dataOffset = 0, dataSize = 0;
    uint32_t blockSize = 0, widthBlocks = 0, heightBlocks = 0;
    uint32_t bpp = 4;
    unsigned char *bytes = NULL;
    uint32_t formatFlags;
    
    //Cast first sizeof(PVRTexHeader) bytes of data stream as PVRTexHeader
    header = (PVRTexHeader *)data;
    
    //Make sure that tag is in correct formatting
    LittleEndianToNative<sizeof(header->pvrTag)>(&header->pvrTag);
    
    /*
     Check that given data really represents pvrtexture
     
     [0] = 'P'
     [1] = 'V'
     [2] = 'R'
     [3] = '!'
     */
    if (gPVRTexIdentifier[0] != ((header->pvrTag >>  0) & 0xff) ||
        gPVRTexIdentifier[1] != ((header->pvrTag >>  8) & 0xff) ||
        gPVRTexIdentifier[2] != ((header->pvrTag >> 16) & 0xff) ||
        gPVRTexIdentifier[3] != ((header->pvrTag >> 24) & 0xff))
    {
        HLLOG("Unsupported PVR format. Use the Legacy format until the new format is supported");
        delete []data;
        return false;
    }
    
    HLGLConfiguration *configuration = HLGLConfiguration::getSingleton();
    
    LittleEndianToNative<sizeof(header->flags)>(&header->flags);
    formatFlags = header->flags & PVR_TEXTURE_FLAG_TYPE_MASK;
    bool flipped = (header->flags & kPVRTextureFlagVerticalFlip) ? true : false;
    if (flipped)
    {
        HLLOG("WARNING: Image is flipped. Regenerate it using PVRTexTool");
    }
    
    if (! configuration->isSupportsNPOT() &&
        (header->width != nextPOT(header->width) || header->height != nextPOT(header->height)))
    {
        HLLOG("ERROR: Loding an NPOT texture (%dx%d) but is not supported on this device", header->width, header->height);
        delete []data;
        return false;
    }
    
    uint32_t m_uTableFormatIndex;
    uint32_t m_uNumberOfMipmaps;
    uint32_t m_uWidth;
    uint32_t m_uHeight;
    bool m_bHasAlpha;
    
    for (m_uTableFormatIndex = 0; m_uTableFormatIndex < (unsigned int)MAX_TABLE_ELEMENTS; m_uTableFormatIndex++)
    {
        //Does image format in table fits to the one parsed from header?
        if (tableFormats[m_uTableFormatIndex][kInternalPVRTextureFormat] == formatFlags)
        {
            //Reset num of mipmaps
            m_uNumberOfMipmaps = 0;
            
            //Get size of maimap
            LittleEndianToNative<sizeof(header->width)>(&header->width);
            m_uWidth = width = header->width;
            LittleEndianToNative<sizeof(header->height)>(&header->height);
            m_uHeight = height = header->height;
            
            //Do we use alpha ?
            LittleEndianToNative<sizeof(header->bitmaskAlpha)>(&header->bitmaskAlpha);
            if (header->bitmaskAlpha)
            {
                m_bHasAlpha = true;
            }
            else
            {
                m_bHasAlpha = false;
            }
            
            //Get ptr to where data starts..
            LittleEndianToNative<sizeof(header->dataLength)>(&header->dataLength);
            dataLength = header->dataLength;
            
            //Move by size of header
            bytes = ((unsigned char *)data) + sizeof(PVRTexHeader);
            format = (HLTexturePixelFormat)(tableFormats[m_uTableFormatIndex][kInternalTexturePixelFormat]);
            bpp = _pixelFormats[format].bpp;
            
            // Calculate the data size for each texture level and respect the minimum number of blocks
            while (dataOffset < dataLength)
            {
                switch (formatFlags) {
                    case kPVRTexturePixelTypePVRTC_2:
                        blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
                        widthBlocks = m_uWidth / 8;
                        heightBlocks = m_uHeight / 4;
                        break;
                    case kPVRTexturePixelTypePVRTC_4:
                        blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                        widthBlocks = m_uWidth / 4;
                        heightBlocks = m_uHeight / 4;
                        break;
                    case kPVRTexturePixelTypeBGRA_8888:
                        if (configuration->isSupportsBGRA8888() == false)
                        {
                            HLLOG("TexturePVR. BGRA8888 not supported on this device");
                            return false;
                        }
                    default:
                        blockSize = 1;
                        widthBlocks = m_uWidth;
                        heightBlocks = m_uHeight;
                        break;
                }
                
                // Clamp to minimum number of blocks
                if (widthBlocks < 2)
                {
                    widthBlocks = 2;
                }
                if (heightBlocks < 2)
                {
                    heightBlocks = 2;
                }
                
                dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
                float packetLength = (dataLength - dataOffset);
                packetLength = packetLength > dataSize ? dataSize : packetLength;
                
                //Make record to the mipmaps array and increment coutner
                MipmapPtr mipmap;
                mipmap.address = bytes + dataOffset;
                mipmap.len = packetLength;
                mipmaps.push_back(mipmap);
                m_uNumberOfMipmaps++;
                
                //Check that we didn't overflow
                HLASSERT(m_uNumberOfMipmaps < PVRMIPMAP_MAX,
                         "PVR: Maximum number of mimpaps reached.");
                
                dataOffset += packetLength;
                
                //Update width and height to the next lower power of two
                m_uWidth = MAX(m_uWidth >> 1, 1);
                m_uHeight = MAX(m_uHeight >> 1, 1);
            }
            
            //Mark pass as success
            success = true;
            break;
        }
    }
    
    if (! success)
    {
        HLLOG("WARNING: Unsupported PVR Pixel Format: 0x%2x. Re-encode it with a OpenGL pixel format variant", formatFlags);
        return false;
    }

    mData = data;
    return true;
}

NS_HL_END
