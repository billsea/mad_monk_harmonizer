//
//  keyGet.h
//  MadMonk
//
//  Created by bill on 2/5/11.
//  Copyright 2011 _LoudSoftware_. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface keyGet : NSObject {

	NSMutableDictionary *keyMapping;
	NSMutableDictionary *frequencyMapping;
}

#pragma mark Key Generation
- (void)buildKeyMapping;
- (NSString *)closestCharForFrequency:(float)frequency;


@end
