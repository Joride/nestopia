//
//  CGSize+Utilities.m
//  Nestopia
//
//  Created by Jorrit van Asselt on 11/12/14.
//
//

#import "CGSize+Utilities.h"

CGSize CGSizeScaled(CGSize size, CGFloat factor)
{
    return CGSizeMake(size.width * factor,
                      size.height * factor);
}

CGSize CGSizeScaledFloorf(CGSize size, CGFloat factor)
{
    return CGSizeMake(floorf(size.width * factor),
                      floorf(size.height * factor));
}
CGSize CGSizeScaledCeilf(CGSize size, CGFloat factor)
{
    return CGSizeMake(ceilf(size.width * factor),
                      ceilf(size.height * factor));
}