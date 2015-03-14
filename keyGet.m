//
//  keyGet.m
//  MadMonk
//
//  Created by bill on 2/5/11.
//  Copyright 2011 _LoudSoftware_. All rights reserved.
//

#import "keyGet.h"

@implementation keyGet

- (void)buildKeyMapping {
	keyMapping = [[NSMutableDictionary alloc] initWithCapacity:9];
	[keyMapping setObject:[NSNumber numberWithFloat:466.7] forKey:@"c"];
	[keyMapping setObject:[NSNumber numberWithFloat:494.4] forKey:@"d"];
	[keyMapping setObject:[NSNumber numberWithFloat:523.8] forKey:@"e"];
	[keyMapping setObject:[NSNumber numberWithFloat:415.8] forKey:@"a"];
	[keyMapping setObject:[NSNumber numberWithFloat:440.5] forKey:@"b"];
	
	frequencyMapping = [[NSMutableDictionary alloc] initWithCapacity:9];
	[frequencyMapping setObject:@"c" forKey:[NSNumber numberWithFloat:466.7]];
	[frequencyMapping setObject:@"d" forKey:[NSNumber numberWithFloat:494.4]];
	[frequencyMapping setObject:@"e" forKey:[NSNumber numberWithFloat:523.8]];
	[frequencyMapping setObject:@"a" forKey:[NSNumber numberWithFloat:415.8]];
	[frequencyMapping setObject:@"b" forKey:[NSNumber numberWithFloat:440.5]];
}

// Gets the character closest to the frequency passed in. 
- (NSString *)closestCharForFrequency:(float)frequency {
	NSString *closestKey = nil;
	float closestFloat = MAXFLOAT;	// Init to largest float value so all ranges closer.
	
	// Check each values distance to the actual frequency.
	for(NSNumber *num in [keyMapping allValues]) {
		float mappedFreq = [num floatValue];
		float tempVal = fabsf(mappedFreq-frequency);
		if (tempVal < closestFloat) {
			closestFloat = tempVal;
			closestKey = [frequencyMapping objectForKey:num];
		}
	}
	
	return closestKey;
}

@end
