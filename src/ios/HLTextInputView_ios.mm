//
//  HLTextImputView_ios.m
//  HoolaiEngine
//
//  Created by tyq on 13-3-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#import "HLTextInputView_ios.h"
#include "HLEditBox.h"
#include "HLObject.h"
#include "HLDirector2D.h"

using namespace hoolai::gui;
using namespace hoolai;

static HLTextInputView* __sysTextInputView = nil;
static CGRect __oldFrame = CGRectMake(0, 0, 0, 0);
//bool __acceptKeyboardDidHide = true;

void __activeTextInputWithEditBox(HLEditBox* editBox)
{
    CGRect frame = [[[UIApplication sharedApplication].delegate performSelector:@selector(rootViewController)] view].frame;
    if (__oldFrame.size.width == 0 && __oldFrame.size.height == 0)
    {
        __oldFrame = frame;
    }
    editBox->onEditBegin(editBox);
    frame.origin.x = frame.origin.y = 0;
    __sysTextInputView = [[HLTextInputView alloc] initWithFrame:[[[UIApplication sharedApplication].delegate performSelector:@selector(rootViewController)] view].bounds];
    [[[[UIApplication sharedApplication].delegate performSelector:@selector(rootViewController)] view] addSubview:__sysTextInputView];
    __sysTextInputView.editbox = editBox;
//    __acceptKeyboardDidHide = false;
    [__sysTextInputView becomeFirstResponder];
//    __acceptKeyboardDidHide = true;
}

void __deactiveTextInput()
{
//    [__sysTextInputView resignFirstResponder];
    [__sysTextInputView removeFromSuperview];
    [__sysTextInputView removeObserver];
//    __sysTextInputView = nil;
    [UIView beginAnimations:@"adjustframe" context:nil];
    [UIView setAnimationDuration:0.2f];
    [[[UIApplication sharedApplication].delegate performSelector:@selector(rootViewController)] view].frame = __oldFrame;
    [UIView commitAnimations];
}

@interface HLIndexedPosition : UITextPosition

@property (nonatomic) NSUInteger index;
@property (nonatomic) id <UITextInputDelegate> inputDelegate;

+ (instancetype)positionWithIndex:(NSUInteger)index;

@end

@implementation HLIndexedPosition

+ (instancetype)positionWithIndex:(NSUInteger)index
{
    HLIndexedPosition * pos = [[self alloc] init];
    pos.index = index;
    return pos;
}

@end

@interface HLTextRange : UITextRange

@property (nonatomic) NSRange range;

+ (instancetype)textRangeWithRange:(NSRange)range;

@end

@implementation HLTextRange

+ (instancetype)textRangeWithRange:(NSRange)range
{
    if (range.location == NSNotFound)
    {
        return nil;
    }
    
    HLTextRange* tRange = [[HLTextRange alloc] init];
    tRange.range = range;
    return tRange;
}

- (UITextPosition *)start
{
    return [HLIndexedPosition positionWithIndex:self.range.location];
}

- (UITextPosition *)end
{
    return [HLIndexedPosition positionWithIndex:(self.range.location+self.range.length)];
}

-(BOOL)isEmpty
{
    return (self.range.length == 0);
}

@end

class EditBoxImpl: public HLObject
{
public:
    HLEditBox* editBox;
    
    void insertText(HLObject* text)
    {
        HLString* s = (HLString*)text;
        editBox->insertText(s->_value);
    }
    
    void deleteBackward(HLObject*)
    {
        editBox->deleteBackward();
    }
    void deleteBackward(int location)
    {
        editBox->deleteBackward();
    }
};

@implementation HLTextInputView

@synthesize markedTextStyle, inputDelegate;
@synthesize autocapitalizationType, keyboardType, returnKeyType, enablesReturnKeyAutomatically, autocorrectionType;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(keyboardDidShow:)
                                                     name:UIKeyboardDidShowNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(keyboardDidHide:)
                                                     name:UIKeyboardDidHideNotification object:nil];
        
        self.userInteractionEnabled = YES;
        self.enablesReturnKeyAutomatically = YES;
        _tokenizer = [[UITextInputStringTokenizer alloc] initWithTextInput:self];
        editBoxImpl = new EditBoxImpl();
        _text = [[NSMutableString alloc] init];
        autocapitalizationType = UITextAutocapitalizationTypeNone;
        _markedTextRange.location = NSNotFound;
        _markedTextRange.length = 0;
        _selectedTextRange.location = 0;
        _selectedTextRange.length = 0;
        self.autocorrectionType = UITextAutocorrectionTypeNo;
        _maxLength = 0;
    }
    return self;
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

- (UIView*)hitTest:(CGPoint)point withEvent:(UIEvent *)event
{
    return self;
}

- (void)setEditbox:(HLEditBox *)eb
{
    editBoxImpl->editBox = eb;
    switch (eb->getKeyboardReturnType())
    {
        case kKeyboardReturnTypeDefault:
            self.returnKeyType = UIReturnKeyDefault;
            break;
        case kKeyboardReturnTypeNext:
            self.returnKeyType = UIReturnKeyNext;
            break;
        case kKeyboardReturnTypeDone:
            self.returnKeyType = UIReturnKeyDone;
            break;
        case kKeyboardReturnTypeGo:
            self.returnKeyType = UIReturnKeyGo;
            break;
        case kKeyboardReturnTypeSearch:
            self.returnKeyType = UIReturnKeySearch;
            break;
        case kKeyboardReturnTypeSend:
            self.returnKeyType = UIReturnKeySend;
            break;
        default:
            break;
    }
    switch (eb->getInputMode())
    {
        case kEditBoxInputModeAny:
            self.keyboardType = UIKeyboardTypeDefault;
            break;
        case kEditBoxInputModeAscii:
            self.keyboardType = UIKeyboardTypeASCIICapable;
            break;
        case kEditBoxInputModeDecimal:
            self.keyboardType = UIKeyboardTypeDecimalPad;
            break;
        case kEditBoxInputModeNumberPad:
            self.keyboardType = UIKeyboardTypeNumberPad;
            break;
        case kEditBoxInputModeEmailAddr:
            self.keyboardType = UIKeyboardTypeEmailAddress;
            break;
        case kEditBoxInputModeNumeric:
            self.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
            break;
        case kEditBoxInputModePhoneNumber:
            self.keyboardType = UIKeyboardTypePhonePad;
            break;
        case kEditBoxInputModeUrl:
            self.keyboardType = UIKeyboardTypeURL;
            break;
        default:
            break;
    }
    [_text appendString:[NSString stringWithUTF8String:eb->getText().c_str()]];
    _selectedTextRange.location = _text.length;
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

- (void)keyboardDidShow:(NSNotification*)aNotification
{
    NSDictionary* info = [aNotification userInfo];
    CGSize kbSize = [[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue].size;
    UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
    if (UIInterfaceOrientationIsLandscape(orientation))
    {
        float tmp = fmax(kbSize.width, kbSize.height);
        kbSize.height = fmin(kbSize.width, kbSize.height);
        kbSize.width = tmp;
    }
    HLPoint p = self.editbox->convertToWorldSpace(HLPointZero);
    p = HLDirector2D::getSingleton()->convertToScreenPoint(p);
    p.y = self.frame.size.height - p.y;
    UIView* view = [[[UIApplication sharedApplication].delegate performSelector:@selector(rootViewController)] view];
    if (p.y < kbSize.height)
    {
        CGRect frame = __oldFrame;
        CGPoint p1;
        p1.x = 0;
        p1.y = p.y - kbSize.height;
        p1 = [view convertPoint:p1 toView:view.superview];
        CGPoint p2 = [view convertPoint:CGPointZero toView:view.superview];
        frame.origin.x += p1.x - p2.x;
        frame.origin.y += p1.y - p2.y;
        [UIView beginAnimations:@"adjustframe" context:nil];
        [UIView setAnimationDuration:0.2f];
        view.frame = frame;
        [UIView commitAnimations];
    }
}

- (void)keyboardDidHide:(NSNotification*)aNotification
{
//    if (!__acceptKeyboardDidHide)
//    {
//        return;
//    }
//    editBoxImpl->editBox->resignActive();
    [UIView beginAnimations:@"adjustframe" context:nil];
    [UIView setAnimationDuration:0.2f];
    [[[UIApplication sharedApplication].delegate performSelector:@selector(rootViewController)] view].frame = __oldFrame;
    [UIView commitAnimations];
}

- (BOOL)resignFirstResponder
{
    if (editBoxImpl)
    {
        editBoxImpl->editBox->resignActive();
    }
    return [super resignFirstResponder];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    editBoxImpl->editBox->resignActive();
}

- (void)removeObserver
{
    editBoxImpl->release();
    editBoxImpl = NULL;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)dealloc
{
    if (editBoxImpl)
        editBoxImpl->release();
}

- (BOOL)hasText
{
    return (_text.length != 0);
}

- (void)insertText:(NSString *)text
{
    if ([text characterAtIndex:0] == '\n' && !editBoxImpl->editBox->allowMultiLine())
    {
        editBoxImpl->editBox->onReturn(editBoxImpl->editBox);
        return;
    }
    
    if (_filter)
    {
        NSRange range;
        while (true)
        {
            range = [text rangeOfCharacterFromSet:_filter];
            if (range.location == NSNotFound) {
                break;
            }
            text = [text stringByReplacingCharactersInRange:range withString:@""];
        }
    }
    
    if (_maxLength && _text.length >= _maxLength)
    {
        text = @"";
    }
    
    if (_markedTextRange.location != NSNotFound)
    {
        [_text replaceCharactersInRange:_markedTextRange withString:text];
        _selectedTextRange.location = _markedTextRange.location + text.length;
        _selectedTextRange.length = 0;
        _markedTextRange = NSMakeRange(NSNotFound, 0);
        HLString* s = new HLString([text UTF8String]);
        editBoxImpl->insertText(s);
        s->release();
        editBoxImpl->editBox->setMarkedText(NULL);
    }
    else if (_selectedTextRange.length > 0)
    {
        [_text replaceCharactersInRange:_selectedTextRange withString:text];
        _selectedTextRange.length = 0;
        _selectedTextRange.location += text.length;
    }
    else
    {
        [_text appendString:text];
         _selectedTextRange.location += text.length;
        HLString* s = new HLString([text UTF8String]);
        editBoxImpl->insertText(s);
        s->release();
    }
}

- (void)deleteBackward
{
    if (_markedTextRange.location != NSNotFound && _markedTextRange.length > 0)
    {
        editBoxImpl->editBox->setMarkedText([[_text substringWithRange:_markedTextRange] UTF8String]);
        [_text deleteCharactersInRange:_markedTextRange];
        _selectedTextRange.location = _markedTextRange.location;
        _selectedTextRange.length = 0;
        _markedTextRange = NSMakeRange(NSNotFound, 0);
    }
    else if (_selectedTextRange.length > 0)
    {
        [_text deleteCharactersInRange:_selectedTextRange];
        _selectedTextRange.length = 0;
    }
    else if (_selectedTextRange.location > 0)
    {
        _markedTextRange = NSMakeRange(NSNotFound, 0);
        _selectedTextRange.location--;
        _selectedTextRange.length = 1;
        [_text deleteCharactersInRange:_selectedTextRange];
        _selectedTextRange.length = 0;
        editBoxImpl->deleteBackward(0);
    }
}

- (BOOL)shouldChangeTextInRange:(UITextRange *)range replacementText:(NSString *)text
{
    return YES;
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text
{
    return YES;
}

- (UITextRange *)selectedTextRange
{
    return [HLTextRange textRangeWithRange:_selectedTextRange];
}


- (void)setSelectedTextRange:(UITextRange *)range
{
    HLTextRange *indexedRange = (HLTextRange *)range;
    _selectedTextRange = indexedRange.range;
}

- (NSString *)textInRange:(UITextRange *)range
{
    HLTextRange *r = (HLTextRange*)range;
    return [_text substringWithRange:r.range];
}

- (void)replaceRange:(UITextRange *)range withText:(NSString *)text
{
//    HLString* s = new HLString([text UTF8String]);
//    editBoxImpl->insertText(s);
//    s->release();
}

- (UITextRange *)markedTextRange
{
    if (_markedTextRange.length == 0)
    {
        return nil;
    }
    return [HLTextRange textRangeWithRange:_markedTextRange];
//    return nil;
}

- (void)setMarkedText:(NSString *)markedText selectedRange:(NSRange)selectedRange
{
    if (_markedTextRange.location != NSNotFound)
    {
        if (!markedText)
        {
            markedText = @"";
        }
        [_text replaceCharactersInRange:_markedTextRange withString:markedText];
        _markedTextRange.length = markedText.length;
    }
    else if (_selectedTextRange.length > 0)
    {
        [_text replaceCharactersInRange:_selectedTextRange withString:markedText];
        _markedTextRange.location = _selectedTextRange.location;
        _markedTextRange.length = markedText.length;
    }
    else
    {
        [_text insertString:markedText atIndex:_selectedTextRange.location];
        _markedTextRange.location = _selectedTextRange.location;
        _markedTextRange.length = markedText.length;
    }
    _selectedTextRange = NSMakeRange(selectedRange.location+_markedTextRange.location, _selectedTextRange.length);
    editBoxImpl->editBox->setMarkedText([markedText UTF8String]);
}

- (void)unmarkText
{
    if (_markedTextRange.location == NSNotFound)
    {
        return;
    }
    editBoxImpl->editBox->setMarkedText(NULL);
    NSString* markedTxt = [_text substringWithRange:_markedTextRange];
    if (_filter)
    {
        NSRange range;
        while (true)
        {
            range = [markedTxt rangeOfCharacterFromSet:_filter];
            if (range.location == NSNotFound) {
                break;
            }
            markedTxt = [markedTxt stringByReplacingCharactersInRange:range withString:@""];
        }
    }
    [_text replaceCharactersInRange:_markedTextRange withString:markedTxt];
    if (_maxLength && _text.length > _maxLength)
    {
        [_text setString:[_text substringToIndex:_maxLength]];
        editBoxImpl->editBox->setText([_text UTF8String]);
        _selectedTextRange.location = _text.length;
        _selectedTextRange.length = 0;
    }
    else
    {
        HLString* s = new HLString([markedTxt UTF8String]);
        if (s->_value.length())
        {
            editBoxImpl->insertText(s);
        }
        s->release();
    }
    _markedTextRange.location = NSNotFound;
}

- (UITextPosition *)beginningOfDocument
{
    return [HLIndexedPosition positionWithIndex:0];
}

- (UITextPosition *)endOfDocument
{
    return [HLIndexedPosition positionWithIndex:_text.length];
}

- (UITextRange *)textRangeFromPosition:(UITextPosition *)fromPosition toPosition:(UITextPosition *)toPosition
{
    return nil;
}

- (UITextPosition *)positionFromPosition:(UITextPosition *)position offset:(NSInteger)offset
{
    return nil;
}

- (UITextPosition *)positionFromPosition:(UITextPosition *)position inDirection:(UITextLayoutDirection)direction offset:(NSInteger)offset
{
    return nil;
}

- (NSComparisonResult)comparePosition:(UITextPosition *)position toPosition:(UITextPosition *)other
{
    return NSOrderedSame;
}

- (NSInteger)offsetFromPosition:(UITextPosition *)from toPosition:(UITextPosition *)toPosition
{
    return 0;
}

- (id <UITextInputTokenizer>) tokenizer
{
    return _tokenizer;
}

- (UITextPosition *)positionWithinRange:(UITextRange *)range farthestInDirection:(UITextLayoutDirection)direction
{
    return nil;
}

- (UITextRange *)characterRangeByExtendingPosition:(UITextPosition *)position inDirection:(UITextLayoutDirection)direction
{
    return nil;
}

- (UITextWritingDirection)baseWritingDirectionForPosition:(UITextPosition *)position inDirection:(UITextStorageDirection)direction
{
    return UITextWritingDirectionLeftToRight;
}

- (void)setBaseWritingDirection:(UITextWritingDirection)writingDirection forRange:(UITextRange *)range
{
    
}

- (CGRect)firstRectForRange:(UITextRange *)range
{
    return CGRectMake(0,0,50,20);
}

- (CGRect)caretRectForPosition:(UITextPosition *)position
{
    return CGRectMake(0,0,50,20);
}

- (NSArray *)selectionRectsForRange:(UITextRange *)range
{
    return nil;
}

- (UITextPosition *)closestPositionToPoint:(CGPoint)point
{
    return nil;
}

- (UITextPosition *)closestPositionToPoint:(CGPoint)point withinRange:(UITextRange *)range
{
    return nil;
}

- (UITextRange *)characterRangeAtPoint:(CGPoint)point
{
    return nil;
}

- (NSDictionary *)textStylingAtPosition:(UITextPosition *)position inDirection:(UITextStorageDirection)direction
{
    // This sample assumes all text is single-styled, so this is easy.
    return @{ UITextInputTextFontKey : [UIFont systemFontOfSize:10] };
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
