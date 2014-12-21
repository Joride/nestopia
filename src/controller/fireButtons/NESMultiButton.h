//
//  NESMultiButton.h
//  D-pad
//
//  Created by Joride on 13/12/14.
//

@import UIKit;

@class NESMultiButton;
@protocol NESMultiButtonDelegate <NSObject>
- (void) multiButton: (NESMultiButton *) multiButton buttonPressedAtIndex: (NSUInteger) index;
- (void) multiButton: (NESMultiButton *) multiButton buttonUnpressedAtIndex: (NSUInteger) index;
@end

@interface NESMultiButton : UIView

// use only for layout purposes of other elements
- (UIView *) viewAtIndex: (NSUInteger) index;
@property (nonatomic, weak) id<NESMultiButtonDelegate> delegate;
@end
