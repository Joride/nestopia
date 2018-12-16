//
//  CPUTracer-Bridge.h
//  Nestopia
//
//  Created by Jorrit van Asselt on 15/12/2018.
//

#ifndef CPUTracer_Bridge_h
#define CPUTracer_Bridge_h

// These are the C "bridging" functions that will be used
// to invoke a specific Objective-C method FROM C++

#import "CPUTracerNESAddressingMode.h"

void CPUTracerStartUpcomingCycle
(int64_t cycles,
 uint pc,
 uint a,
 uint x,
 uint y,
 uint p,
 uint sp,
 uint8_t * addressSpace);


void CPUTracerEndCycle();
void CPUTracerCurrentOpcodeName(const char * opcodeName);
void CPUTracerSetCurrentAddressingMode(CPUTracerNESAddressingMode addressingMode);

#endif /* CPUTracer_Bridge_h */
