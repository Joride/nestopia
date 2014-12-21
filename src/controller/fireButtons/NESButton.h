//
//  NESButton.h
//  D-pad
//
//  Created by Joride on 14/12/14.
//

@import UIKit;

@class NESButton;
@protocol NESButtonDelegate <NSObject>
- (void) buttonWasPressed: (NESButton *) button;
- (void) buttonWasDepressed: (NESButton *) button;
@end

@interface NESButton : UIView
@property (nonatomic, weak) id<NESButtonDelegate> delegate;
- (void) setPressedImage: (UIImage *) image;
- (void) setUnpressedImage: (UIImage *) image;
- (void) setPressedTitle: (NSString *) title;
- (void) setUnpressedTitle: (NSString *) title;
- (void) setPressedTitleColor: (UIColor *) color;
- (void) setUnpressedTitleColor: (UIColor *) color;
@end
