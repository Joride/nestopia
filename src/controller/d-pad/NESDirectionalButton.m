//
//  NESDirectionalButton.m
//  D-pad
//
//  Created by Joride on 15/12/14.
//

#import "NESDirectionalButton.h"

@interface NESDirectionalButton ()
@property (nonatomic, readwrite) NESDirection currentlyPressedDirection;
@property (nonatomic, strong) UIImageView * backgroundImageView;
@end

@implementation NESDirectionalButton
{
    UIImage * _backgroundImage;
    BOOL _touchesStarted;
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
-(instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame: frame];
    if (self)
    {
        [self commonInit];
    }
    return self;
}

- (void) commonInit
{
    _currentlyPressedDirection = kNESDirectionNone;
    self.userInteractionEnabled = YES;
    self.multipleTouchEnabled = NO;
}
- (void) setBackgroundImage:(UIImage *)image
{
    _backgroundImage = image;
    if (nil == self.backgroundImageView)
    {
        self.backgroundImageView = [[UIImageView alloc] initWithImage: nil];
        [self insertSubview: self.backgroundImageView
                    atIndex: 0];
    }
    
    self.backgroundImageView.image = _backgroundImage;
}
-(CGSize)sizeThatFits:(CGSize)size
{
    CGSize sizeThatFits = [super sizeThatFits: size];
    if (nil != _backgroundImage)
    {
        sizeThatFits = _backgroundImage.size;
    }
    return sizeThatFits;
}
-(CGSize)intrinsicContentSize
{
    CGSize intrinsicContentSize = [super intrinsicContentSize];
    
    if (nil != _backgroundImage)
    {
        intrinsicContentSize = _backgroundImage.size;
    }
    return intrinsicContentSize;
}
-(void)layoutSubviews
{
    [super layoutSubviews];
    
    [self.backgroundImageView sizeToFit];
    self.backgroundImageView.center = [self convertPoint: self.center
                                                fromView: self.superview];
}


- (void) directionChangedFrom: (NESDirection) oldDirection
                  toDirection: (NESDirection) newDirection
{
    if (oldDirection == newDirection)
    {
        NSAssert(NO, @"Calling this method with the same old and new direction will do nothing");
        return;
    }
    
    if (oldDirection != kNESDirectionNone &&
        [self.delegate respondsToSelector: @selector(directionalButton:didUnpressDirection:)])
    {
        [self.delegate directionalButton: self
                     didUnpressDirection: oldDirection];
    }
    
    if (newDirection != kNESDirectionNone &&
        [self.delegate respondsToSelector: @selector(directionalButton:didPressDirection:)])
    {
        [self.delegate directionalButton: self
                       didPressDirection: newDirection];
    }
}

#pragma mark TouchHandling
- (CGRect) hitTestRect
{
    // if the touches already started, we increase the size of the d-pad
    CGRect hitTestRect = self.bounds;
    
//    if (_touchesStarted)
//    {
//        hitTestRect = CGRectInset(self.bounds,
//                                  -self.bounds.size.width,
//                                  -self.bounds.size.height);
//    }
    return hitTestRect;
}
- (UIView *)hitTest:(CGPoint)point
          withEvent:(UIEvent *)event
{
    // if the touches already started, we increase the size of the d-pad
    CGRect hitTestRect = [self hitTestRect];
    
    if (CGRectContainsPoint(hitTestRect,
                            point))
    {
        return self;
    }
    return nil;

}
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    _touchesStarted = YES;
    UITouch * touch = [touches anyObject];
    CGPoint location = [touch locationInView: self];
    
    NESDirection newDirection = [self directionForPoint: location];
    
    // update the ivar before calling the delegate
    _currentlyPressedDirection = [self directionForPoint: location];
    
    // if we tapped the middle of the d-pad (no direction),
    // we do nothing
    if (_currentlyPressedDirection != kNESDirectionNone)
    {
        [self directionChangedFrom: kNESDirectionNone
                       toDirection: newDirection];
    }
}
-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch * touch = [touches anyObject];
    CGPoint location = [touch locationInView: self];
    
    NESDirection oldDirection = _currentlyPressedDirection;
    
    NESDirection newDirection = [self directionForPoint: location];
    
    // as long as the touch is ongoing, we do not report none-directions
    if (newDirection != kNESDirectionNone)
    {
        // update the ivar before calling the delegate
        _currentlyPressedDirection = [self directionForPoint: location];
        if (oldDirection != _currentlyPressedDirection)
        {
            [self directionChangedFrom: oldDirection
                           toDirection: _currentlyPressedDirection];
        }
    }
}
-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    NESDirection oldDirection = _currentlyPressedDirection;

    // update the ivar before calling the delegate
    _currentlyPressedDirection = kNESDirectionNone;
    if (oldDirection != _currentlyPressedDirection)
    {
        [self directionChangedFrom: oldDirection
                       toDirection: _currentlyPressedDirection];
    }
    _touchesStarted = NO;
}
-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    NESDirection oldDirection = _currentlyPressedDirection;
    
    // update the ivar before calling the delegate
    _currentlyPressedDirection = kNESDirectionNone;
    if (oldDirection != _currentlyPressedDirection)
    {
        [self directionChangedFrom: oldDirection
                       toDirection: _currentlyPressedDirection];
    }
    _touchesStarted = NO;
}

#pragma mark - Utilities
- (NESDirection) directionForPoint: (CGPoint) pointInSelf
{
    // if this point is outside self, we do nothing and return
    // the current direction
    if (!CGRectContainsPoint(self.bounds, pointInSelf))
        return _currentlyPressedDirection;
    
    
// TODO: since the hittestrect gets enlarged when touches began is called,
    // we need to figure out a different way to handle the directions
    // then simply dividing into 9 squares:
    // we need to mathematically draw the tic-tac-toe lines and use those
    // lines to figure out in which quadrant the touch is
    CGRect hitTestRect = [self hitTestRect];
    
    /*
     We divide the view up into 9 equal rectangles. Each rectangle
     represents one direction.
     */
    CGFloat unitWidth = hitTestRect.size.width / 3.0f;
    CGFloat unitHeigth = hitTestRect.size.height / 3.0f;
    
    // if all the next if-statements fail, we simple return
    // what the the direction is now
    NESDirection direction = _currentlyPressedDirection;
    
    /// Right
    CGRect middleRight = CGRectMake(hitTestRect.origin.x + 2.0 * unitWidth,
                                    hitTestRect.origin.x + unitHeigth,
                                    unitWidth,
                                    unitHeigth);
    if (CGRectContainsPoint(middleRight, pointInSelf))
    {
        return kNESDirectionRight;
    }
    
    /// Left
    CGRect middleLeft = CGRectMake(hitTestRect.origin.x,
                                   hitTestRect.origin.x + unitHeigth,
                                   unitWidth,
                                   unitHeigth);
    if (CGRectContainsPoint(middleLeft, pointInSelf))
    {
        return kNESDirectionLeft;
    }
    
    /// Up
    CGRect topMiddle = CGRectMake(hitTestRect.origin.x + unitWidth,
                                  hitTestRect.origin.x,
                                  unitWidth,
                                  unitHeigth);
    if (CGRectContainsPoint(topMiddle, pointInSelf))
    {
        return kNESDirectionUp;
    }

    /// Down
    CGRect bottomMiddle = CGRectMake(hitTestRect.origin.x + unitWidth,
                                     hitTestRect.origin.x + 2.0 * unitHeigth,
                                     unitWidth,
                                     unitHeigth);
    if (CGRectContainsPoint(bottomMiddle, pointInSelf))
    {
        return kNESDirectionDown;
    }
    
    /// UpRight
    CGRect topRight = CGRectMake(hitTestRect.origin.x + 2.0 * unitWidth,
                                 hitTestRect.origin.x,
                                 unitWidth,
                                 unitHeigth);
    if (CGRectContainsPoint(topRight, pointInSelf))
    {
        return kNESDirectionUpRight;
    }
    
    /// UpLeft
    CGRect topLeft = CGRectMake(hitTestRect.origin.x,
                                hitTestRect.origin.x,
                                unitWidth,
                                unitHeigth);
    
    if (CGRectContainsPoint(topLeft, pointInSelf))
    {
        return kNESDirectionUpLeft;
    }
    
    /// None
    CGRect middleMiddle = CGRectMake(hitTestRect.origin.x + unitWidth,
                                   hitTestRect.origin.x + unitHeigth,
                                   unitWidth,
                                   unitHeigth);
    if (CGRectContainsPoint(middleMiddle, pointInSelf))
    {
        return kNESDirectionNone;
    }
    
    /// DownLeft
    CGRect bottomLeft = CGRectMake(hitTestRect.origin.x,
                                    hitTestRect.origin.x + 2.0 * unitHeigth,
                                    unitWidth,
                                    unitHeigth);
    if (CGRectContainsPoint(bottomLeft, pointInSelf))
    {
        return kNESDirectionDownLeft;
    }
    
    /// DownRight
    CGRect bottomRight = CGRectMake(hitTestRect.origin.x +  2.0f * unitWidth,
                                     hitTestRect.origin.x + 2.0 * unitHeigth,
                                     unitWidth,
                                     unitHeigth);
    if (CGRectContainsPoint(bottomRight, pointInSelf))
    {
        return kNESDirectionDownRight;
    }
    
    NSAssert(NO, @"This point in the code should never be reached. Did we miss a case above here, did the touch move to some unexpected place (like outside self.bounds?");
    return direction;
}
- (NESDirection) cardinalDirectionForPoint: (CGPoint) pointInSelf
{
    /* the view can be considered to be split in four diagonal quadrants
     by two diagonal lines.
     These two lines can be easily mathematically described:
     Given:
     - A = top left corner of the view = x0, y0
     - B = bottom left corner of the view = x0, y1
     - C = bottom left corner of the view = x1, y1
     - D = top right corner of the view = x1, y0
     A (x0, y0)     D (x1, y0)
     +--------------+
     |              |
     |              |
     |              |
     |              |
     +--------------+
     B (x0, y1)     C (x1, y1)
     
     The line AC can be described as follows:
     y(x) = y0 + ((y1 - y0) / (x1 - x0)) * x
     
     The line BD can be described as follows:
     y(x) = y1 + ((y0 - y1) / (x1 - x0)) * x
     
     Using these mathematical descriptions, we figure out 
     if the point is located:
     - below both lines: down
     - above both lines: up
     - below AC, and above BD: left
     - above AC, and below BD: right
     
     !!!: Remember that in normal Euclidian mathematics the Y increases upwards,
     while in a UIView Y increases downwards.
     */
    
    NESDirection direction = kNESDirectionNone;
    
    // simplify the equations by getting the directionCoefficients first
    CGFloat directionCoefficientAC = 0.00f;
    directionCoefficientAC = self.bounds.size.height / self.bounds.size.width;
    CGFloat directionCoefficientBD = 0.00f;
    directionCoefficientBD = - (self.bounds.size.height / self.bounds.size.width);
    
    // YForXOnAC will be the YValue for the given x
    // that is on the diagonal line AC.
    CGFloat YForXOnAC = 0.00f;
    YForXOnAC = self.bounds.origin.y + (directionCoefficientAC * pointInSelf.x);

    // YForXOnBD will be the YValue for the given x
    // that is on the diagonal line BD.
    CGFloat YForXOnBD = 0.00f;
    YForXOnBD = (self.bounds.origin.y + self.bounds.size.height) +
                                        (directionCoefficientBD * pointInSelf.x);
    
    // for no particular reason, we include the position that is on the line
    // to be part of the direction that is on the counter-clockwise side of
    // the line
    
    
    // now we figure out where the given Y is positioned,
    // relative to the two diagonal lines.
    
    // is to the left?
    CGFloat Y = pointInSelf.y;
    if (Y >= YForXOnAC &&
        Y < YForXOnBD)
    {
        direction = kNESDirectionLeft;
    }

    // is it to the right?
    else if (Y <= YForXOnAC &&
             Y > YForXOnBD)
    {
        direction = kNESDirectionRight;
    }
    // is it down?
    else if (Y >= YForXOnBD &&
             Y > YForXOnAC)
    {
        direction = kNESDirectionDown;
    }
    // is it up?
    else if (Y <= YForXOnBD &&
             Y < YForXOnAC)
    {
        direction = kNESDirectionUp;
    }
    
    NSAssert(direction != kNESDirectionNone, @"No direction for a point, this should be impossible");
    
    return direction;
}

#pragma mark - Debug
- (NSString *) stringForDirection: (NESDirection) direction
{
    NSString * returnString = @"???";
    switch (direction)
    {
        case kNESDirectionNone:
            returnString = @"None";
            break;
        case kNESDirectionUp:
            returnString = @"Up";
            break;
        case kNESDirectionLeft:
            returnString = @"Left";
            break;
        case kNESDirectionDown:
            returnString = @"Down";
            break;
        case kNESDirectionRight:
            returnString = @"Right";
            break;
        case kNESDirectionUpLeft:
            returnString = @"UpLeft";
            break;
        case kNESDirectionDownLeft:
            returnString = @"DownLeft";
            break;
        case kNESDirectionDownRight:
            returnString = @"DownRight";
            break;
        case kNESDirectionUpRight:
            returnString = @"UpRight";
            break;
    }
    return returnString;
}


@end
