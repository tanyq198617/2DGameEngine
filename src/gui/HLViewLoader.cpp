
//
//  HLViewLoader.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-2-25.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLViewLoader.h"
#include "guifile.pb.h"
#include "HLFileUtil.h"
#include "HLButton.h"
#include "HLLabel.h"
#include "HLImageView.h"
#include "HLScrollView.h"
#include "HLTexture.h"
#include "HLEditBox.h"

NS_HL_BEGIN

namespace gui
{
    HLView* HLViewLoader::loadFile(const char* file, HLView* parent)
    {
        char path[MAX_PATH];
        get_file_path(file, path);
        filepath = path;
        
//        get_full_path(file, path);
        HLFileData *filedata = HLResourceManager::getSingleton()->getFileData(file);
        
        if (!filedata || !filedata->size)
        {
            if (filedata)
                delete filedata;
            printf("file (%s) not found\n", file);
            return NULL;
        }
        
        model::View modelView;
        bool res = modelView.ParseFromArray(filedata->buffer, (int)filedata->size);
        delete filedata;
        if (!res)
        {
            printf("skeleton data parse error: %s", file);
            return NULL;
        }
        return loadView(modelView, parent);
    }
    
    HLView* HLViewLoader::loadView(const model::View& modelView, HLView* parent)
    {
        HLView *view = NULL;
        if (modelView.has_class_name())
        {
            onCreateCustomView(modelView.class_name(), &view, parent);
            if (!view)
            {
                printf("CustomClass (%s) not created, will create default instance instead.\n", modelView.class_name().c_str());
            }
        }
        
        if (!view)
        {
            switch (modelView.type())
            {
                case model::View::kView:
                {
                    view = new HLView(parent);
                }
                    break;
                case model::View::kButton:
                {
                    view = new HLButton(parent);
                }
                    break;
                case model::View::kLabel:
                {
                    view = new HLLabel(parent);
                }
                    break;
                case model::View::kImageView:
                {
                    view = new HLImageView(parent);
                }
                    break;
                case model::View::kScrollView:
                {
                    view = new HLScrollView(parent);
                }
                    break;
                case model::View::kEditBox:
                {
                    view = new HLEditBox(parent, HLRectZero);
                }
                    break;
                default:
                    printf("unkown view type.\n");
                    return NULL;
                    break;
            }
        }
        
        HLRect frame;
        frame.origin.x = modelView.x();
        frame.origin.y = modelView.y();
        frame.size.width = modelView.width();
        frame.size.height = modelView.height();
        view->setFrame(frame);
        if (modelView.has_bg_color())
        {
            color4B color;
            color.r = modelView.bg_color().r();
            color.g = modelView.bg_color().g();
            color.b = modelView.bg_color().b();
            color.a = modelView.bg_color().a();
            view->setBackgroundColor(color);
        }
        view->setVisible(modelView.visible());
        if (modelView.has_tag())
        {
            view->setTag(modelView.tag());
        }
        
        if (modelView.type() == model::View::kLabel)
        {
            HLLabel* label = static_cast<HLLabel*>(view);
            label->setText(modelView.label().text());
            label->setFontSize(modelView.label().font_size());
            label->setTextStroke(modelView.label().stroke());
            label->setIsFontBold(modelView.label().bold());
            label->setTextAlignment(modelView.label().text_align());
            color4B color;
            color.r = modelView.label().tl_color().r();
            color.g = modelView.label().tl_color().g();
            color.b = modelView.label().tl_color().b();
            color.a = modelView.label().tl_color().a();
            label->setTextColorTL(color);
            color.r = modelView.label().tr_color().r();
            color.g = modelView.label().tr_color().g();
            color.b = modelView.label().tr_color().b();
            color.a = modelView.label().tr_color().a();
            label->setTextColorTR(color);
            color.r = modelView.label().bl_color().r();
            color.g = modelView.label().bl_color().g();
            color.b = modelView.label().bl_color().b();
            color.a = modelView.label().bl_color().a();
            label->setTextColorBL(color);
            color.r = modelView.label().br_color().r();
            color.g = modelView.label().br_color().g();
            color.b = modelView.label().br_color().b();
            color.a = modelView.label().br_color().a();
            label->setTextColorBR(color);
            color.r = modelView.label().stroke_color().r();
            color.g = modelView.label().stroke_color().g();
            color.b = modelView.label().stroke_color().b();
            color.a = modelView.label().stroke_color().a();
            label->setTextStrokeColor(color);
        }
        else if (modelView.type() == model::View::kImageView)
        {
            HLImageView* imageView = static_cast<HLImageView*>(view);
            HLRect imageCap;
            imageCap.origin.x = modelView.image().image_cap().x();
            imageCap.origin.y = modelView.image().image_cap().y();
            imageCap.size.width = modelView.image().image_cap().width();
            imageCap.size.height = modelView.image().image_cap().height();
            std::string image = filepath + modelView.image().image();
            std::string clipImg = modelView.image().clip_image();
            
            if (clipImg.length()>0 && (image.rfind(".json")!=std::string::npos || image.rfind(".bsf")!=std::string::npos))
            {
                HLSpriteFrame* _frame = HLSpriteFrame::getSpriteFrame(image.c_str(), clipImg.c_str());
                imageView->setImage(_frame, imageCap);
            }
            else if (modelView.image().image().length())
            {
                HLTexture* tex = HLTexture::getTexture(image);
                if (tex)
                {
                    imageView->setImage(tex, imageCap);
                }
            }
            color4B color;
            color.r = modelView.image().tint_color().r();
            color.g = modelView.image().tint_color().g();
            color.b = modelView.image().tint_color().b();
            color.a = modelView.image().tint_color().a();
            imageView->setTintColor(color);
            imageView->setShowGrayImage(modelView.image().gray());
            imageView->setRepeatStretch(modelView.image().repeat());
            imageView->setBrightness(modelView.image().brightness());
            imageView->setFlipX(modelView.image().flip_x());
            imageView->setFlipY(modelView.image().flip_y());
            if (modelView.image().has_rotation())
                imageView->setRotation(modelView.image().rotation());
        }
        else if (modelView.type() == model::View::kButton)
        {
            HLButton* button = static_cast<HLButton*>(view);
            HLLabel* label = button->getTitleLabel();
            
            {
                label->setFontSize(modelView.button().label().font_size());
                label->setTextStroke(modelView.button().label().stroke());
                label->setIsFontBold(modelView.button().label().bold());
                label->setTextAlignment(modelView.button().label().text_align());
                color4B color;
                color.r = modelView.button().label().tl_color().r();
                color.g = modelView.button().label().tl_color().g();
                color.b = modelView.button().label().tl_color().b();
                color.a = modelView.button().label().tl_color().a();
                label->setTextColorTL(color);
                color.r = modelView.button().label().tr_color().r();
                color.g = modelView.button().label().tr_color().g();
                color.b = modelView.button().label().tr_color().b();
                color.a = modelView.button().label().tr_color().a();
                label->setTextColorTR(color);
                color.r = modelView.button().label().bl_color().r();
                color.g = modelView.button().label().bl_color().g();
                color.b = modelView.button().label().bl_color().b();
                color.a = modelView.button().label().bl_color().a();
                label->setTextColorBL(color);
                color.r = modelView.button().label().br_color().r();
                color.g = modelView.button().label().br_color().g();
                color.b = modelView.button().label().br_color().b();
                color.a = modelView.button().label().br_color().a();
                label->setTextColorBR(color);
                color.r = modelView.button().label().stroke_color().r();
                color.g = modelView.button().label().stroke_color().g();
                color.b = modelView.button().label().stroke_color().b();
                color.a = modelView.button().label().stroke_color().a();
                label->setTextStrokeColor(color);
            }
            
            {
                if (modelView.button().normal().has_title())
                {
                    button->setNormalTitle(modelView.button().normal().title());
                }
                if (modelView.button().normal().has_image())
                {
                    HLRect imageCap;
                    if (modelView.button().normal().has_image_cap())
                    {
                        imageCap.origin.x = modelView.button().normal().image_cap().x();
                        imageCap.origin.y = modelView.button().normal().image_cap().y();
                        imageCap.size.width = modelView.button().normal().image_cap().width();
                        imageCap.size.height = modelView.button().normal().image_cap().height();
                    }
                    
                    std::string image = filepath + modelView.button().normal().image();
                    std::string clipImg = modelView.button().normal().clip_image();
                    if (clipImg.length()>0 && (image.rfind(".json")!=std::string::npos || image.rfind(".bsf")!=std::string::npos))
                    {
                        HLSpriteFrame* _frame = HLSpriteFrame::getSpriteFrame(image.c_str(), clipImg.c_str());
                        button->setNormalBackgroundImage(_frame, imageCap);
                    }
                    else if (modelView.button().normal().image().length())
                    {
                        HLTexture* tex = HLTexture::getTexture(image);
                        if (tex)
                        {
                            button->setNormalBackgroundImage(tex, imageCap);
                        }
                    }
                }
            }
            
            if (modelView.button().has_selected())
            {
                if (modelView.button().selected().has_title())
                {
                    button->setSelectedTitle(modelView.button().selected().title());
                }
                if (modelView.button().selected().has_image())
                {
                    HLRect imageCap;
                    if (modelView.button().selected().has_image_cap())
                    {
                        imageCap.origin.x = modelView.button().selected().image_cap().x();
                        imageCap.origin.y = modelView.button().selected().image_cap().y();
                        imageCap.size.width = modelView.button().selected().image_cap().width();
                        imageCap.size.height = modelView.button().selected().image_cap().height();
                    }
                    std::string image = filepath + modelView.button().selected().image();
                    std::string clipImg = modelView.button().selected().clip_image();
                    if (clipImg.length()>0 && (image.rfind(".json")!=std::string::npos || image.rfind(".bsf")!=std::string::npos))
                    {
                        HLSpriteFrame* _frame = HLSpriteFrame::getSpriteFrame(image.c_str(), clipImg.c_str());
                        button->setSelectedBackgroundImage(_frame, imageCap);
                    }
                    else if (modelView.button().selected().image().length())
                    {
                        HLTexture* tex = HLTexture::getTexture(image);
                        if (tex)
                        {
                            button->setSelectedBackgroundImage(tex, imageCap);
                        }
                    }
                }
            }
            
            if (modelView.button().has_highlighted())
            {
                if (modelView.button().highlighted().has_title())
                {
                    button->setHighlightedTitle(modelView.button().highlighted().title());
                }
                if (modelView.button().highlighted().has_image())
                {
                    HLRect imageCap;
                    if (modelView.button().highlighted().has_image_cap())
                    {
                        imageCap.origin.x = modelView.button().highlighted().image_cap().x();
                        imageCap.origin.y = modelView.button().highlighted().image_cap().y();
                        imageCap.size.width = modelView.button().highlighted().image_cap().width();
                        imageCap.size.height = modelView.button().highlighted().image_cap().height();
                    }
                    std::string image = filepath + modelView.button().highlighted().image();
                    std::string clipImg = modelView.button().highlighted().clip_image();
                    if (clipImg.length()>0 && (image.rfind(".json")!=std::string::npos || image.rfind(".bsf")!=std::string::npos))
                    {
                        HLSpriteFrame* _frame = HLSpriteFrame::getSpriteFrame(image.c_str(), clipImg.c_str());
                        button->setHighlightedBackgroundImage(_frame, imageCap);
                    }
                    else if (modelView.button().highlighted().image().length())
                    {
                        HLTexture* tex = HLTexture::getTexture(image);
                        if (tex)
                        {
                            button->setHighlightedBackgroundImage(tex, imageCap);
                        }
                    }
                }
            }
            
            if (modelView.button().has_disabled())
            {
                if (modelView.button().disabled().has_title())
                {
                    button->setDisabledTitle(modelView.button().disabled().title());
                    
                }
                if (modelView.button().disabled().has_image())
                {
                    HLRect imageCap;
                    if (modelView.button().disabled().has_image_cap())
                    {
                        imageCap.origin.x = modelView.button().disabled().image_cap().x();
                        imageCap.origin.y = modelView.button().disabled().image_cap().y();
                        imageCap.size.width = modelView.button().disabled().image_cap().width();
                        imageCap.size.height = modelView.button().disabled().image_cap().height();
                    }
                    std::string image = filepath + modelView.button().disabled().image();
                    std::string clipImg = modelView.button().disabled().clip_image();
                    if (clipImg.length()>0 && (image.rfind(".json")!=std::string::npos || image.rfind(".bsf")!=std::string::npos))
                    {
                        HLSpriteFrame* _frame = HLSpriteFrame::getSpriteFrame(image.c_str(), clipImg.c_str());
                        button->setDisabledBackgroundImage(_frame, imageCap);
                    }
                    else if (modelView.button().disabled().image().length())
                    {
                        HLTexture* tex = HLTexture::getTexture(image);
                        if (tex)
                        {
                            button->setDisabledBackgroundImage(tex, imageCap);
                        }
                    }
                }
            }
            
            button->setZoomOnTouchDown(modelView.button().zoom_on_touchdown());
            button->getImageView()->setRepeatStretch(modelView.button().repeat());
            if (modelView.button().has_state())
            {
                button->setButtonState((HLButton::HLButtonState)modelView.button().state());
            }
            if (modelView.button().has_action())
            {
                onResolveButtonAction(modelView.button().action(), button);
            }
        }
        else if (modelView.type() == model::View::kEditBox)
        {
            HLEditBox* editbox = static_cast<HLEditBox*>(view);
            editbox->setPlaceHolder(modelView.editbox().label().text());
            HLLabel* label = editbox->getTextLabel();
            
            {
//                label->setText(modelView.editbox().label().text());
                label->setFontSize(modelView.editbox().label().font_size());
                label->setTextStroke(modelView.editbox().label().stroke());
                label->setIsFontBold(modelView.editbox().label().bold());
                label->setTextAlignment(modelView.editbox().label().text_align());
                color4B color;
                color.r = modelView.editbox().label().tl_color().r();
                color.g = modelView.editbox().label().tl_color().g();
                color.b = modelView.editbox().label().tl_color().b();
                color.a = modelView.editbox().label().tl_color().a();
                label->setTextColorTL(color);
                color.r = modelView.editbox().label().tr_color().r();
                color.g = modelView.editbox().label().tr_color().g();
                color.b = modelView.editbox().label().tr_color().b();
                color.a = modelView.editbox().label().tr_color().a();
                label->setTextColorTR(color);
                color.r = modelView.editbox().label().bl_color().r();
                color.g = modelView.editbox().label().bl_color().g();
                color.b = modelView.editbox().label().bl_color().b();
                color.a = modelView.editbox().label().bl_color().a();
                label->setTextColorBL(color);
                color.r = modelView.editbox().label().br_color().r();
                color.g = modelView.editbox().label().br_color().g();
                color.b = modelView.editbox().label().br_color().b();
                color.a = modelView.editbox().label().br_color().a();
                label->setTextColorBR(color);
                color.r = modelView.editbox().label().stroke_color().r();
                color.g = modelView.editbox().label().stroke_color().g();
                color.b = modelView.editbox().label().stroke_color().b();
                color.a = modelView.editbox().label().stroke_color().a();
                label->setTextStrokeColor(color);
            }
        }
        
        int count = modelView.children_size();
        for (int i = 0; i < count; ++i)
        {
            loadView(modelView.children(i), view);
        }
        
        if (modelView.has_id())
        {
            onAssignVariable(modelView.id(), view);
        }
        
        return view;
    }
}

NS_HL_END
