//
//  NESControllerInterpreter.h
//  Nestopia
//
//  Created by Joride on 21/12/14.
//
//

@import Foundation;
#import "NESControllerOutput.h"

@class NESController;

@interface NESControllerInterpreter : NSObject <NESControllerOutput>
-(instancetype)initWithController: (NESController *) controller;

// debug
- (NSString *) controllerState;
@end
