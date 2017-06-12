/*
*	File:		_LoudSoftware_MadMonk_CocoaView.m
*
*	Version:	1.0
*
*	Created:	11/17/10
*
*	Copyright:  Copyright © 2010 _LoudSoftware_, All Rights Reserved
*
*	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by
*Apple Computer, Inc. ("Apple") in
*				consideration of your agreement to the following
*terms, and your use, installation, modification
*				or redistribution of this Apple software
*constitutes acceptance of these terms.  If you do
*				not agree with these terms, please do not use,
*install, modify or redistribute this Apple
*				software.
*
*				In consideration of your agreement to abide by the
*following terms, and subject to these terms,
*				Apple grants you a personal, non-exclusive license,
*under Apple's copyrights in this
*				original Apple software (the "Apple Software"), to
*use, reproduce, modify and redistribute the
*				Apple Software, with or without modifications, in
*source and/or binary forms; provided that if you
*				redistribute the Apple Software in its entirety and
*without modifications, you must retain this
*				notice and the following text and disclaimers in
*all such redistributions of the Apple Software.
*				Neither the name, trademarks, service marks or
*logos of Apple Computer, Inc. may be used to
*				endorse or promote products derived from the Apple
*Software without specific prior written
*				permission from Apple.  Except as expressly stated
*in this notice, no other rights or
*				licenses, express or implied, are granted by Apple
*herein, including but not limited to any
*				patent rights that may be infringed by your
*derivative works or by other works in which the
*				Apple Software may be incorporated.
*
*				The Apple Software is provided by Apple on an "AS
*IS" basis.  APPLE MAKES NO WARRANTIES, EXPRESS OR
*				IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
*WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY
*				AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING THE
*APPLE SOFTWARE OR ITS USE AND OPERATION ALONE
*				OR IN COMBINATION WITH YOUR PRODUCTS.
*
*				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL,
*INDIRECT, INCIDENTAL OR CONSEQUENTIAL
*				DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
*OF SUBSTITUTE GOODS OR SERVICES; LOSS
*				OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
*ARISING IN ANY WAY OUT OF THE USE,
*				REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION OF
*THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
*				UNDER THEORY OF CONTRACT, TORT (INCLUDING
*NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN
*				IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF
*SUCH DAMAGE.
*
*/

#import "MadMonk_CocoaView.h"
enum {
	kParam_AutoMan=0,
	kParam_CleanLevel=1,
	kFilterParam_CutoffFrequency=2,
	kFilterParam_Resonance = 3,
	kParam_SourcePan = 4,
	kParam_Key=5,
	kParam_KeyType=6,
	kParam_MinorType=7,
	kParam_VoiceOneInterval = 8,
	kParam_VoiceOneAutoInterval = 9,
	kParam_VoiceOneOctave=10,
	kParam_VoiceOneLevel =11,
	kParam_VoiceOnePan = 12,
	kParam_VoiceTwoInterval = 13,
	kParam_VoiceTwoAutoInterval = 14,
	kParam_VoiceTwoOctave=15,
	kParam_VoiceTwoLevel=16,
	kParam_VoiceTwoPan = 17,
	kParam_SourceNote=18,
	kNumberOfParameters=19
};


#pragma mark ____ LISTENER CALLBACK DISPATCHER ____
void ParameterListenerDispatcher (void *inRefCon, void *inObject, const AudioUnitParameter *inParameter, Float32 inValue) {
	MadMonk_CocoaView *SELF = (MadMonk_CocoaView *)inRefCon;
    
		NSLog(@"inValue: %f\n",inValue);
	
	[SELF _parameterListener:inObject parameter:inParameter value:inValue];
	
}

NSString *MadMonk_GestureSliderMouseDownNotification = @"CAGestureSliderMouseDownNotification";
NSString *MadMonk_GestureSliderMouseUpNotification = @"CAGestureSliderMouseUpNotification";

@implementation MadMonk_GestureSlider

/*	We create our own custom subclass of NSSlider so we can do begin/end gesture notification
	We cannot override mouseUp: because it will never be called. Instead we do a clever trick in mouseDown to send mouseUp notifications */
- (void)mouseDown:(NSEvent *)inEvent {
	[[NSNotificationCenter defaultCenter] postNotificationName: MadMonk_GestureSliderMouseDownNotification object: self];
	
	[super mouseDown: inEvent];	// this call does not return until mouse tracking is complete
								// once tracking is done, we know the mouse has been released, so we can send our mouseup notification

	[[NSNotificationCenter defaultCenter] postNotificationName: MadMonk_GestureSliderMouseUpNotification object: self];
}
	
@end

@implementation MadMonk_CocoaView

-(void) awakeFromNib {
		//NSString *path = [[NSBundle bundleForClass: [bassAmp_CocoaView class]] pathForImageResource: @"SectionPatternLight"];
		//NSImage *pattern = [[NSImage alloc] initByReferencingFile: path];
	
	
		// Check Evaluation time and registration id
		//[self evaluationCheck];
	
	
		//mBackgroundColor = [NSColor whiteColor];//[[NSColor colorWithPatternImage: [pattern autorelease]] retain];
		//backgroundImage =[NSImage imageNamed:@"brushedMetal.gif"];
	
		// setup a timer to call limiter meter display 

    gSourceNoteTimer = [[NSTimer scheduledTimerWithTimeInterval:0.01		// interval, 0.01 seconds
														   target:self
														 selector:@selector(displaySourceNote:)		// call this method
														 userInfo:nil
														  repeats:YES] retain];					// repeat until we cancel it
	
	
	
}

#pragma mark ____ (INIT /) DEALLOC ____
- (void)dealloc {
    [self _removeListeners];
    [super dealloc];
}

#pragma mark ____ PUBLIC FUNCTIONS ____
- (void)setAU:(AudioUnit)inAU {
	// remove previous listeners
	if (mAU) [self _removeListeners];
	mAU = inAU;
    
   	mParameter[0].mAudioUnit = inAU;
	mParameter[0].mParameterID = kParam_AutoMan;
	mParameter[0].mScope = kAudioUnitScope_Global;
	mParameter[0].mElement = 0;	

	
	mParameter[1].mAudioUnit = inAU;
	mParameter[1].mParameterID = kParam_CleanLevel;
	mParameter[1].mScope = kAudioUnitScope_Global;
	mParameter[1].mElement = 0;
	
	mParameter[2].mAudioUnit = inAU;
	mParameter[2].mParameterID = kFilterParam_CutoffFrequency;
	mParameter[2].mScope = kAudioUnitScope_Global;
	mParameter[2].mElement = 0;
	
	mParameter[3].mAudioUnit = inAU;
	mParameter[3].mParameterID = kFilterParam_Resonance;
	mParameter[3].mScope = kAudioUnitScope_Global;
	mParameter[3].mElement = 0;
	
	mParameter[4].mAudioUnit = inAU;
	mParameter[4].mParameterID = kParam_SourcePan;
	mParameter[4].mScope = kAudioUnitScope_Global;
	mParameter[4].mElement = 0;
	
	mParameter[5].mAudioUnit = inAU;
	mParameter[5].mParameterID = kParam_Key;
	mParameter[5].mScope = kAudioUnitScope_Global;
	mParameter[5].mElement = 0;
	
	mParameter[6].mAudioUnit = inAU;
	mParameter[6].mParameterID = kParam_KeyType;
	mParameter[6].mScope = kAudioUnitScope_Global;
	mParameter[6].mElement = 0;
	
	mParameter[7].mAudioUnit = inAU;
	mParameter[7].mParameterID = kParam_MinorType;
	mParameter[7].mScope = kAudioUnitScope_Global;
	mParameter[7].mElement = 0;
	
	mParameter[8].mAudioUnit = inAU;
	mParameter[8].mParameterID = kParam_VoiceOneInterval;
	mParameter[8].mScope = kAudioUnitScope_Global;
	mParameter[8].mElement = 0;
	
	mParameter[9].mAudioUnit = inAU;
	mParameter[9].mParameterID = kParam_VoiceOneAutoInterval;
	mParameter[9].mScope = kAudioUnitScope_Global;
	mParameter[9].mElement = 0;
	
	mParameter[10].mAudioUnit = inAU;
	mParameter[10].mParameterID = kParam_VoiceOneOctave;
	mParameter[10].mScope = kAudioUnitScope_Global;
	mParameter[10].mElement = 0;
	
	mParameter[11].mAudioUnit = inAU;
	mParameter[11].mParameterID = kParam_VoiceOneLevel;
	mParameter[11].mScope = kAudioUnitScope_Global;
	mParameter[11].mElement = 0;
	
	mParameter[12].mAudioUnit = inAU;
	mParameter[12].mParameterID = kParam_VoiceOnePan;
	mParameter[12].mScope = kAudioUnitScope_Global;
	mParameter[12].mElement = 0;
	
	mParameter[13].mAudioUnit = inAU;
	mParameter[13].mParameterID = kParam_VoiceTwoInterval;
	mParameter[13].mScope = kAudioUnitScope_Global;
	mParameter[13].mElement = 0;
	
	mParameter[14].mAudioUnit = inAU;
	mParameter[14].mParameterID = kParam_VoiceTwoAutoInterval;
	mParameter[14].mScope = kAudioUnitScope_Global;
	mParameter[14].mElement = 0;
	
	mParameter[15].mAudioUnit = inAU;
	mParameter[15].mParameterID = kParam_VoiceTwoOctave;
	mParameter[15].mScope = kAudioUnitScope_Global;
	mParameter[15].mElement = 0;
	
	mParameter[16].mAudioUnit = inAU;
	mParameter[16].mParameterID = kParam_VoiceTwoLevel;
	mParameter[16].mScope = kAudioUnitScope_Global;
	mParameter[16].mElement = 0;
	
	mParameter[17].mAudioUnit = inAU;
	mParameter[17].mParameterID = kParam_VoiceTwoPan;
	mParameter[17].mScope = kAudioUnitScope_Global;
	mParameter[17].mElement = 0;
	
	mParameter[18].mAudioUnit = inAU;
	mParameter[18].mParameterID = kParam_SourceNote;
	mParameter[18].mScope = kAudioUnitScope_Global;
	mParameter[18].mElement = 0;
	
	
	
	// add new listeners
	[self _addListeners];
	
	// initial setup
	[self _synchronizeUIWithParameterValues];

}

#pragma mark ____ INTERFACE ACTIONS ____
- (void)displaySourceNote:(id)sender;
{
	float value;
	ComponentResult cr;
	cr = AudioUnitGetParameter(mAU, mParameter[18].mParameterID, kAudioUnitScope_Global, 0, &value);
	
	NSArray *semiToneNames= [[NSArray alloc] initWithObjects: @"A", @"A#", @"B",@"C",@"C#",@"D",@"D#",@"E",@"F",@"F#",@"G",@"G#",nil];
	[uiSourceNote setStringValue:[semiToneNames objectAtIndex:(int)value]];
		//NSLog(@"meter Display value =  %f", value);//
	
}

- (IBAction)iaAutoManual:(id)sender {
	
	
		//NSMatrix * selm = sender;
		//int selRow = [selm selectedRow];
				  
		//NSLog(@"selRow: %d\n",[uiAutoManualMatrix selectedRow]);
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[0], (Float32)[uiAutoManualMatrix selectedRow], 0) == noErr,
			 @"[MadMonk_CocoaView iaAutoManual:] AUParameterSet()");
    if (sender == uiAutoManualMatrix) {
        //[uiVoiceOneOctaveDisplay setFloatValue:voiceOneLevel];
    } else {
			//[uiAutoManualMatrix selectCellAtRow:1 column:0];
    }
}



- (IBAction)iaSourceLevelChanged:(id)sender
{
	
		//feedback amount
	float sourceLevel= [sender floatValue];
		//NSLog(@"sourceLevel: %f\n",sourceLevel);
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[1], (Float32)sourceLevel, 0) == noErr,
			 @"[MadMonk_CocoaView iaSourceLevelChanged:] AUParameterSet()");
    if (sender == uiSourceLevel) {
			//[uiSourceLevelDisplay setFloatValue:sourceLevel];
    } else {
		[uiSourceLevel setFloatValue:sourceLevel];
    }
}

- (IBAction)iaKeyChanged:(id)sender
{
		//NSString * keyName = [[NSString alloc] initWithString:[sender stringValue]];
	int selIndex = [sender indexOfSelectedItem];
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[5], (Float32)selIndex, 0) == noErr,
			 @"[MadMonk_CocoaView iaKeyChanged:] AUParameterSet()");
    if (sender == uiKeySelect) {
			//[uiVoiceOneOctaveDisplay setFloatValue:voiceOneLevel];
    } else {
		[uiKeySelect selectItemAtIndex:selIndex]; 
    }
	
}

- (IBAction)iaKeyTypeChanged:(id)sender
{

	int selIndex = [sender indexOfSelectedItem];
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[6], (Float32)selIndex, 0) == noErr,
			 @"[MadMonk_CocoaView iaKeyTypeChanged:] AUParameterSet()");
    if (sender == uiKeyTypeSelect) {
			//[uiVoiceOneOctaveDisplay setFloatValue:voiceOneLevel];
    } else {
		[uiKeyTypeSelect selectItemAtIndex:selIndex]; 
    }
	
}

- (IBAction)iaMinorTypeChanged:(id)sender
{
	int selIndex = [sender indexOfSelectedItem];
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[7], (Float32)selIndex, 0) == noErr,
			 @"[MadMonk_CocoaView iaMinorTypeChanged:] AUParameterSet()");
    if (sender == uiKeyMinorType) {
			//[uiVoiceOneOctaveDisplay setFloatValue:voiceOneLevel];
    } else {
		[uiKeyMinorType selectItemAtIndex:selIndex]; 
    }
	
}

- (IBAction)iaVoiceOneIntervalChanged:(id)sender
{
		//NSString * v1Interval = [[NSString alloc] initWithString:[sender stringValue]];
	int selIndex = [sender indexOfSelectedItem];

	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[8], (Float32)selIndex, 0) == noErr,
			 @"[MadMonk_CocoaView iaVoiceOneIntervalChanged:] AUParameterSet()");
    if (sender == uiVoiceOneIntervalSelect) {
			//[uiVoiceOneOctaveDisplay setFloatValue:voiceOneLevel];
    } else {
		[uiVoiceOneIntervalSelect selectItemAtIndex:selIndex]; 
    }
	
	
	
}

- (IBAction)iaVoiceOneAutoIntervalChanged:(id)sender
{
	int selIndex = [sender indexOfSelectedItem];
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[9], (Float32)selIndex, 0) == noErr,
			 @"[MadMonk_CocoaView iaVoiceOneAutoIntervalChanged:] AUParameterSet()");
    if (sender == uiVoiceOneAutoIntervalSelect) {
			//[uiVoiceOneOctaveDisplay setFloatValue:voiceOneLevel];
    } else {
		[uiVoiceOneAutoIntervalSelect selectItemAtIndex:selIndex]; 
    }
}

- (IBAction)iaVoiceOneOctaveIntervalChanged:(id)sender
{
	float voiceOneOctave= [sender floatValue];
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[10], (Float32)voiceOneOctave, 0) == noErr,
			 @"[MadMonk_CocoaView iaVoiceOneOctaveIntervalChanged:] AUParameterSet()");
    if (sender == uiVoiceOneOctave) {
			//[uiVoiceOneOctaveDisplay setFloatValue:voiceOneLevel];
    } else {
			//[uiVoiceOneOctaveDisplay setFloatValue:voiceOneLevel];
    }
}


- (IBAction)iaVoiceOneLevelChanged:(id)sender
{

	float voiceOneLevel= [sender floatValue];
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[11], (Float32)voiceOneLevel, 0) == noErr,
			 @"[MadMonk_CocoaView iaVoiceOneLevelChanged:] AUParameterSet()");
    if (sender == uiVoiceOneLevel) {
        [uiVoiceOneLevelDisplay setFloatValue:voiceOneLevel];
    } else {
        [uiVoiceOneLevelDisplay setFloatValue:voiceOneLevel];
    }
}

- (IBAction)iaVoiceTwoIntervalChanged:(id)sender
{
	int selIndex = [sender indexOfSelectedItem];
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[13], (Float32)selIndex, 0) == noErr,
			 @"[MadMonk_CocoaView iaVoiceTwoIntervalChanged:] AUParameterSet()");
    if (sender == uiVoiceTwoIntervalSelect) {
			//[uiVoiceTwoOctaveDisplay setFloatValue:voiceTwoLevel];
    } else {
		[uiVoiceTwoIntervalSelect selectItemAtIndex:selIndex]; 
    }
	
}

- (IBAction)iaVoiceTwoAutoIntervalChanged:(id)sender
{
	int selIndex = [sender indexOfSelectedItem];
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[14], (Float32)selIndex, 0) == noErr,
			 @"[MadMonk_CocoaView iaVoiceTwoAutoIntervalChanged:] AUParameterSet()");
    if (sender == uiVoiceTwoAutoIntervalSelect) {
			//[uiVoiceTwoOctaveDisplay setFloatValue:voiceTwoLevel];
    } else {
		[uiVoiceTwoAutoIntervalSelect selectItemAtIndex:selIndex]; 
    }
}

- (IBAction)iaVoiceTwoOctaveIntervalChanged:(id)sender
{
	float voiceTwoOctave= [sender floatValue];
	
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[15], (Float32)voiceTwoOctave, 0) == noErr,
			 @"[MadMonk_CocoaView iaVoiceTwoOctaveIntervalChanged:] AUParameterSet()");
    if (sender == uiVoiceTwoOctave) {
			//[uiVoiceTwoOctaveDisplay setFloatValue:voiceTwoLevel];
    } else {
			//[uiVoiceTwoOctaveDisplay setFloatValue:voiceTwoLevel];
    }
}


- (IBAction)iaVoiceTwoLevelChanged:(id)sender
{

	float voiceTwoLevel= [sender floatValue];
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[16], (Float32)voiceTwoLevel, 0) == noErr,
			 @"[MadMonk_CocoaView iaVoiceTwoLevelChanged:] AUParameterSet()");
    if (sender == uiVoiceTwoLevel) {
        [uiVoiceTwoLevelDisplay setFloatValue:voiceTwoLevel];
    } else {
        [uiVoiceTwoLevelDisplay setFloatValue:voiceTwoLevel];
    }
}


- (IBAction)iaFilterCutoff:(id)sender
{
	float value = (int)[sender floatValue];
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[2], (Float32)value, 0) == noErr,
			 @"[MadMonk_CocoaView iaFilterCutoff:] AUParameterSet()");
    if (sender == uiFilterCutoffFreq) {
		[uiFilterCutoffDisplay setFloatValue:value];
    } else {
		[uiFilterCutoffDisplay setFloatValue:value];
    }
}
- (IBAction)iaFilterResonance:(id)sender
{
	float value = (int)[sender floatValue];
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[3], (Float32)value, 0) == noErr,
			 @"[MadMonk_CocoaView iaFilterResonance:] AUParameterSet()");
    if (sender == uiFilterResonance) {
			[uiFilterResonanceDisplay setFloatValue:value];
    } else {
			[uiFilterResonanceDisplay setFloatValue:value];
    }
}

- (IBAction)iaSourcePanChanged:(id)sender
{
	float value = [sender floatValue];
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[4], (Float32)value, 0) == noErr,
			 @"[MadMonk_CocoaView iaSourcePanChanged:] AUParameterSet()");
    if (sender == sourcePan) {
			//[uiFilterResonanceDisplay setFloatValue:value];
    } else {
			//[uiFilterResonanceDisplay setFloatValue:value];
    }
}

- (IBAction)iaVoiceOnePanChanged:(id)sender
{
	float value = [sender floatValue];
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[12], (Float32)value, 0) == noErr,
			 @"[MadMonk_CocoaView iaVoiceOnePanChanged:] AUParameterSet()");
    if (sender == voiceOnePan) {
			//[uiFilterResonanceDisplay setFloatValue:value];
    } else {
			//[uiFilterResonanceDisplay setFloatValue:value];
    }
}

- (IBAction)iaVoiceTwoPanChanged:(id)sender
{
	float value = [sender floatValue];
	NSAssert(	AUParameterSet(mParameterListener, sender, &mParameter[17], (Float32)value, 0) == noErr,
			 @"[MadMonk_CocoaView iaVoiceTwoPanChanged:] AUParameterSet()");
    if (sender == voiceTwoPan) {
			//[uiFilterResonanceDisplay setFloatValue:value];
    } else {
			//[uiFilterResonanceDisplay setFloatValue:value];
    }
}


//////////////////////////////





#pragma mark ____ NOTIFICATIONS ____

// This routine is called when the user has clicked on the slider. We need to send a begin parameter change gesture to alert hosts that the parameter may be changing value
-(void) handleMouseDown: (NSNotification *) aNotification {
	if ([aNotification object] == uiVoiceOneLevel) {
		AudioUnitEvent event;
		event.mArgument.mParameter = mParameter[1];
		event.mEventType = kAudioUnitEvent_BeginParameterChangeGesture;
		
		AUEventListenerNotify (NULL, self, &event);		// NOTE, if you have an AUEventListenerRef because you are listening to event notification, 
														// pass that as the first argument to AUEventListenerNotify instead of NULL 
	}

	
}

-(void) handleMouseUp: (NSNotification *) aNotification {
	if ([aNotification object] == uiVoiceOneLevel) {
		AudioUnitEvent event;
		event.mArgument.mParameter = mParameter[1];
		event.mEventType = kAudioUnitEvent_EndParameterChangeGesture;
	
		AUEventListenerNotify (NULL, self, &event);		// NOTE, if you have an AUEventListenerRef because you are listening to event notification, 
														// pass that as the first argument to AUEventListenerNotify instead of NULL 
	}


}


#pragma mark ____ PRIVATE FUNCTIONS ____
- (void)_addListeners {
	NSAssert (	AUListenerCreate(	ParameterListenerDispatcher, self, 
                                    CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 0.100, // 100 ms
                                    &mParameterListener	) == noErr,
                @"[MadMonk_CocoaView _addListeners] AUListenerCreate()");
	
	
    int i;
    for (i = 0; i < kNumberOfParameters; ++i) {
        mParameter[i].mAudioUnit = mAU;
        NSAssert (	AUListenerAddParameter (mParameterListener, NULL, &mParameter[i]) == noErr,
                    @"[MadMonk_CocoaView _addListeners] AUListenerAddParameter()");
    }
    
   	[[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(handleMouseDown:) name:MadMonk_GestureSliderMouseDownNotification object: uiSourceLevel];
	[[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(handleMouseUp:) name:MadMonk_GestureSliderMouseUpNotification object: uiSourceLevel];
	
}

- (void)_removeListeners {
    int i;
    for (i = 0; i < kNumberOfParameters; ++i) {
        NSAssert (	AUListenerRemoveParameter(mParameterListener, NULL, &mParameter[i]) == noErr,
                    @"[MadMonk_CocoaView _removeListeners] AUListenerRemoveParameter()");
    }
    
	NSAssert (	AUListenerDispose(mParameterListener) == noErr,
                @"[MadMonk_CocoaView _removeListeners] AUListenerDispose()");
                
    [[NSNotificationCenter defaultCenter] removeObserver: self];
}

- (void)_synchronizeUIWithParameterValues {
	Float32 value;
    int i;
	
    for (i = 0; i < kNumberOfParameters; ++i) {
        // only has global parameters
        NSAssert (	AudioUnitGetParameter(mAU, mParameter[i].mParameterID, kAudioUnitScope_Global, 0, &value) == noErr,
                    @"[MadMonk_CocoaView synchronizeUIWithParameterValues] (x.1)");
        NSAssert (	AUParameterSet (mParameterListener, self, &mParameter[i], value, 0) == noErr,
                    @"[MadMonk_CocoaView synchronizeUIWithParameterValues] (x.2)");
        NSAssert (	AUParameterListenerNotify (mParameterListener, self, &mParameter[i]) == noErr,
                    @"[MadMonk_CocoaView synchronizeUIWithParameterValues] (x.3)");
    }
	
	
}

#pragma mark ____ LISTENER CALLBACK DISPATCHEE ____
- (void)_parameterListener:(void *)inObject parameter:(const AudioUnitParameter *)inParameter value:(Float32)inValue {
    //inObject ignored in this case.
    
		NSLog(@"inValue: %f\n",inValue);
	
	switch (inParameter->mParameterID) {
		case kParam_AutoMan:
			[uiAutoManualMatrix setState:NSOnState atRow:inValue column:0];
			break;
		case kParam_CleanLevel:
			[uiSourceLevel setFloatValue:inValue];
			[uiSourceLevelDisplay setStringValue:[[NSNumber numberWithFloat:inValue] stringValue]];
			break;
		case kParam_Key:
			[uiKeySelect selectItemAtIndex:inValue];
			break;
		case kParam_KeyType:
			[uiKeyTypeSelect selectItemAtIndex:inValue];
			break;
		case kParam_MinorType:
			[uiKeyMinorType selectItemAtIndex:inValue];
			break;
		case kParam_VoiceOneInterval:
			[uiVoiceOneIntervalSelect selectItemAtIndex:inValue];
			break;
		case kParam_VoiceOneAutoInterval:
			[uiVoiceOneAutoIntervalSelect selectItemAtIndex:inValue];
			break;
		case kParam_VoiceOneOctave:
			[uiVoiceOneOctave setState:inValue];
			break;
		case kParam_VoiceOneLevel:
			[uiVoiceOneLevel setFloatValue:inValue];
			[uiVoiceOneLevelDisplay setStringValue:[[NSNumber numberWithFloat:inValue] stringValue]];
			break;
		case kParam_VoiceTwoInterval:
			[uiVoiceTwoIntervalSelect selectItemAtIndex:inValue];
			break;
		case kParam_VoiceTwoAutoInterval:
			[uiVoiceTwoAutoIntervalSelect selectItemAtIndex:inValue];
			break;
		case kParam_VoiceTwoOctave:
			[uiVoiceTwoOctave setState:inValue];
			break;
		case kParam_VoiceTwoLevel:
			[uiVoiceTwoLevel setFloatValue:inValue];
			[uiVoiceTwoLevelDisplay setStringValue:[[NSNumber numberWithFloat:inValue] stringValue]];
			break;
		case kParam_SourceNote:
			[uiSourceNote setFloatValue:inValue];
			break;
		case kFilterParam_CutoffFrequency:
			[uiFilterCutoffFreq setFloatValue:inValue];
			[uiFilterCutoffDisplay setFloatValue:inValue];
			break;
		case kFilterParam_Resonance:
			[uiFilterResonance setFloatValue:inValue];
			[uiFilterResonanceDisplay setFloatValue:inValue];
			break;
		case kParam_VoiceOnePan:
			[voiceOnePan setFloatValue:inValue];
			break;
		case kParam_VoiceTwoPan:
			[voiceTwoPan setFloatValue:inValue];
			break;

	}
}


@end
