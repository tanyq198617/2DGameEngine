//
//  HLTextInputView.h
//  HoolaiEngine
//
//  Created by tyq on 13-3-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#import <Cocoa/Cocoa.h>

namespace hoolai
{
    namespace gui
    {
        class HLEditBox;
    }
}

class EditBoxImpl;

@interface HLTextInputView : NSView <NSTextInputClient>
{
    EditBoxImpl* editBoxImpl;
    
    int _maxLength;
    NSCharacterSet * _filter;
}

@property(assign)hoolai::gui::HLEditBox* editbox;

@end
