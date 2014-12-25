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
     NES Controller actual size is 12.497 cm x 5.512 cm
     
                            |   ppi     cm/px       width       heigth      width       heigth      scale
                            |                       (pxs)       (pxs)       (pts)       (pts)       px/pts  fits?
     -------------------------------------------------------------------------------------------------------------
     iPhone 3.5" (480x320)  |   326     ~0,0077914   1603,95    707,45      802,00      353,5       2       NO
     iPhone 4"   (548x320)  |   326     ~0,0077914   1603,95    707,45      802,00      353,5       2       NO
     iPhone 6    (667x375)  |   326     ~0,0077914   1603,95    707,45      802,00      353,5       2       NO
     iPhone 6 Plus (640x540)|   401     ~0,0063341   1972,95    870,20      657,67      290,0       3       NO
     iPad 1 and 2 (1024x768)|   132     ~0,0192424   649,450    286,45      649,00      286,0       1       YES
     iPad retina (1024x768) |   264     ~0,0096212  1298,900    572,90      649,5       286,5       2       YES
     iPad mini (1024x768)   |   163     ~0,0155828   801,973    353,72      802,00      354,0       1       YES
     iPad mini retina       |   326     ~0,0077914   1603,95    707,45      802,00      353,5       2       YES
     (1024x768)
     */

    NSString * machineName = [self machineName];
 
    CGSize NESControllerSize = CGSizeZero;
    
    if ([self machineIsIPhone])
    {
        // No iPhone is big enough, so we return the biggest size possible
        // that stil has the correct ratio.
        // As width, we return the width of the main screen the heigth is
        // relative to that based on the NES Controllers actual size
        CGFloat width = [UIScreen mainScreen].bounds.size.width;
        CGFloat heigth = width / 1152.0f * 443;
        
        NESControllerSize = CGSizeMake(width, heigth);
    }
    else
    {
        // this is an iPad, figure out what type and return the size based
        // on the above table
        if ([self machineIsIPadMini])
        {
            // iPad mini
            NESControllerSize = CGSizeMake(802.0f, 353.50f);
        }
        else
        {
            // large iPad
            NESControllerSize = CGSizeMake(649.5f, 286.5f);
        }
    }
    
    // if we are not running on the simulatore, we should always have
    // a non-zero size.
    if ([machineName respondsToSelector: @selector(containsString:)])
    {
        // iOS8
        if (![machineName containsString: @"x86"])
        {
            NSAssert(!CGSizeEqualToSize(CGSizeZero, NESControllerSize),
                     @"No size for the controller found based on device type");
        }
    }
    else
    {
        // iOS7
        NSRange x86 = [machineName rangeOfString: @"x86"];
        if (x86.location == NSNotFound)
        {
            NSAssert(!CGSizeEqualToSize(CGSizeZero, NESControllerSize),
                     @"No size for the controller found based on device type");
        }
    }
    
    return NESControllerSize;
}
+ (BOOL) machineIsIPhone
{
    NSString * machineName = [self machineName];
    BOOL machineIsIPhone = NO;
    if ([machineName respondsToSelector: @selector(containsString:)])
    {
        // iOS8
        if ([machineName containsString: @"iPhone"] ||
            [machineName containsString: @"iPod"] ||
            [machineName containsString: @"x86"])
        {
            machineIsIPhone = YES;
        }
    }
    else
    {
        // iOS7
        NSRange iPhone = [machineName rangeOfString: @"iPad2,5"];
        NSRange iPod = [machineName rangeOfString: @"iPad4,4"];
        NSRange x86 = [machineName rangeOfString: @"iPad4,5"];
        if (iPhone.location != NSNotFound ||
            iPod.location   != NSNotFound ||
            x86.location    != NSNotFound)
        {
            machineIsIPhone = YES;
        }
    }
    return machineIsIPhone;
}
+ (BOOL) machineIsIPadMini
{
    NSString * machineName = [self machineName];
    BOOL machineIsIPadMini = NO;
    
    if ([machineName respondsToSelector: @selector(containsString:)])
    {
        // iOS8
        if ([machineName containsString: @"iPad2,5"] ||
            [machineName containsString: @"iPad4,4"] ||
            [machineName containsString: @"iPad4,5"])
        {
            machineIsIPadMini = YES;
        }
    }
    else
    {
        // iOS7
        NSRange iPad25 = [machineName rangeOfString: @"iPad2,5"];
        NSRange iPad44 = [machineName rangeOfString: @"iPad4,4"];
        NSRange iPad45 = [machineName rangeOfString: @"iPad4,5"];
        if (iPad25.location != NSNotFound ||
            iPad44.location != NSNotFound ||
            iPad45.location != NSNotFound)
        {
            machineIsIPadMini = YES;
        }
    }
    
    return machineIsIPadMini;
}
+ (BOOL) machineIsIPad
{
    BOOL machineIsIPad = NO;
    
    if (![self machineIsIPadMini] &&
        ![self machineIsIPhone])
    {
        machineIsIPad = YES;
    }
    return machineIsIPad;
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
