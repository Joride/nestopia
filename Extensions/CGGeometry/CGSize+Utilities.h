//
//  CGSize+Utilities.h
//  Nestopia
//
//  Created by Jorrit van Asselt on 11/12/14.
//
//

@import CoreGraphics;

/*!
 @function CGSize CGSizeScaled(CGSize size, CGFloat factor)
 Returns a new CGSize that is scaled with the factor. This function
 can return fractional sizes.
 This method makes no assumptions about its input, it simply performs 
 the math.
 @param size
 The size to be rescaled.
 @param factor
 The factor with wich the size will be scaled. Both size and width will be 
 multiplied by this factor.
 */
CGSize CGSizeScaled(CGSize size, CGFloat factor);

/*!
 @function CGSize CGSizeScaledFloor(CGSize size, CGFloat factor)
 Like CGSize CGSizeScaled(CGSize size, CGFloat factor), but floors
 the width and heigth the get integral values.
 @see CGSize CGSizeScaled(CGSize size, CGFloat factor)
 */
CGSize CGSizeScaledFloorf(CGSize size, CGFloat factor);

/*!
 @function CGSize CGSizeScaledCeil(CGSize size, CGFloat factor)
 Like CGSize CGSizeScaled(CGSize size, CGFloat factor), but ceils
 the width and heigth the get integral values.
 @see CGSize CGSizeScaled(CGSize size, CGFloat factor)
 */
CGSize CGSizeScaledCeilf(CGSize size, CGFloat factor);
