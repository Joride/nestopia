//
//  CPUTracer.m
//  Nestopia
//
//  Created by Jorrit van Asselt on 15/12/2018.
//

#import "CPUTracer.h"

NSDictionary * NESAddressingModeNameByMode();

@interface CPUTracer ()
@property (nonnull, nonatomic, readonly) NSMutableString * log;
@property (nonatomic) dispatch_queue_t queue;
@property (nonatomic) NSInteger saveCounter;
@property (nonatomic, readonly) NSDictionary<NSNumber *, NSString *> * NESAddressingModeNameByMode;
@property (nonatomic, readonly) NSURL * fileURL;

// per-cycle values
@property (nonatomic, copy) NSString * opcodeName;
@property (nonatomic, copy) NSString * registersString;
@property (nonatomic) int64_t cycleCount;
@property (nonatomic) uint pc;
@property (nonatomic) uint a;
@property (nonatomic) uint x;
@property (nonatomic) uint y;
@property (nonatomic) uint p;
@property (nonatomic) uint sp;
@property (nonatomic) uint8_t * addressSpace;
@property (nonatomic) CPUTracerNESAddressingMode addressingMode;
@end

@implementation CPUTracer
{
    BOOL _didLogAddressSpace;
    BOOL _waitingForEndCycleCall;
}

+ (instancetype) sharedTracer
{
    static dispatch_once_t token = 0;
    static id sharedTracer = nil;
    
    dispatch_once(&token, ^{
        sharedTracer = [[self alloc] init];
    });
    
    return sharedTracer;
}
-(void)setOpcodeName:(NSString *)opcodeName
{
    dispatch_async(self.queue, ^{
        if ((nil != _opcodeName) ||
            (!_waitingForEndCycleCall))
        { NSLog(@"%@", [self debugDescription]); }
        
        NSAssert(nil == _opcodeName,
                 @"Trying to overwrite another opcode name. Did you forget to call -endCycle?");
        NSAssert(_waitingForEndCycleCall, @"Calling -setOpcodeName: before calling -startUpcomingCycle::::::::");
        _opcodeName = [opcodeName copy];
    });
}
-(void)setAddressingMode:(CPUTracerNESAddressingMode)addressingMode
{
    dispatch_async(self.queue, ^{
        if (CPUTracerNESAddressingModeUnknown != _addressingMode)
        { NSLog(@"%@", [self debugDescription]); }
        NSAssert(CPUTracerNESAddressingModeUnknown == _addressingMode,
                 @"Trying to overwrite addressingMode. Did you forget to call -endCycle?");
        _addressingMode = addressingMode;
    });
}

-(instancetype)init
{
    self = [super init];
    if (self)
    {
        _waitingForEndCycleCall = NO;
        NSURL * directoryURL =
        [NSURL fileURLWithPath: @"/Users/Jorrit/iOS/EmuOutputCompare/output/"
                   isDirectory: YES];
        NSUInteger count =
        [[NSFileManager.defaultManager contentsOfDirectoryAtURL: directoryURL
                                     includingPropertiesForKeys: nil
                                                        options: nil
                                                          error: nil] count];
        
        NSString * fileName = [NSString stringWithFormat: @"nestopia%2lu.txt", (unsigned long)count];
        _fileURL = [directoryURL URLByAppendingPathComponent: fileName];
        
        _NESAddressingModeNameByMode = NESAddressingModeNameByMode();
        _didLogAddressSpace = NO;
        _saveCounter = 0;
        _queue = dispatch_queue_create("CPUTracer", DISPATCH_QUEUE_SERIAL);
        _log = [[NSMutableString alloc] initWithString: @"cycle\tpc\topcode\tmode\topargs\tname\targvs\tregisters"];
        [self prepareToStartCycle];
        
        
    }
    return self;
}
+ (NSString *) contentsOfAddressSpace: (uint8_t *) buffer
{
    NSMutableString * contentString = [[NSMutableString alloc] init];
    for (uint16_t index = 0; index < 0xFFFF; index++)
    {
        uint8_t value = buffer[index];
        NSString * indexAndValue =
        [NSString stringWithFormat: @"0x%04X\t0x%02X\n",
         index,
         value];
        [contentString appendString: indexAndValue];
    }
    return contentString;
}
- (BOOL) readyToStartCycle
{
    BOOL ready = YES;
    
    void(^checkReady)(BOOL) = ^(BOOL isReady)
    {
        if (!isReady)
        { NSLog(@"%@", [self debugDescription]); }
        NSAssert(isReady, @"Not ready to start a new cycle, not all info from pevious cycle has been cleared");
    };
    
    ready = (ready && (nil == self.opcodeName)); checkReady(ready);
    ready = (ready && (-1 == self.cycleCount)); checkReady(ready);
    ready = (ready && (-1 == self.pc)); checkReady(ready);
    ready = (ready && (-1 == self.a)); checkReady(ready);
    ready = (ready && (-1 == self.x)); checkReady(ready);
    ready = (ready && (-1 == self.y)); checkReady(ready);
    ready = (ready && (-1 == self.p )); checkReady(ready);
    ready = (ready && (-1 == self.sp)); checkReady(ready);
    ready = (ready && (NULL == self.addressSpace)); checkReady(ready);
    ready = (ready && (self.addressingMode == CPUTracerNESAddressingModeUnknown)); checkReady(ready);
    
    return ready;
}
- (BOOL) readyToEndCycle
{
    BOOL ready = YES;
    
    void(^checkReady)(BOOL) = ^(BOOL isReady)
    {
        if (!isReady) { NSLog(@"%@", [self debugDescription]); }
        NSAssert(isReady, @"Not ready to end a cycle, not all necessary info has been set.");
    };
    
    ready = (ready && (nil != self.opcodeName)); checkReady(ready);
    ready = (ready && (-1 != self.cycleCount)); checkReady(ready);
    ready = (ready && (-1 != self.pc)); checkReady(ready);
    ready = (ready && (-1 != self.a)); checkReady(ready);
    ready = (ready && (-1 != self.x)); checkReady(ready);
    ready = (ready && (-1 != self.y)); checkReady(ready);
    ready = (ready && (-1 != self.p)); checkReady(ready);
    ready = (ready && (-1 != self.sp)); checkReady(ready);
    ready = (ready && (NULL != self.addressSpace)); checkReady(ready);
    ready = (ready && (self.addressingMode != CPUTracerNESAddressingModeUnknown)); checkReady(ready);
    
    return ready;
}
- (void) startUpcomingCycle: (int64_t) cycleCount
                         pc: (uint) pc
                          a: (uint) a
                          x: (uint) x
                          y: (uint) y
                          p: (uint) p
                         sp: (uint) sp
               addressSpace: (uint8_t *) addressSpace
{
    dispatch_async(self.queue, ^{
        if (_waitingForEndCycleCall)
        {
            NSLog(@"Calling -startUpcomingCycle:::::::: while previous call has not been mathched with a endCycle yet.");
            NSLog(@"%@", [self debugDescription]);
            NSAssert(NO, @"Calling -startUpcomingCycle:::::::: while previous call has not been mathched with a endCycle yet.");
        }
        
        BOOL ready = [self readyToStartCycle];
        if (!ready) { NSLog(@"%@", [self debugDescription]); }
        NSAssert(ready, @"Not ready to start a new cycle");
        
        self.cycleCount = cycleCount;
        self.pc = pc;
        self.a = a;
        self.x = x;
        self.y = y;
        self.p = p;
        self.sp = sp;
        self.addressSpace = addressSpace;
        
        _waitingForEndCycleCall = YES;
    });
}
-(NSString *)debugDescription
{
    NSMutableString * description =
    [NSMutableString stringWithFormat: @"%@", [super debugDescription]];
    
    [description appendString: [NSString stringWithFormat: @"\nopcode:\t%@", self.opcodeName]];
    [description appendString: [NSString stringWithFormat: @"\ncycles:\t%@", @(self.cycleCount)]];
    [description appendString: [NSString stringWithFormat: @"\npc:\t%04X", self.pc]];
    [description appendString: [NSString stringWithFormat: @"\na:\t%02X", self.a]];
    [description appendString: [NSString stringWithFormat: @"\ny:\t%02X", self.y]];
    [description appendString: [NSString stringWithFormat: @"\nx:\t%02X", self.x]];
    [description appendString: [NSString stringWithFormat: @"\np:\t%02X", self.p]];
    [description appendString: [NSString stringWithFormat: @"\nsp:\t%02X", self.sp]];
    [description appendString: [NSString stringWithFormat: @"\naddressSpace:\t%s", self.addressSpace]];
    [description appendString: [NSString stringWithFormat: @"\nwaitingForEndCycleCall:\t%@",
                                     _waitingForEndCycleCall ? @"YES" : @"NO"]];
    return description;
}
- (void) endCycle
{
    dispatch_async(self.queue, ^{
        BOOL isReady = [self readyToEndCycle];
        if (!isReady)
        {
            NSLog(@"%@", [self debugDescription]);
            NSAssert(isReady, @"Not ready to end the current cycle");
        }
        
        /*
         cycle  pc      opcode  opargs  name    argvs   registers
         0      C000    4C      F5 C5   JMP     $C5F5   A:00 X:00 Y:00 P:24 SP:FD
         */
        
        uint8_t opcode = self.addressSpace[self.pc];
        if (nil == self.opcodeName)
        {
            NSString * opcodeString = [NSString stringWithFormat: @"op0x%02X", opcode];
            NSLog(@"%@", opcodeString);
            NSLog(@"%@", [self debugDescription]);
            NSAssert(false, @"No name for the opcode mentioned. Is it implemented correctly in the nstcpu file? Is it in the mapping dictionary in this file?");
        }
        
        if (CPUTracerNESAddressingModeUnknown == self.addressingMode)
        {
            NSString * opcodeString = [NSString stringWithFormat: @"op0x%02X", opcode];
            NSLog(@"No addressingmode for opcode: '%@'. THis is unexpected", opcodeString);
        }
        
        self.registersString =
        [NSString stringWithFormat: @"A:%02X X:%02X Y:%02X P:%02X SP:%02X",
         self.a,
         self.x,
         self.y,
         self.p,
         self.sp];
        
        NSString * addressingMode = self.NESAddressingModeNameByMode[@(self.addressingMode)];
        
        NSString * stringToAppend =
        [NSString stringWithFormat: @"\n%lld\t%04X\t%02X\t%@\t%@\t%@\t%@\t%@",
         self.cycleCount,
         self.pc,
         opcode,                // opcode value
         addressingMode,        // addressingMode
         @"-- --",              // opargs string
         self.opcodeName,       // opcode name (mnemonic)
         @"$----",              // opcode arguments string
         self.registersString]; // registers string
        
        if (!_didLogAddressSpace)
        {
            _didLogAddressSpace = YES;
//            [self.log appendString: [self contentsOfAddressSpace: addressSpace]];
        }
        
        [self.log appendString: stringToAppend];
        self.saveCounter += 1;
        
        if (self.saveCounter >= 1000)
        {
//            FILE * outFile = fopen("/Users/Jorrit/iOS/EmuOutputCompare/output/_nestopia.txt", "a");
            FILE * outFile = fopen(self.fileURL.path.UTF8String, "a");
            fprintf(outFile, "%s", [self.log UTF8String]);
            fclose(outFile);
            
            [self.log setString: @""];
            self.saveCounter = 0;
        }
        [self prepareToStartCycle];
        _waitingForEndCycleCall = NO;
    });
}

- (void) prepareToStartCycle
{
    _opcodeName = nil;
    
    self.cycleCount = -1;
    self.pc =  -1;
    self.a =  -1;
    self.x =  -1;
    self.y =  -1;
    self.p =  -1;
    self.sp =  -1;
    _addressingMode = CPUTracerNESAddressingModeUnknown;
    
    free(self.addressSpace);
    self.addressSpace = NULL;
}
@end


#pragma mark - C++ bridging functions
void CPUTracerStartUpcomingCycle
(int64_t cycles,
 uint pc,
 uint a,
 uint x,
 uint y,
 uint p,
 uint sp,
 uint8_t * addressSpace)
{
    [[CPUTracer sharedTracer] startUpcomingCycle: cycles
                                              pc: pc
                                               a: a
                                               x: x
                                               y: y
                                               p: p
                                              sp: sp
                                    addressSpace: addressSpace];
}
void CPUTracerEndCycle()
{
    [[CPUTracer sharedTracer] endCycle];
}

void CPUTracerCurrentOpcodeName(const char * opcodeName)
{
    [CPUTracer sharedTracer].opcodeName =
     [[NSString stringWithUTF8String: opcodeName] lowercaseString];
}
void CPUTracerSetCurrentAddressingMode(CPUTracerNESAddressingMode addressingMode)
{
    [CPUTracer sharedTracer].addressingMode = addressingMode;
}

#pragma mark -
static NSDictionary * __NESAddressingModeNameByMode = nil;
NSDictionary * NESAddressingModeNameByMode()
{
    __NESAddressingModeNameByMode =  @{
                                     @(CPUTracerNESAddressingModeUnknown) : @"unknown",
                                     @(CPUTracerNESAddressingModeNotApplicable) : @"n/a",
                                     @(CPUTracerNESAddressingModeImplied) : @"imp",
                                     @(CPUTracerNESAddressingModeAccumulator) : @"A",
                                     @(CPUTracerNESAddressingModeImmediate) : @"#v",
                                     @(CPUTracerNESAddressingModeAbsolute) : @"a",
                                     @(CPUTracerNESAddressingModeZeropage) : @"d",
                                     @(CPUTracerNESAddressingModeIndirectAbsolute) : @"(a)",
                                     @(CPUTracerNESAddressingModeAbsoluteIndexedX) : @"a,x",
                                     @(CPUTracerNESAddressingModeAbsoluteIndexedY) : @"a,y",
                                     @(CPUTracerNESAddressingModeZeroPageIndexedX) : @"d,x",
                                     @(CPUTracerNESAddressingModeZeroPageIndexedY) : @"d,y",
                                     @(CPUTracerNESAddressingModeIndexedIndirect) : @"(d,x)",
                                     @(CPUTracerNESAddressingModeIndirectIndexed) : @"(d),y",
                                     @(CPUTracerNESAddressingModeRelative) : @"rltv"
                                     };
    return __NESAddressingModeNameByMode;
}
