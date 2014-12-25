//
//  NESControllerOutput.h
//  Nestopia
//
//  Created by Joride on 22/12/14.
//
//

@import Foundation;
#import "NestopiaCore.h"

@protocol NESControllerOutput <NSObject>
@property (nonatomic, readonly) NestopiaPadInput signal;
@end
