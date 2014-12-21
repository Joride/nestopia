//
//  NESControllerInterpreter.h
//  Nestopia
//
//  Created by Joride on 21/12/14.
//
//

@import Foundation;
#import "NestopiaCore.h"

@class NESController;

@interface NESControllerInterpreter : NSObject
-(instancetype)initWithController: (NESController *) controller;
@property (nonatomic, readonly) NestopiaPadInput input;

// debug
- (NSString *) controllerState;
@end
