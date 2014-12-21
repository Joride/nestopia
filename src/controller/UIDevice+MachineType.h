//
//  UIDevice+MachineType.h
//  Controller
//
//  Created by Joride on 20/12/14.
//

@import UIKit;
#import <sys/utsname.h>

/*!
 @category UIDevice (NESController)
 This category exists to add functionality to
 UIDevice to find out which type of device we are on.
 Specifically useful for the NESController, a class that 
 aims to display a live-sized NES controller.
 */
@interface UIDevice (NESController)

+ (CGSize) NESControllerSize;
@end
