//
//  NESController.m
//  D-pad
//
//  Created by Joride on 15/12/14.
//

#import "NESController.h"
#import "NESDirectionalButton.h"
#import "NESMultiButton.h"
#import "UIDevice+MachineType.h"


@interface NESController ()
<NESDirectionalButtonDelegate,
NESMultiButtonDelegate>

@property (nonatomic, readonly) UIButton * startButton;
@property (nonatomic, readonly) UIButton * selectButton;
@property (nonatomic, readonly) NESDirectionalButton * directionalButton;
@property (nonatomic, readonly) NESMultiButton * multiButton;
@property (nonatomic, readonly) UIImageView * AButtonImage;
@property (nonatomic, readonly) UIImageView * BButtonImage;

@property (nonatomic, readonly) UIImageView * nintendoLogoView;
@property (nonatomic, readonly) UIImageView * leftBackgroundImageView;
@property (nonatomic, readonly) UIImageView * rightBackgroundImageView;

@property (nonatomic, strong) NSArray * currentConstraints;
@end

@implementation NESController

+(BOOL)requiresConstraintBasedLayout
{
    return YES;
}
- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
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
    [self setContentCompressionResistancePriority: UILayoutPriorityRequired
                                          forAxis: UILayoutConstraintAxisHorizontal];
    [self setContentCompressionResistancePriority: UILayoutPriorityRequired
                                          forAxis: UILayoutConstraintAxisVertical];
    [self setContentHuggingPriority: UILayoutPriorityRequired
                            forAxis: UILayoutConstraintAxisHorizontal];
    [self setContentHuggingPriority: UILayoutPriorityRequired
                            forAxis: UILayoutConstraintAxisVertical];
    
    self.clipsToBounds = YES;
    self.translatesAutoresizingMaskIntoConstraints = NO;
    
    // backgroundImages
    UIImage * leftController = [UIImage imageNamed: @"NES controller left"];
    
    _leftBackgroundImageView = [[UIImageView alloc] initWithImage: leftController];
    _leftBackgroundImageView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview: _leftBackgroundImageView];
    
    UIImage * rightController = [UIImage imageNamed: @"NES controller right"];
    _rightBackgroundImageView = [[UIImageView alloc] initWithImage: rightController];
    _rightBackgroundImageView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview: _rightBackgroundImageView];
    
    // nintendo logo (291 PX from the bottom, 160 px from the left
    UIImage * nintendoLogo = [UIImage imageNamed:@"NES Nintendo logo"];
    _nintendoLogoView = [[UIImageView alloc] initWithImage: nintendoLogo];
    _nintendoLogoView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview: _nintendoLogoView];
    
    // A and B images
    UIImage * AButtonImage = [UIImage imageNamed: @"NES Controller A"];
    _AButtonImage = [[UIImageView alloc] initWithImage: AButtonImage];
    _AButtonImage.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview: _AButtonImage];
    
    UIImage * BButtonImage = [UIImage imageNamed: @"NES Controller B"];
    _BButtonImage = [[UIImageView alloc] initWithImage: BButtonImage];
    _BButtonImage.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview: _BButtonImage];
    
    UIImage * startSelectImage = [UIImage imageNamed: @"NES Start Select controller"];

    // start button
    _startButton = [UIButton buttonWithType: UIButtonTypeCustom];
    [_startButton addTarget: self
                     action: @selector(startButtonPressed:)
           forControlEvents: UIControlEventTouchDown];
    [_startButton addTarget: self
                     action: @selector(startButtonUnpressed:)
           forControlEvents: UIControlEventTouchUpInside];
    _startButton.translatesAutoresizingMaskIntoConstraints = NO;
    [_startButton setImage: startSelectImage
                  forState: UIControlStateNormal];
    [self addSubview: _startButton];
    
    // select button
    _selectButton = [UIButton buttonWithType: UIButtonTypeCustom];
    [_selectButton addTarget: self
                     action: @selector(selectButtonPressed:)
           forControlEvents: UIControlEventTouchDown];
    [_selectButton addTarget: self
                     action: @selector(selectButtonUnpressed:)
           forControlEvents: UIControlEventTouchUpInside];
    _selectButton.translatesAutoresizingMaskIntoConstraints = NO;
    [_selectButton setImage: startSelectImage
                  forState: UIControlStateNormal];
    [self addSubview: _selectButton];
    
    // d-pad button
    _directionalButton = [[NESDirectionalButton alloc]
                          initWithFrame: CGRectZero];
    _directionalButton.translatesAutoresizingMaskIntoConstraints = NO;
    _directionalButton.delegate = self;
    UIImage * dpadImage = [UIImage imageNamed: @"NES controller d-pad"];
    [_directionalButton setBackgroundImage: dpadImage];
    [self addSubview: _directionalButton];
    
    // firebuttons
    _multiButton = [[NESMultiButton alloc] initWithFrame: CGRectZero];
    _multiButton.translatesAutoresizingMaskIntoConstraints = NO;
    _multiButton.delegate = self;
    [self addSubview: _multiButton];
}

#pragma mark - Layout relate
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
-(CGSize)intrinsicContentSize
{
    /*
     size reflects:
     - without slicing: the size of the actual image in points
     - with vertical slicing on the width will be: in the slicing pane
     where you to slicing: left + right + width
     - with horizontal slicing on the heigth will be: in the slicing pane
     where you to slicing: top + bottom + heigth
     */
    /*
     - ppi is the pixels per inch for a display
     - mm / px is the size an individual pixel
     - width (pts) is the number of points required for a real-sized controller
     - heigth (pts) is the number of points required for a real-sized controller
     - width (pxs) is the number of pixels required for a real-sized controller
     - heigth (pxs) is the number of pixels required for a real-sized controller
     
                            |   ppi     mm/px   width	heigth  width   heigth
                            |                   (pts)   (pts)   (pxs)   (pxs)
     ---------------------------------------------------------------------------
     iPhone 4 through 6 &   |
     iPad mini retina       |   326     ~0,0077	802,5   354     1605	708
     iPhone 6 Plus          |   401     ~0,0063 987     435,5	1974    871
     iPad 1 and 2           |   132     ~0,0192	325     143,5	650     287
     iPad retina            |   264     ~0,0096	650     286,5	1300	573
     iPad mini              |   163     ~0,0155	401,5	177     803     354
     */
    
    // based on the above, we return a size that maintains the ration
    // width / length, but scaled down as neccessary.
    CGSize intrinsicContentSize = [UIDevice NESControllerSize];
    return intrinsicContentSize;
    

}
-(void)layoutSubviews
{
    [super layoutSubviews];
    
}
#pragma mark - Constraints
- (NSArray *) makeConstraints
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
    [constraints addObjectsFromArray: [self constraintsForLeftBackGroundImage]];
    [constraints addObjectsFromArray: [self constraintsForRightBackGroundImage]];
    [constraints addObjectsFromArray: [self constraintsForDirectionalButton]];
    [constraints addObjectsFromArray: [self constraintsForNintendoLogo]];
    [constraints addObjectsFromArray: [self constraintsForStartButton]];
    [constraints addObjectsFromArray: [self constraintsForSelectButton]];
    [constraints addObjectsFromArray: [self constraintsForMultiButton]];
    [constraints addObjectsFromArray: [self constraintsForAAndBLabel]];
     
     return constraints;
}
- (NSArray *) constraintsForLeftBackGroundImage
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
    
    // left backgroundImage
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.leftBackgroundImageView
                             attribute: NSLayoutAttributeLeft
                             relatedBy: NSLayoutRelationEqual
                             toItem: self
                             attribute: NSLayoutAttributeLeft
                             multiplier: 1.0f
                             constant: 0.0]];
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.leftBackgroundImageView
                             attribute: NSLayoutAttributeTop
                             relatedBy: NSLayoutRelationEqual
                             toItem: self
                             attribute: NSLayoutAttributeTop
                             multiplier: 1.0f
                             constant: 0.0]];
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.leftBackgroundImageView
                             attribute: NSLayoutAttributeBottom
                             relatedBy: NSLayoutRelationEqual
                             toItem: self
                             attribute: NSLayoutAttributeBottom
                             multiplier: 1.0f
                             constant: 0.0]];
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.leftBackgroundImageView
                             attribute: NSLayoutAttributeRight
                             relatedBy: NSLayoutRelationEqual
                             toItem: self
                             attribute: NSLayoutAttributeCenterX
                             multiplier: 1.0f
                             constant: 0.0]];
    return constraints;
}
- (NSArray *) constraintsForRightBackGroundImage
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
    
    // right backgroundImage
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.rightBackgroundImageView
                             attribute: NSLayoutAttributeLeft
                             relatedBy: NSLayoutRelationEqual
                             toItem: self
                             attribute: NSLayoutAttributeCenterX
                             multiplier: 1.0f
                             constant: 0.0]];
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.rightBackgroundImageView
                             attribute: NSLayoutAttributeTop
                             relatedBy: NSLayoutRelationEqual
                             toItem: self
                             attribute: NSLayoutAttributeTop
                             multiplier: 1.0f
                             constant: 0.0]];
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.rightBackgroundImageView
                             attribute: NSLayoutAttributeBottom
                             relatedBy: NSLayoutRelationEqual
                             toItem: self
                             attribute: NSLayoutAttributeBottom
                             multiplier: 1.0f
                             constant: 0.0]];
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.rightBackgroundImageView
                             attribute: NSLayoutAttributeRight
                             relatedBy: NSLayoutRelationEqual
                             toItem: self
                             attribute: NSLayoutAttributeRight
                             multiplier: 1.0f
                             constant: 0.0]];
    return constraints;
}
- (NSArray *) constraintsForDirectionalButton
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
    
    // directionalButton
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.directionalButton
                             attribute: NSLayoutAttributeLeft
                             relatedBy: NSLayoutRelationEqual
                             toItem: self.leftBackgroundImageView
                             attribute: NSLayoutAttributeLeft
                             multiplier: 1.0f / 2.50f
                             constant: 44.0]];
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.directionalButton
                             attribute: NSLayoutAttributeBottom
                             relatedBy: NSLayoutRelationEqual
                             toItem: self
                             attribute: NSLayoutAttributeBottom
                             multiplier: 1.0f
                             constant: -26.0f]];
    return constraints;
}
- (NSArray *) constraintsForNintendoLogo
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
    // nintendo-logo
    // this computation is derived from the actual size of the layout of the controller
    CGFloat verticalOffsetFromBottom = (-291.0f / self.nintendoLogoView.image.size.height) *
    self.nintendoLogoView.image.size.height /
    [UIScreen mainScreen].scale;
    verticalOffsetFromBottom = floorf(verticalOffsetFromBottom);
    
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.nintendoLogoView
                             attribute: NSLayoutAttributeCenterX
                             relatedBy: NSLayoutRelationEqual
                             toItem: self.multiButton
                             attribute: NSLayoutAttributeCenterX
                             multiplier: 1.0f
                             constant: 0.0f]];
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.nintendoLogoView
                             attribute: NSLayoutAttributeBottom
                             relatedBy: NSLayoutRelationEqual
                             toItem: self.rightBackgroundImageView
                             attribute: NSLayoutAttributeBottom
                             multiplier: 1.0f
                             constant: verticalOffsetFromBottom]];
    return constraints;
}
- (NSArray *) constraintsForStartButton
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
    
    // start button
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.startButton
                             attribute: NSLayoutAttributeLeft
                             relatedBy: NSLayoutRelationEqual
                             toItem: self.rightBackgroundImageView
                             attribute: NSLayoutAttributeLeft
                             multiplier: 1.0f
                             constant: 10.0f]];
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.startButton
                             attribute: NSLayoutAttributeBottom
                             relatedBy: NSLayoutRelationEqual
                             toItem: self.leftBackgroundImageView
                             attribute: NSLayoutAttributeBottom
                             multiplier: 1.0f
                             constant: -48.0f]];
    return constraints;
}
- (NSArray *) constraintsForSelectButton
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
    // select button
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.selectButton
                             attribute: NSLayoutAttributeRight
                             relatedBy: NSLayoutRelationEqual
                             toItem: self.leftBackgroundImageView
                             attribute: NSLayoutAttributeRight
                             multiplier: 1.0f
                             constant: -14.0f]];
    [constraints addObject: [NSLayoutConstraint
                             constraintWithItem: self.selectButton
                             attribute: NSLayoutAttributeBottom
                             relatedBy: NSLayoutRelationEqual
                             toItem: self.leftBackgroundImageView
                             attribute: NSLayoutAttributeBottom
                             multiplier: 1.0f
                             constant: -48.0f]];
    return constraints;
}
- (NSArray *) constraintsForMultiButton
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
    
    // A and B button
    [constraints addObject: [NSLayoutConstraint constraintWithItem: self.multiButton
                                                         attribute: NSLayoutAttributeRight
                                                         relatedBy: NSLayoutRelationEqual
                                                            toItem: self.rightBackgroundImageView
                                                         attribute: NSLayoutAttributeRight
                                                        multiplier: 1.0f
                                                          constant: -44.0]];
    [constraints addObject: [NSLayoutConstraint constraintWithItem: self.multiButton
                                                         attribute: NSLayoutAttributeBottom
                                                         relatedBy: NSLayoutRelationEqual
                                                            toItem: self.rightBackgroundImageView
                                                         attribute: NSLayoutAttributeBottom
                                                        multiplier: 1.0f
                                                          constant: -26.0f]];
    return constraints;
}
- (NSArray *) constraintsForAAndBLabel
{
    NSMutableArray * constraints = [[NSMutableArray alloc] init];
    
    // 'A' (below A button)
    UIView * aButton = [self.multiButton viewAtIndex: 1];
    CGFloat aButtonYOffset = (8.0f/26.0f) * self.BButtonImage.image.size.height / [UIScreen mainScreen].scale;
    [constraints addObject: [NSLayoutConstraint constraintWithItem: self.AButtonImage
                                                         attribute: NSLayoutAttributeTop
                                                         relatedBy: NSLayoutRelationEqual
                                                            toItem: self.multiButton
                                                         attribute: NSLayoutAttributeBottom
                                                        multiplier: 1.0f
                                                          constant: aButtonYOffset]];
    [constraints addObject: [NSLayoutConstraint constraintWithItem: self.AButtonImage
                                                         attribute: NSLayoutAttributeRight
                                                         relatedBy: NSLayoutRelationEqual
                                                            toItem: aButton
                                                         attribute: NSLayoutAttributeRight
                                                        multiplier: 1.0f
                                                          constant: 0.0f]];
    
    // 'A' (below A button)
    
    // 8/26 * (image.size.heigth * scale)
    UIView * bButton = [self.multiButton viewAtIndex: 0];
    CGFloat bButtonYOffset = (8.0f/26.0f) * self.BButtonImage.image.size.height / [UIScreen mainScreen].scale;
    bButtonYOffset = floorf(bButtonYOffset);
    [constraints addObject: [NSLayoutConstraint constraintWithItem: self.BButtonImage
                                                         attribute: NSLayoutAttributeTop
                                                         relatedBy: NSLayoutRelationEqual
                                                            toItem: self.multiButton
                                                         attribute: NSLayoutAttributeBottom
                                                        multiplier: 1.0f
                                                          constant: bButtonYOffset]];
    [constraints addObject: [NSLayoutConstraint constraintWithItem: self.BButtonImage
                                                         attribute: NSLayoutAttributeRight
                                                         relatedBy: NSLayoutRelationEqual
                                                            toItem: bButton
                                                         attribute: NSLayoutAttributeRight
                                                        multiplier: 1.0f
                                                          constant: 0.0f]];
    return constraints;
}

#pragma mark - NESDirectionalButtonDelegate
- (void) directionalButton: (NESDirectionalButton *) directionalButton
         didPressDirection: (NESDirection) direction
{
    if ([self.delegate respondsToSelector: @selector(controller:didPressDirectionOnDpad:)])
    {
        [self.delegate controller: self didPressDirectionOnDpad: direction];
    }
}
- (void) directionalButton: (NESDirectionalButton *) directionalButton
       didUnpressDirection: (NESDirection) direction
{
    if ([self.delegate respondsToSelector: @selector(controller:didReleaseDirectionOnDPad:)])
    {
        [self.delegate controller: self didReleaseDirectionOnDPad: direction];
    }
}

#pragma mark - Actions
- (void) startButtonPressed: (UIButton *) button
{
    if ([self.delegate respondsToSelector: @selector(controllerDidPressStartButton:)])
    {
        [self.delegate controllerDidPressStartButton: self];
    }
}
- (void) startButtonUnpressed: (UIButton *) button
{
    if ([self.delegate respondsToSelector: @selector(controllerDidReleaseStartButton:)])
    {
        [self.delegate controllerDidReleaseStartButton: self];
    }
}
- (void) selectButtonPressed: (UIButton *) button
{
    if ([self.delegate respondsToSelector: @selector(controllerDidPressSelectButton:)])
    {
        [self.delegate controllerDidPressSelectButton: self];
    }
}
- (void) selectButtonUnpressed: (UIButton *) button
{
    if ([self.delegate respondsToSelector: @selector(controllerDidReleaseSelectButton:)])
    {
        [self.delegate controllerDidReleaseSelectButton: self];
    }
}

#pragma mark - NESMultiButtonDelegate
- (void) multiButton: (NESMultiButton *) multiButton
buttonPressedAtIndex: (NSUInteger) index
{
    if (index == 0)
    {
        // left fire button == B
        if ([self.delegate respondsToSelector: @selector(controllerDidPressBButton:)])
        {
            [self.delegate controllerDidPressBButton: self];
        }
    }
    else if (index == 1)
    {
        if ([self.delegate respondsToSelector: @selector(controllerDidPressAButton:)])
        {
            // right button == A
            [self.delegate controllerDidPressAButton: self];
        }
    }
}
- (void) multiButton: (NESMultiButton *) multiButton buttonUnpressedAtIndex: (NSUInteger) index;
{
    if (index == 0)
    {
        // left fire button == B
        if ([self.delegate respondsToSelector: @selector(controllerDidReleaseBButton:)])
        {
            [self.delegate controllerDidReleaseBButton: self];
        }
    }
    else if (index == 1)
    {
        if ([self.delegate respondsToSelector: @selector(controllerDidReleaseAButton:)])
        {
            // right button == A
            [self.delegate controllerDidReleaseAButton: self];
        }
    }
}
- (NSString *) stringForDirection: (NESDirection) direction
{
#if DEBUG
    return [self.directionalButton stringForDirection: direction];
#else
    return @"Directionstring only available in debug";
#endif
}

@end
