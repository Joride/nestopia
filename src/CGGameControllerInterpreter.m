//
//  SSStratusGameController.m
//  Nestopia
//
//  Created by Joride on 22/12/14.
//
//

#import "CGGameControllerInterpreter.h"


@interface CGGameControllerInterpreter ()
@property (nonatomic, strong) GCController * controller;
@property (nonatomic, getter= isPauseButtonPressed) BOOL pauseButtonPressed;
@end



@implementation CGGameControllerInterpreter
{
    BOOL _pauseButtonPressed;
}
-(instancetype)initWithController: (GCController *) controller
{
    self = [super init];
    if (self)
    {
        _controller = controller;
        __weak CGGameControllerInterpreter * weakSelf = self;
        _controller.controllerPausedHandler = ^(GCController * controller)
        {
            // set a flag, clear it when signal is called
            weakSelf.pauseButtonPressed = YES;
        };
    }
    return self;
}

-(NestopiaPadInput)signal
{
    NestopiaPadInput signal = 0;
    
    // switched, as layout on original NES had B on the left,
    // and Apple spec has B on the right
    uint8_t AButton = (self.controller.gamepad.buttonX.isPressed) ? NestopiaPadInputB : 0;
    uint8_t BButton = (self.controller.gamepad.buttonA.isPressed) ? NestopiaPadInputA : 0;
    uint8_t selectButton = (self.controller.gamepad.leftShoulder.isPressed) ? NestopiaPadInputSelect : 0;
    uint8_t left = (self.controller.gamepad.dpad.left.isPressed) ? NestopiaPadInputLeft : 0;
    uint8_t right = (self.controller.gamepad.dpad.right.isPressed) ? NestopiaPadInputRight : 0;
    uint8_t up = (self.controller.gamepad.dpad.up.isPressed) ? NestopiaPadInputUp : 0;
    uint8_t down = (self.controller.gamepad.dpad.down.isPressed) ? NestopiaPadInputDown: 0;
    
    uint8_t start = (self.isPauseButtonPressed) ? NestopiaPadInputStart: 0;
    self.pauseButtonPressed = NO;
    
    signal |= AButton;
    signal |= BButton;
    signal |= selectButton;
    signal |= left;
    signal |= right;
    signal |= up;
    signal |= down;
    signal |= start;
    
    return signal;
}

@end
