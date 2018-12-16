//
//  CPUTracer.h
//  Nestopia
//
//  Created by Jorrit van Asselt on 15/12/2018.
//

#import <Foundation/Foundation.h>
#import "CPUTracerNESAddressingMode.h"

NS_ASSUME_NONNULL_BEGIN

@interface CPUTracer : NSObject
+ (instancetype) sharedTracer;
- (void) startUpcomingCycle: (int64_t) cycleCount
                         pc: (uint) pc
                          a: (uint) a
                          x: (uint) x
                          y: (uint) y
                          p: (uint) reg
                         sp: (uint) sp
               addressSpace: (uint8_t *) addressSpace;
- (void) endCycle;
@end

NS_ASSUME_NONNULL_END

