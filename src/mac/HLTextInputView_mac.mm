 //
//  HLTextInputView.m
//  HoolaiEngine
//
//  Created by tyq on 13-3-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#import "HLTextInputView_mac.h"
#include "HLEditBox.h"
#include "HLFont.h"
#include "HLObject.h"

using namespace hoolai::gui;
using namespace hoolai;

static HLTextInputView* __sysTextInputView = nil;

void __activeTextInputWithEditBox(HLEditBox* editBox)
{
    if (__sysTextInputView)
    {
        __sysTextInputView.editbox->resignActive();
    }
    editBox->onEditBegin(editBox);
    dispatch_sync(dispatch_get_main_queue(), ^(){
        NSSize size = [[NSApplication sharedApplication].mainWindow.contentView frame].size;
        NSRect frame;
        frame.origin.x = frame.origin.y = 0;
        frame.size = size;
        __sysTextInputView = [[HLTextInputView alloc] initWithFrame:frame];
        __sysTextInputView.editbox = editBox;
        [[NSApplication sharedApplication].mainWindow.contentView addSubview:__sysTextInputView];
        [[NSApplication sharedApplication].mainWindow makeFirstResponder:__sysTextInputView];
    });
}

void __deactiveTextInput()
{
    [__sysTextInputView removeFromSuperview];
    __sysTextInputView = nil;
}

class EditBoxImpl: public HLObject
{
public:
    HLEditBox* editBox;
    
    void insertText(HLObject* text)
    {
        HLString* s = (HLString*)text;
        if (s->_value[0] == '\n' && !editBox->allowMultiLine())
        {
            editBox->onReturn(editBox);
            return;
        }
        editBox->insertText(s->_value);
    }
    
    void onReturn(HLObject* ob)
    {
        editBox->onReturn(editBox);
    }
    
    void deleteBackward(HLObject*)
    {
        editBox->deleteBackward();
    }
};

@implementation HLTextInputView

@synthesize editbox;

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        editBoxImpl = new EditBoxImpl();
    }
    
    return self;
}

- (void)setEditbox:(HLEditBox *)eb
{
    editBoxImpl->editBox = eb;
    _maxLength = eb->getMaxLength();
    if (eb->getCharInputFilter()[0])
    {
        _filter = [[NSCharacterSet characterSetWithCharactersInString:[NSString stringWithUTF8String:eb->getCharInputFilter()]] invertedSet];
    }
}

- (HLEditBox *)editbox
{
    return editBoxImpl->editBox;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)becomeFirstResponder
{
    return YES;
}

- (BOOL)resignFirstResponder
{
    return YES;
}

- (void)drawRect:(NSRect)dirtyRect
{
    // Drawing code here.
}

- (void)insertText:(id)aString replacementRange:(NSRange)replacementRange
{
    if (_filter)
    {
        NSRange range;
        while (true)
        {
            range = [aString rangeOfCharacterFromSet:_filter];
            if (range.location == NSNotFound) {
                break;
            }
            aString = [aString stringByReplacingCharactersInRange:range withString:@""];
        }
    }
    if ([aString length] == 0)
        return;
    NSString *tmp = [NSString stringWithUTF8String:editBoxImpl->editBox->getText().c_str()];
    if (_maxLength && tmp.length >= _maxLength)
    {
        return;
    }
    HLString* s = new HLString([aString UTF8String]);
    editBoxImpl->performFuncOnMainThread((CallFuncO)&EditBoxImpl::insertText, s);
    s->release();
}

- (void)doCommandBySelector:(SEL)aSelector
{
    [super doCommandBySelector:aSelector]; // NSResponder's implementation will do nicely
}

- (void)insertNewline:(id)sender
{
    HLString* s = new HLString("\n");
    editBoxImpl->performFuncOnMainThread((CallFuncO)&EditBoxImpl::insertText, s);
    s->release();
}

- (void)deleteBackward:(id)sender
{
    editBoxImpl->performFuncOnMainThread((CallFuncO)&EditBoxImpl::deleteBackward, NULL);
}

- (void)setMarkedText:(id)aString selectedRange:(NSRange)newSelection replacementRange:(NSRange)replacementRange
{
}

- (void)unmarkText
{
}

- (NSRange)selectedRange
{
    return NSMakeRange(NSNotFound, 0);
}

- (NSRange)markedRange
{
    return NSMakeRange(NSNotFound, 0);
}

- (BOOL)hasMarkedText
{
    return NO;
}

- (NSAttributedString *)attributedSubstringForProposedRange:(NSRange)aRange actualRange:(NSRangePointer)actualRange
{
    return nil;
}

- (NSArray *)validAttributesForMarkedText {
    // We only allow these attributes to be set on our marked text (plus standard attributes)
    // NSMarkedClauseSegmentAttributeName is important for CJK input, among other uses
    // NSGlyphInfoAttributeName allows alternate forms of characters
    return [NSArray arrayWithObjects:nil];
}

- (NSRect)firstRectForCharacterRange:(NSRange)aRange actualRange:(NSRangePointer)actualRange
{
    return NSMakeRect(0, 0, 0, 0);
}

- (NSUInteger)characterIndexForPoint:(NSPoint)aPoint
{
    return 0;
}

- (void)keyDown:(NSEvent *)theEvent
{
    [[self inputContext] handleEvent:theEvent];
    return;
}

- (void)mouseDown:(NSEvent *)theEvent
{
//    editBoxImpl->editBox->onEditEnd(editBoxImpl->editBox);
//    editBoxImpl->editBox->resignActive();
    [super mouseDown:theEvent];
}

- (void)dealloc
{
    editBoxImpl->release();
}

@end
