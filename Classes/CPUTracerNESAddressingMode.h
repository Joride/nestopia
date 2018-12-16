//
//  CPUTracerNESAddressingMode.h
//  Nestopia
//
//  Created by Jorrit van Asselt on 15/12/2018.
//

#ifndef CPUTracerNESAddressingMode_h
#define CPUTracerNESAddressingMode_h


// https://www.masswerk.at/6502/6502_instruction_set.html
// https://wiki.nesdev.com/w/index.php/CPU_addressing_modes
typedef enum CPUTracerNESAddressingMode
{
    CPUTracerNESAddressingModeUnknown = 0, // signal uninitialized value, should never actually be set as such
    CPUTracerNESAddressingModeNotApplicable,
    CPUTracerNESAddressingModeImplied,
    CPUTracerNESAddressingModeAccumulator,
    CPUTracerNESAddressingModeImmediate,
    CPUTracerNESAddressingModeAbsolute,
    CPUTracerNESAddressingModeZeropage,
    CPUTracerNESAddressingModeIndirectAbsolute,
    CPUTracerNESAddressingModeAbsoluteIndexedX,
    CPUTracerNESAddressingModeAbsoluteIndexedY,
    CPUTracerNESAddressingModeZeroPageIndexedX,
    CPUTracerNESAddressingModeZeroPageIndexedY,
    CPUTracerNESAddressingModeIndexedIndirect,
    CPUTracerNESAddressingModeIndirectIndexed,
    CPUTracerNESAddressingModeRelative
} CPUTracerNESAddressingMode;

#endif /* CPUTracerNESAddressingMode_h */
