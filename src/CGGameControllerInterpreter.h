//
//  SSStratusGameController.h
//  Nestopia
//
//  Created by Joride on 22/12/14.
//
//

@import Foundation;
@import GameController;
#import "NESControllerOutput.h"

@interface CGGameControllerInterpreter : NSObject
<NESControllerOutput>
-(instancetype)initWithController: (GCController *) controller;
@end
