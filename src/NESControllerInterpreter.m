//
//  NESControllerInterpreter.m
//  Nestopia
//
//  Created by Joride on 21/12/14.
//
//

#import "NESControllerInterpreter.h"
#import "NESController.h"

@interface NESControllerInterpreter ()
<NESControllerDelegate>
@property (nonatomic, strong) NESController * controller;
@end

@implementation NESControllerInterpreter
{
    uint32_t _inputA;
    uint32_t _inputB;
    uint32_t _inputSelect;
    uint32_t _inputStart;
    uint32_t _inputUp;
    uint32_t _inputDown;
    uint32_t _inputLeft;
    uint32_t _inputRight;
}
-(instancetype)initWithController: (NESController *) controller
{
    NSParameterAssert(controller);
    self = [super init];
    if (self)
    {
        _inputA = 0;
        _inputB = 0;
        _inputSelect = 0;
        _inputStart = 0;
        _inputUp = 0;
        _inputDown = 0;
        _inputLeft = 0;
        _inputRight = 0;
        
        _controller = controller;
        NSAssert(controller.delegate == nil, @"This controller already has a delegate, most likely you do not want it to be replaced.");
        
        _controller.delegate  = self;
    }
    return self;
}

-(NestopiaPadInput)input
{
    NestopiaPadInput input = 0;
    
    input |= _inputA;
    input |= _inputB;
    input |= _inputStart;
    input |= _inputSelect;
    input |= _inputUp;
    input |= _inputDown;
    input |= _inputLeft;
    input |= _inputRight;
    
    return input;
}

- (NSString *) controllerState
{
    NSString * state = [NSString
                        stringWithFormat: @"\nstart:\t%@\nselect:\t%@\nA:\t\t%@\nB:\t\t%@\nup:\t\t%@down:\t\t%@left:\t\t%@right:\t\t%@",
                        (_inputStart == NestopiaPadInputStart) ? @"1" : @"0",
                        (_inputSelect == NestopiaPadInputSelect) ? @"1" : @"0",
                        (_inputA == NestopiaPadInputA) ? @"1" : @"0",
                        (_inputB == NestopiaPadInputB) ? @"1" : @"0",
                        (_inputUp == NestopiaPadInputUp) ? @"1" : @"0",
                        (_inputDown == NestopiaPadInputDown) ? @"1" : @"0",
                        (_inputLeft == NestopiaPadInputLeft) ? @"1" : @"0",
                        (_inputRight == NestopiaPadInputRight) ? @"1" : @"0"];
    return state;
}

#pragma mark - NESControllerDelegate
// A-Buttton
- (void) controllerDidPressAButton: (NESController *) controller
{
    _inputA = NestopiaPadInputA;
}
- (void) controllerDidReleaseAButton: (NESController *) controller
{
    _inputA = 0;
}
// B-Buttton
- (void) controllerDidPressBButton: (NESController *) controller
{
    _inputB = NestopiaPadInputB;
}
- (void) controllerDidReleaseBButton: (NESController *) controller
{
    _inputB = 0;
}

// start button
- (void) controllerDidPressStartButton: (NESController *) controller
{
    _inputStart = NestopiaPadInputStart;
}
- (void) controllerDidReleaseStartButton: (NESController *) controller
{
    _inputStart = 0;
}
// select button
- (void) controllerDidPressSelectButton: (NESController *) controller
{
    _inputSelect = NestopiaPadInputSelect;
}

- (void) controllerDidReleaseSelectButton: (NESController *) controller
{
    _inputSelect = 0;
}

// d-pad
- (void) controller: (NESController *) controller didPressDirectionOnDpad: (NESDirection) direction
{
    switch (direction)
    {
        case kNESDirectionUp:
            _inputUp = NestopiaPadInputUp;
            break;
        case kNESDirectionLeft:
            _inputLeft = NestopiaPadInputLeft;
            break;
        case kNESDirectionDown:
            _inputDown = NestopiaPadInputDown;
            break;
        case kNESDirectionRight:
            _inputRight = NestopiaPadInputRight;
            break;
        case kNESDirectionUpLeft:
            _inputUp = NestopiaPadInputUp;
            _inputLeft = NestopiaPadInputLeft;
            break;
        case kNESDirectionDownLeft:
            _inputDown = NestopiaPadInputDown;
            _inputLeft = NestopiaPadInputLeft;
            break;
        case kNESDirectionDownRight:
            _inputDown = NestopiaPadInputDown;
            _inputRight = NestopiaPadInputRight;
            break;
        case kNESDirectionUpRight:
            _inputUp = NestopiaPadInputUp;
            _inputRight = NestopiaPadInputRight;
            break;
        default:
            NSAssert(NO, @"PROGRAMMING ERROR: the controller should never report an unexpected (= impossible) direction");
            break;
    }
    NSLog(@"%@, %@", NSStringFromSelector(_cmd),
          [controller stringForDirection: direction]);
}
- (void) controller: (NESController *) controller didReleaseDirectionOnDPad: (NESDirection) direction
{
    switch (direction)
    {
        case kNESDirectionUp:
            _inputUp = 0;
            break;
        case kNESDirectionLeft:
            _inputLeft = 0;
            break;
        case kNESDirectionDown:
            _inputDown = 0;
            break;
        case kNESDirectionRight:
            _inputRight = 0;
            break;
        case kNESDirectionUpLeft:
            _inputUp = 0;
            _inputLeft = 0;
            break;
        case kNESDirectionDownLeft:
            _inputDown = 0;
            _inputLeft = 0;
            break;
        case kNESDirectionDownRight:
            _inputDown = 0;
            _inputRight = 0;
            break;
        case kNESDirectionUpRight:
            _inputUp = 0;
            _inputRight = 0;
            break;
        default:
            NSAssert(NO, @"PROGRAMMING ERROR: the controller should never report an unexpected (= impossible) direction");
            break;
    }
    NSLog(@"%@, %@", NSStringFromSelector(_cmd),
          [controller stringForDirection: direction]);
}
@end
