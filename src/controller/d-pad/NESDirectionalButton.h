//
//  NESDirectionalButton.h
//  D-pad
//
//  Created by Joride on 15/12/14.
//

@import UIKit;

#import "NESControllerConstants.h"

@class NESDirectionalButton;
@protocol NESDirectionalButtonDelegate <NSObject>
- (void) directionalButton: (NESDirectionalButton *) directionalButton
         didPressDirection: (NESDirection) direction;
- (void) directionalButton: (NESDirectionalButton *) directionalButton
       didUnpressDirection: (NESDirection) direction;
@end

@interface NESDirectionalButton : UIView
- (void) setBackgroundImage: (UIImage *) image;
@property (nonatomic, readonly) NESDirection currentlyPressedDirection;
@property (nonatomic, weak) id<NESDirectionalButtonDelegate> delegate;

- (NSString *) stringForDirection: (NESDirection) direction;
@end
