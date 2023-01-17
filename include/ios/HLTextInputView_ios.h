//
//  HLTextImputView_ios.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#import <UIKit/UIKit.h>

namespace hoolai
{
    namespace gui
    {
        class HLEditBox;
    }
}

class EditBoxImpl;

@interface HLTextInputView : UIView<UITextInput, UITextInputTraits>
{
    EditBoxImpl* editBoxImpl;
    UITextInputStringTokenizer* _tokenizer;
    NSMutableString *_text;
    NSRange _markedTextRange;
    NSRange _selectedTextRange;
    
    int _maxLength;
    NSCharacterSet * _filter;
}

- (void)removeObserver;
@property(assign)hoolai::gui::HLEditBox* editbox;
@property(nonatomic) UITextAutocapitalizationType autocapitalizationType;
@property(nonatomic) UIKeyboardType keyboardType;
@property(nonatomic) UIReturnKeyType returnKeyType;
@property(nonatomic) BOOL enablesReturnKeyAutomatically;
@property(nonatomic) UITextAutocorrectionType autocorrectionType; 

@end
