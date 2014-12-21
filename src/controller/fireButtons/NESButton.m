//
//  NESButton.m
//  D-pad
//
//  Created by Joride on 14/12/14.
//

#import "NESButton.h"

typedef NS_ENUM(NSInteger, NESButtonState)
{
    kNESButtonStateReady,
    kNESButtonStateBegan,
    kNESButtonStateEnded
};

@interface NESButton ()
@property (nonatomic) NESButtonState state;
@property (nonatomic, strong) UILabel * titleLabel;
@property (nonatomic, strong) UIImageView * backgroundImageView;

@property (nonatomic, copy) NSString * pressedString;
@property (nonatomic, copy) NSString * unpressedString;
@property (nonatomic, strong) UIImage * pressedImage;
@property (nonatomic, strong) UIImage * unpressedImage;
@property (nonatomic, strong) UIColor * unpressedTitleColor;
@property (nonatomic, strong) UIColor * pressedTitleColor;

@property (nonatomic, strong) NSArray * currentConstraints;
@end

@implementation NESButton
+(BOOL)requiresConstraintBasedLayout
{
    return YES;
}
-(instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame: frame];
    if (self)
    {
        [self commonInnit];
    }
    return self;
}
- (void) commonInnit
{
    self.backgroundColor = [UIColor blueColor];
    _state = kNESButtonStateReady;
    self.userInteractionEnabled = YES;
    _titleLabel = [[UILabel alloc] initWithFrame: CGRectZero];
    _titleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    _titleLabel.textAlignment = NSTextAlignmentCenter;
    [self addSubview: _titleLabel];
    
    _backgroundImageView = [[UIImageView alloc] initWithImage: nil];
    _backgroundImageView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview: _backgroundImageView];
}
-(void)updateLabelAndImageForCurrentState
{
    NSString * title = nil;
    UIImage * image = nil;
    UIColor * color = nil;
    
    // set the title and image
    switch (self.state)
    {
        case kNESButtonStateReady:
            title = self.unpressedString;
            image = self.unpressedImage;
            if (self.pressedTitleColor != nil)
            {
                color = self.unpressedTitleColor;
            }
            break;
        case kNESButtonStateBegan:
            title = self.pressedString;
            image = self.pressedImage;
            if (self.pressedTitleColor != nil)
            {
                color = self.pressedTitleColor;
            }
            break;
        case kNESButtonStateEnded:
            title = self.unpressedString;
            image = self.unpressedImage;
            if (self.pressedTitleColor != nil)
            {
                color = self.unpressedTitleColor;
            }
            break;
    }
    self.titleLabel.text = title;
    self.titleLabel.textColor = color;
    self.backgroundImageView.image = image;
    
    [self setNeedsLayout];
}
-(CGSize)intrinsicContentSize
{
    CGSize titleSize = [self.titleLabel sizeThatFits:
                        CGSizeMake(CGFLOAT_MAX,
                                   CGFLOAT_MAX)];
    CGSize imageSize = [self.backgroundImageView sizeThatFits:
                        CGSizeMake(CGFLOAT_MAX,
                                   CGFLOAT_MAX)];
    
    CGSize intrinsicContentSize = [super intrinsicContentSize];
    
    if (nil != self.backgroundImageView.image ||
        0   != self.titleLabel.text.length)
    {
        CGFloat width = (titleSize.width > imageSize.width) ? titleSize.width :
                                                                imageSize.width;
        CGFloat height = (titleSize.height > imageSize.height) ? titleSize.height :
                                                                imageSize.height;
        
        intrinsicContentSize = CGSizeMake(width, height);
    }
    return intrinsicContentSize;
}
- (void) setPressedImage: (UIImage *) image
{
    _pressedImage = image;
    [self updateLabelAndImageForCurrentState];
}
- (void) setUnpressedImage: (UIImage *) image
{
    _unpressedImage = image;
    [self updateLabelAndImageForCurrentState];
}
- (void) setPressedTitle: (NSString *) title
{
    _pressedString = [title copy];
    [self updateLabelAndImageForCurrentState];
}
- (void) setUnpressedTitle: (NSString *) title
{
    _unpressedString = [title copy];
    [self updateLabelAndImageForCurrentState];
}
- (void) setPressedTitleColor: (UIColor *) color
{
    _pressedTitleColor = color;
    [self updateLabelAndImageForCurrentState];
}
- (void) setUnpressedTitleColor: (UIColor *) color
{
    _unpressedTitleColor = color;
    [self updateLabelAndImageForCurrentState];
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
- (NSArray *) makeConstraints
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
 
    if (nil != _backgroundImageView)
    {
        [constraints addObject:
         [NSLayoutConstraint constraintWithItem: _backgroundImageView
                                      attribute: NSLayoutAttributeCenterY
                                      relatedBy: NSLayoutRelationEqual
                                         toItem: self
                                      attribute: NSLayoutAttributeCenterY
                                     multiplier: 1.0f
                                       constant: 0.0f]];
        
        [constraints addObject:
         [NSLayoutConstraint constraintWithItem: _backgroundImageView
                                      attribute: NSLayoutAttributeCenterX
                                      relatedBy: NSLayoutRelationEqual
                                         toItem: self
                                      attribute: NSLayoutAttributeCenterX
                                     multiplier: 1.0f
                                       constant: 0.0f]];
    }
    if (nil != _titleLabel)
    {
        [constraints addObject:
         [NSLayoutConstraint constraintWithItem: _titleLabel
                                      attribute: NSLayoutAttributeCenterY
                                      relatedBy: NSLayoutRelationEqual
                                         toItem: self
                                      attribute: NSLayoutAttributeCenterY
                                     multiplier: 1.0f
                                       constant: 0.0f]];
        [constraints addObject:
         [NSLayoutConstraint constraintWithItem: _titleLabel
                                      attribute: NSLayoutAttributeCenterX
                                      relatedBy: NSLayoutRelationEqual
                                         toItem: self
                                      attribute: NSLayoutAttributeCenterX
                                     multiplier: 1.0f
                                       constant: 0.0f]];
    }
    
    return constraints;
}

#pragma mark - Touch handling
-(void)touchesBegan:(NSSet *)touches
          withEvent:(UIEvent *)event
{
    _state = kNESButtonStateBegan;
    [self setNeedsLayout];
    if ([self.delegate respondsToSelector: @selector(buttonWasPressed:)])
    {
        [self.delegate buttonWasPressed: self];
    }
    [self updateLabelAndImageForCurrentState];
}
-(void)touchesMoved:(NSSet *)touches
          withEvent:(UIEvent *)event
{
    // nothing changes, this button does not respond to a moving touch,
    // even if the touch moves outside the bounds.
}
-(void)touchesEnded:(NSSet *)touches
          withEvent:(UIEvent *)event
{
    _state = kNESButtonStateEnded;
    [self setNeedsLayout];
    if ([self.delegate respondsToSelector: @selector(buttonWasDepressed:)])
    {
        [self.delegate buttonWasDepressed: self];
    }
    [self updateLabelAndImageForCurrentState];
}
-(void)touchesCancelled:(NSSet *)touches
              withEvent:(UIEvent *)event
{
    _state = kNESButtonStateEnded;
    [self setNeedsLayout];
    if ([self.delegate respondsToSelector: @selector(buttonWasDepressed:)])
    {
        [self.delegate buttonWasDepressed: self];
    }
    [self updateLabelAndImageForCurrentState];
}

@end
