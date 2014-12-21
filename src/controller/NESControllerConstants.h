//
//  NSObject+NESControllerConstants.h
//  D-pad
//
//  Created by Joride on 15/12/14.
//

@import Foundation;

typedef NS_ENUM(NSInteger, NESDirection)
{
    kNESDirectionNone = 1,
    kNESDirectionRight,
    kNESDirectionUp,
    kNESDirectionUpRight,
    kNESDirectionUpLeft,
    kNESDirectionLeft,
    kNESDirectionDown,
    kNESDirectionDownRight,
    kNESDirectionDownLeft
};
