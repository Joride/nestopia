//
//  UIDevice+MachineType.m
//  Controller
//
//  Created by Joride on 20/12/14.
//

#import "UIDevice+MachineType.h"

@implementation UIDevice (MachineType)

#pragma mark - Public
+ (CGSize)NESControllerSize
{
    /*
     - ppi is the pixels per inch for a display
     - mm / px is the size an individual pixel
     - width (pts) is the number of points required for a real-sized controller
     - heigth (pts) is the number of points required for a real-sized controller
     - width (pxs) is the number of pixels required for a real-sized controller
     - heigth (pxs) is the number of pixels required for a real-sized controller
     
     |   ppi     mm/px   width	heigth  width   heigth
     |                   (pts)   (pts)   (pxs)   (pxs)
     ---------------------------------------------------------------------------
     iPhone 4 through 6 &   |
     iPad mini retina       |   326     ~0,0077	802,5   354     1605	708
     iPhone 6 Plus          |   401     ~0,0063 987     435,5	1974    871
     iPad 1 and 2           |   132     ~0,0192	325     143,5	650     287
     iPad retina            |   264     ~0,0096	650     286,5	1300	573
     iPad mini              |   163     ~0,0155	401,5	177     803     354
     */

    NSString * machineName = [self machineName];
 
    CGSize NESControllerSize = CGSizeZero;
    
    if ([machineName containsString: @"iPhone"] ||
        [machineName containsString: @"iPod"] ||
        [machineName containsString: @"x86"])
    {
        // as width, we return the width of the main screen
        // the heigth is relative to that based on the
        // NES Controllers actual size
        // 12.497 cm / 5.512 cm
        CGFloat width = [UIScreen mainScreen].bounds.size.width;
        CGFloat heigth = width / 1152.0f * 443;
        
        NESControllerSize = CGSizeMake(width, heigth);
    }
    else
    {
        // this is an iPad, figure out what type and return the size based
        // on the above table
        if ([machineName containsString: @"iPad2,5"] ||
            [machineName containsString: @"iPad4,4"] ||
            [machineName containsString: @"iPad4,5"])
        {
            // iPad mini
            // 803 * 354 pts
            NESControllerSize = CGSizeMake(803.0f, 286.5f);
        }
        else
        {
            // large iPad
            // 650 * 286.5 pts
            NESControllerSize = CGSizeMake(650.0f, 286.5f);
            
            
        }
        
    }
    
    if (![machineName containsString: @"x86"])
    {
        NSAssert(!CGSizeEqualToSize(CGSizeZero, NESControllerSize),
                 @"No size for the controller found based on device type");
    }
    
    return NESControllerSize;
}

+(NSString *)machineName
{
    NSString *machineName;
    /*
     possible return values:
     
     // simulator
     @"i386"        on 32-bit Simulator
     @"x86_64"      on 64-bit Simulator
     
     // iPod Touch
     @"iPod1,1"     on iPod Touch
     @"iPod2,1"     on iPod Touch Second Generation
     @"iPod3,1"     on iPod Touch Third Generation
     @"iPod4,1"     on iPod Touch Fourth Generation
     
     // iPhone
     @"iPhone1,1"   on iPhone
     @"iPhone1,2"   on iPhone 3G
     @"iPhone2,1"   on iPhone 3GS
     @"iPhone3,1"   on iPhone 4
     @"iPhone4,1"   on iPhone 4S
     @"iPhone5,1"   on iPhone 5 (model A1428, AT&T/Canada)
     @"iPhone5,2"   on iPhone 5 (model A1429, everything else)
     @"iPhone7,1"   on iPhone 6 Plus
     @"iPhone7,2"   on iPhone 6
     @"iPhone5,3"   on iPhone 5c (model A1456, A1532 | GSM)
     @"iPhone5,4"   on iPhone 5c (model A1507, A1516, A1526 (China), A1529 | Global)
     @"iPhone6,1"   on iPhone 5s (model A1433, A1533 | GSM)
     @"iPhone6,2"   on iPhone 5s (model A1457, A1518, A1528 (China), A1530 | Global)
     
     // iPad 9.7"
     @"iPad1,1"     on iPad
     @"iPad2,1"     on iPad 2
     @"iPad3,1"     on 3rd Generation iPad
     @"iPad3,4"     on 4th Generation iPad
     @"iPad4,1"     on 5th Generation iPad (iPad Air) - Wifi
     @"iPad4,2"     on 5th Generation iPad (iPad Air) - Cellular

     // iPad 7.9"
     @"iPad2,5"     on iPad Mini
     @"iPad4,4"     on 2nd Generation iPad Mini - Wifi
     @"iPad4,5"     on 2nd Generation iPad Mini - Cellular
     
     */
    struct utsname systemInfo;
    uname(&systemInfo);
    
    machineName = [NSString stringWithCString: systemInfo.machine
                                     encoding: NSUTF8StringEncoding];
    return machineName;
}
@end
