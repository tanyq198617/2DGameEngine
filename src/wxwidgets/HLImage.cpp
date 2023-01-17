//
//  HLImage.c
//  HoolaiAnimationEditor
//
//  Created by zhao shuan on 12-9-26.
//
//

#include "HLImage.h"
#include "HLFileUtil.h"
#include <string>
#include "GameEngineConfig.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif

#include "wx/wxprec.h"
#include "wx/bitmap.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

NS_HL_BEGIN

HLImage::HLImage(const char* path)
{
//    char fullpath[MAX_PATH];
//    get_full_path(path, fullpath);
//    HLFileData data(fullpath, "rb");
//    loadImage(data.buffer, data.size);
}

HLImage::HLImage(const char* text, const char* fontName, int size):_scale(1)
{
    size = 10;
    wxFont font(size, wxDEFAULT, wxNORMAL, wxNORMAL);
    
    wxMemoryDC dc;
    dc.SetFont(font);
    wxSize bmsize = dc.GetTextExtent(text);
    wxBitmap bmp(bmsize.x, bmsize.y);
    dc.SelectObject(bmp);
    dc.SetTextForeground(*wxBLACK);
    dc.DrawText(text, 0, 0);
    dc.SelectObject(wxNullBitmap);
    
     wxImage image = bmp.ConvertToImage();
    
    int width = image.GetWidth();
    int height = image.GetHeight();
    _width = width;
    _height = height;
    _hasAlpha = true;
    _bitsPerComponent = 8;
    _preMulti = true;
    _data = new unsigned char[width*height*4];
    
    
    wxImage img = bmp.ConvertToImage();
    GLubyte *bitmapData=img.GetData();
    GLubyte *alphaData=img.GetAlpha();
    int bytesPerPixel = img.HasAlpha() ?  4 : 3;
    int rev_val=height-1;
    
    for(int y=0; y<height; y++)
    {
        for(int x=0; x<width; x++)
        {
            _data[(x+y*(width))*bytesPerPixel+0]=
            bitmapData[( x+(rev_val-y)*(width))*3];
            
            _data[(x+y*(width))*bytesPerPixel+1]=
            bitmapData[( x+(rev_val-y)*(width))*3 + 1];
            
            _data[(x+y*(width))*bytesPerPixel+2]=
            bitmapData[( x+(rev_val-y)*(width))*3 + 2];
            
            if(bytesPerPixel==4) _data[(x+y*(width))*bytesPerPixel+3]=
                alphaData[ x+(rev_val-y)*(width) ];
            else _data[(x+y*(width))*bytesPerPixel+3] = 255;
        }
    }
    
}

NS_HL_END