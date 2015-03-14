/*
*	File:		CocoaView.h
*	
*	Version:	1.0
* 
*	Created:	11/17/10
*	
*	Copyright:  Copyright © 2010 _LoudSoftware_, All Rights Reserved
* 
*	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc. ("Apple") in 
*				consideration of your agreement to the following terms, and your use, installation, modification 
*				or redistribution of this Apple software constitutes acceptance of these terms.  If you do 
*				not agree with these terms, please do not use, install, modify or redistribute this Apple 
*				software.
*
*				In consideration of your agreement to abide by the following terms, and subject to these terms, 
*				Apple grants you a personal, non-exclusive license, under Apple's copyrights in this 
*				original Apple software (the "Apple Software"), to use, reproduce, modify and redistribute the 
*				Apple Software, with or without modifications, in source and/or binary forms; provided that if you 
*				redistribute the Apple Software in its entirety and without modifications, you must retain this 
*				notice and the following text and disclaimers in all such redistributions of the Apple Software. 
*				Neither the name, trademarks, service marks or logos of Apple Computer, Inc. may be used to 
*				endorse or promote products derived from the Apple Software without specific prior written 
*				permission from Apple.  Except as expressly stated in this notice, no other rights or 
*				licenses, express or implied, are granted by Apple herein, including but not limited to any 
*				patent rights that may be infringed by your derivative works or by other works in which the 
*				Apple Software may be incorporated.
*
*				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO WARRANTIES, EXPRESS OR 
*				IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY 
*				AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE 
*				OR IN COMBINATION WITH YOUR PRODUCTS.
*
*				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR CONSEQUENTIAL 
*				DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*				OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, 
*				REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER 
*				UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN 
*				IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#import <Cocoa/Cocoa.h>
#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>


/************************************************************************************************************/
/* NOTE: It is important to rename ALL ui classes when using the XCode Audio Unit with Cocoa View template	*/
/*		 Cocoa has a flat namespace, and if you use the default filenames, it is possible that you will		*/
/*		 get a namespace collision with classes from the cocoa view of a previously loaded audio unit.		*/
/*		 We recommend that you use a unique prefix that includes the manufacturer name and unit name on		*/
/*		 all objective-C source files. You may use an underscore in your name, but please refrain from		*/
/*		 starting your class name with an undescore as these names are reserved for Apple.					*/
/*  Example  : AppleDemoFilter_UIView AppleDemoFilter_ViewFactory											*/
/************************************************************************************************************/

@interface MadMonk_GestureSlider : NSSlider {}
@end

@interface MadMonk_CocoaView : NSView
{
    // IB Members
	IBOutlet MadMonk_GestureSlider * uiSourceLevel;
    IBOutlet NSTextField *			uiSourceLevelDisplay;
	
	IBOutlet MadMonk_GestureSlider * uiVoiceOneLevel;
    IBOutlet NSTextField *			uiVoiceOneLevelDisplay;

	IBOutlet MadMonk_GestureSlider * uiVoiceTwoLevel;
    IBOutlet NSTextField *			uiVoiceTwoLevelDisplay;

		//key
	IBOutlet NSComboBox * uiKeySelect;
	IBOutlet NSComboBox * uiKeyTypeSelect;
	IBOutlet NSComboBox * uiKeyMinorType;
	
	//interval select
	IBOutlet NSComboBox *	uiVoiceOneIntervalSelect;
	IBOutlet NSComboBox *	uiVoiceOneAutoIntervalSelect;
	IBOutlet NSButton * uiVoiceOneOctave;
	
	IBOutlet NSComboBox *	uiVoiceTwoIntervalSelect;
	IBOutlet NSComboBox *	uiVoiceTwoAutoIntervalSelect;
	IBOutlet NSButton * uiVoiceTwoOctave;
	
	//auto-manual select
	IBOutlet NSMatrix * uiAutoManualMatrix;
	
	
		//input filter
	IBOutlet NSSlider * uiFilterCutoffFreq;
	IBOutlet NSSlider * uiFilterResonance;
	IBOutlet NSTextField * uiFilterCutoffDisplay;
	IBOutlet NSTextField * uiFilterResonanceDisplay;
	
		//pan controls
	IBOutlet NSSlider * sourcePan;
	IBOutlet NSSlider * voiceOnePan;
	IBOutlet NSSlider * voiceTwoPan;
	
		//note display
	IBOutlet NSTextField * uiSourceNote;
	
    // Other Members
    AudioUnit 				mAU;
	AudioUnitParameter		mParameter[19];
    AUParameterListenerRef	mParameterListener;
	
		//timer for soure note
	NSTimer 			*			gSourceNoteTimer;
	
	
}

#pragma mark ____ PUBLIC FUNCTIONS ____
- (void)setAU:(AudioUnit)inAU;

#pragma mark ____ INTERFACE ACTIONS ____
- (IBAction)iaAutoManual:(id)sender;
- (IBAction)iaSourceLevelChanged:(id)sender;
- (IBAction)iaKeyChanged:(id)sender;
- (IBAction)iaKeyTypeChanged:(id)sender;
- (IBAction)iaMinorTypeChanged:(id)sender;
- (IBAction)iaVoiceOneIntervalChanged:(id)sender;
- (IBAction)iaVoiceOneAutoIntervalChanged:(id)sender;
- (IBAction)iaVoiceOneOctaveIntervalChanged:(id)sender;
- (IBAction)iaVoiceOneLevelChanged:(id)sender;
- (IBAction)iaVoiceTwoIntervalChanged:(id)sender;
- (IBAction)iaVoiceTwoAutoIntervalChanged:(id)sender;
- (IBAction)iaVoiceTwoOctaveIntervalChanged:(id)sender;
- (IBAction)iaVoiceTwoLevelChanged:(id)sender;
- (IBAction)iaSourcePanChanged:(id)sender;
- (IBAction)iaVoiceOnePanChanged:(id)sender;
- (IBAction)iaVoiceTwoPanChanged:(id)sender;
- (IBAction)iaFilterCutoff:(id)sender;
- (IBAction)iaFilterResonance:(id)sender;
- (void)displaySourceNote:(id)sender;
/*
- (void)setVoiceOnePitch:(int)value:(id)sender;
- (void)setVoiceTwoPitch:(int)value:(id)sender;
 */
#pragma mark ____ PRIVATE FUNCTIONS
- (void)_synchronizeUIWithParameterValues;
- (void)_addListeners;
- (void)_removeListeners;

#pragma mark ____ LISTENER CALLBACK DISPATCHEE ____
- (void)_parameterListener:(void *)inObject parameter:(const AudioUnitParameter *)inParameter value:(Float32)inValue;

@end
