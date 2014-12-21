//
//  NESMultiButton.m
//  D-pad
//
//  Created by Joride on 13/12/14.
//

/*******************************************************************************
 
 1. The first button will receive the actual touchesBegan:
 2. This first button will get all the other touches..: methods too.
 3. Other buttons will get a touchesStarted: when touchesMoved first occured
    in their frame.
 4. Other buttons will get touchesMoved: when touchesMoved occured in their 
    frame and they already had a touchesBegan: method called.
 5. Other buttons will get touchesEnded: the first time the touchesMoved:
    occurs outside their frame.
 6. Other buttons that received touchesBegan: will alway get the 
    touchesCancelled: call if they are still active.
 
 ******************************************************************************/

#import "NESMultiButton.h"
#import "NESButton.h"

@interface NESMultiButton () <NESButtonDelegate>
@property (nonatomic, readonly) NESButton * leftButton;
@property (nonatomic, readonly) NESButton * rightButton;
@property (nonatomic, weak) UIView * firstTouchedView;
@property (nonatomic, readonly) NSMutableSet * subsequentViews;
@property (nonatomic, strong) NSArray * currentConstraints;
@end

@implementation NESMultiButton
+(BOOL)requiresConstraintBasedLayout
{
    return YES;
}
-(instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame: frame];
    if (self)
    {
        [self commonInit];
    }
    return self;
}
-(id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder: aDecoder];
    if (self)
    {
        [self commonInit];
    }
    return self;
}
- (void) commonInit
{
    self.clipsToBounds = NO;
    
    UIImage * fireButtonImage  = [UIImage imageNamed: @"NES firebutton"];
    
    _leftButton = [[NESButton alloc] initWithFrame: CGRectZero];
    _leftButton.translatesAutoresizingMaskIntoConstraints = NO;
    _leftButton.delegate = self;
    [_leftButton setPressedImage: fireButtonImage];
    [_leftButton setUnpressedImage: fireButtonImage];

    _rightButton = [[NESButton alloc] initWithFrame: CGRectZero];
    _rightButton.translatesAutoresizingMaskIntoConstraints = NO;
    _rightButton.delegate = self;
    [_rightButton setUnpressedImage: fireButtonImage];
    [_rightButton setPressedImage: fireButtonImage];
    
    [self addSubview: _rightButton];
    [self addSubview: _leftButton];
}
- (UIView *) viewAtIndex: (NSUInteger) index
{
    UIView * viewAtIndex = nil;
    if (index == 0)
    {
        viewAtIndex = self.leftButton;
    }
    else if (index == 1)
    {
        viewAtIndex = self.rightButton;
    }
    return viewAtIndex;
}
+ (CGFloat) horizontalPaddinBetweenButtons
{
    UIImage * fireButtonImage  = [UIImage imageNamed: @"NES firebutton"];
    
    // this is derived from the actual controller
    CGFloat paddingBetweenButtons = 0.28f * fireButtonImage.size.width;
    return paddingBetweenButtons;
}
-(CGSize)intrinsicContentSize
{
    UIImage * fireButtonImage  = [UIImage imageNamed: @"NES firebutton"];
    
    // this is derived from the actual controller
    CGFloat paddingBetweenButtons = [[self class]
                                     horizontalPaddinBetweenButtons];
    
    CGSize intrinsicContentSize = CGSizeMake(fireButtonImage.size.width * 2.0f +
                                                        paddingBetweenButtons,
                                             fireButtonImage.size.height);
    return intrinsicContentSize;
}
- (void) setNeedsUpdateConstraints
{
    if (self.currentConstraints)
    {
        [self removeConstraints: self.currentConstraints];
        self.currentConstraints = nil;
    }
    
    [super setNeedsUpdateConstraints];
}
-(void)updateConstraints
{
    if (self.currentConstraints == nil)
    {
        self.currentConstraints = [self makeConstraints];
        [self addConstraints: self.currentConstraints];
    }
    [super updateConstraints];
}

#pragma mark - Touch handling
-(UIView *)hitTest: (CGPoint)point
         withEvent: (UIEvent *)event
{
    if (CGRectContainsPoint(self.bounds, point))
    {
        return self;
    }
    return nil;
}

@synthesize subsequentViews = _subsequentViews;
-(NSMutableSet *)subsequentViews
{
    if (nil == _subsequentViews)
    {
        _subsequentViews = [[NSMutableSet alloc] init];
    }
    return _subsequentViews;
}
- (NSArray *) makeConstraints
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
    
    // place right
    [constraints addObject: [NSLayoutConstraint constraintWithItem: self.leftButton
                                                         attribute: NSLayoutAttributeLeft
                                                         relatedBy: NSLayoutRelationEqual
                                                            toItem: self
                                                         attribute: NSLayoutAttributeLeft
                                                        multiplier: 1.0f
                                                          constant: 0.0f]];
    
    [constraints addObject: [NSLayoutConstraint constraintWithItem: self.leftButton
                                                         attribute: NSLayoutAttributeCenterY
                                                         relatedBy: NSLayoutRelationEqual
                                                            toItem: self
                                                         attribute: NSLayoutAttributeCenterY
                                                        multiplier: 1.0f
                                                          constant: 0.0f]];
    // place left
    [constraints addObject: [NSLayoutConstraint constraintWithItem: self.rightButton
                                                         attribute: NSLayoutAttributeRight
                                                         relatedBy: NSLayoutRelationEqual
                                                            toItem: self
                                                         attribute: NSLayoutAttributeRight
                                                        multiplier: 1.0f
                                                          constant: 0.0f]];
    
    [constraints addObject: [NSLayoutConstraint constraintWithItem: self.rightButton
                                                         attribute: NSLayoutAttributeCenterY
                                                         relatedBy: NSLayoutRelationEqual
                                                            toItem: self
                                                         attribute: NSLayoutAttributeCenterY
                                                        multiplier: 1.0f
                                                          constant: 0.0f]];
    return constraints;
}

#pragma mark - Touch handling
- (void) setMultipleTouchEnabled:(BOOL)multipleTouchEnabled
{
    [super setMultipleTouchEnabled: NO];
}
- (void) clearState
{
    self.firstTouchedView = nil;
    [self.subsequentViews removeAllObjects];
}
-(void)touchesBegan:(NSSet *)touches
          withEvent:(UIEvent *)event
{
    UITouch * touch = [touches anyObject];
    CGPoint location = [touch locationInView: self];

    // figure out if there is a button below this point
    NESButton * touchedButton = nil;
    
    if (CGRectContainsPoint(self.leftButton.frame, location))
    {
        touchedButton = self.leftButton;
    }
    else if (CGRectContainsPoint(self.rightButton.frame, location))
    {
        touchedButton = self.rightButton;
    }
    
    if (nil != touchedButton)
    {
        self.firstTouchedView = touchedButton;
        [touchedButton touchesBegan: touches
                          withEvent: event];
    }
}
-(void)touchesMoved:(NSSet *)touches
          withEvent:(UIEvent *)event
{
    [self.firstTouchedView touchesMoved: touches
                              withEvent: event];
    
    UITouch * touch = [touches anyObject];
    CGPoint location = [touch locationInView: self];
    
    // figure out if there is a button below this point
    NESButton * touchedButton = nil;
    NESButton * otherButton = nil;
    
    if (CGRectContainsPoint(self.leftButton.frame, location))
    {
        touchedButton = self.leftButton;
        otherButton = self.rightButton;
        
    }
    else if (CGRectContainsPoint(self.rightButton.frame, location))
    {
        touchedButton = self.rightButton;
        otherButton = self.leftButton;
    }
    
    // the other button is not below the touch,
    // if it was present in the subsequent touches,
    // we remove it and send a touchesEnded:: method
    if (nil != otherButton &&
        [self.subsequentViews containsObject: otherButton])
    {
        // this button was active, but is not active anymore
        [self.subsequentViews removeObject: otherButton];
        [otherButton touchesEnded: touches
                    withEvent: event];
    }
    
    // if there is a touchedButton and that button is not the
    // firstTouchedButton, we figure out if we have to sent it a
    // touchesBegan:: or a touchesMoved:: message.
    if (nil != touchedButton &&
        touchedButton != self.firstTouchedView)
    {
        if ([self.subsequentViews containsObject: touchedButton])
        {
            // this button has already received a -touchesBegan::
            [touchedButton touchesMoved: touches
                              withEvent: event];
        }
        else
        {
            // this button has not yet received a -touchesBegan::
            [self.subsequentViews addObject: touchedButton];
            [touchedButton touchesBegan: touches
                              withEvent: event];
        }
    }
}
-(void)touchesEnded:(NSSet *)touches
          withEvent:(UIEvent *)event
{
    [self.firstTouchedView touchesEnded: touches
                              withEvent: event];
    
    for (NESButton * aButton in self.subsequentViews)
    {
        [aButton touchesEnded: touches
                    withEvent: event];
    }
    
    [self clearState];
}
-(void)touchesCancelled:(NSSet *)touches
              withEvent:(UIEvent *)event
{
    [self.firstTouchedView touchesCancelled: touches
                                  withEvent: event];
    for (NESButton * aButton in self.subsequentViews)
    {
        [aButton touchesCancelled: touches
                        withEvent: event];
    }
    
    [self clearState];
}
#pragma mark - NESButtonDelegate
- (void) buttonWasPressed: (NESButton *) button
{
    if ([self.delegate respondsToSelector: @selector(multiButton:buttonPressedAtIndex:)])
    {
        NSUInteger index = NSNotFound;
        if (button == self.leftButton)
        {
            index = 0;
            
        }
        else if (button == self.rightButton)
        {
            index = 1;
        }
        [self.delegate multiButton: self buttonPressedAtIndex: index];
    }
}
- (void) buttonWasDepressed: (NESButton *) button
{
    if ([self.delegate respondsToSelector: @selector(multiButton:buttonUnpressedAtIndex:)])
    {
        NSUInteger index = NSNotFound;
        if (button == self.leftButton)
        {
            index = 0;
            
        }
        else if (button == self.rightButton)
        {
            index = 1;
        }
        [self.delegate multiButton: self buttonUnpressedAtIndex: index];
    }
}
@end
