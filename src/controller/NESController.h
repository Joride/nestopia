//
//  NESController.h
//  D-pad
//
//  Created by Joride on 15/12/14.
//

@import UIKit;


#import "NESControllerConstants.h"

@class NESController;
@protocol NESControllerDelegate <NSObject>

// fire buttons
- (void) controllerDidPressAButton: (NESController *) controller;
- (void) controllerDidPressBButton: (NESController *) controller;
- (void) controllerDidReleaseAButton: (NESController *) controller;
- (void) controllerDidReleaseBButton: (NESController *) controller;

// start and select buttons
- (void) controllerDidPressStartButton: (NESController *) controller;
- (void) controllerDidPressSelectButton: (NESController *) controller;
- (void) controllerDidReleaseStartButton: (NESController *) controller;
- (void) controllerDidReleaseSelectButton: (NESController *) controller;

// d-pad
- (void) controller: (NESController *) controller didPressDirectionOnDpad: (NESDirection) direction;
- (void) controller: (NESController *) controller didReleaseDirectionOnDPad: (NESDirection) direction;
@end




@interface NESController : UIView
// DEBUG ONLY
- (NSString *) stringForDirection: (NESDirection) direction;

@property (nonatomic, weak) id<NESControllerDelegate> delegate;
@end
