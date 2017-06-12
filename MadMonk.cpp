/*

 TESTING
 *	File:		MadMonk.cpp
 *
 *	Version:	1.0
 *
 *	Created:	11/17/10
 *
 *	Copyright:  Copyright © 2010 _LoudSoftware_, All Rights Reserved
 *
 *	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by
 Apple Computer, Inc. ("Apple") in
 *				consideration of your agreement to the following
 terms, and your use, installation, modification
 *				or redistribution of this Apple software
 constitutes acceptance of these terms.  If you do
 *				not agree with these terms, please do not use,
 install, modify or redistribute this Apple
 *				software.
 *
 *				In consideration of your agreement to abide by the
 following terms, and subject to these terms,
 *				Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this
 *				original Apple software (the "Apple Software"), to
 use, reproduce, modify and redistribute the
 *				Apple Software, with or without modifications, in
 source and/or binary forms; provided that if you
 *				redistribute the Apple Software in its entirety
 and without modifications, you must retain this
 *				notice and the following text and disclaimers in
 all such redistributions of the Apple Software.
 *				Neither the name, trademarks, service marks or
 logos of Apple Computer, Inc. may be used to
 *				endorse or promote products derived from the Apple
 Software without specific prior written
 *				permission from Apple.  Except as expressly stated
 in this notice, no other rights or
 *				licenses, express or implied, are granted by Apple
 herein, including but not limited to any
 *				patent rights that may be infringed by your
 derivative works or by other works in which the
 *				Apple Software may be incorporated.
 *
 *				The Apple Software is provided by Apple on an "AS
 IS" basis.  APPLE MAKES NO WARRANTIES, EXPRESS OR
 *				IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
 WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY
 *				AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING
 THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE
 *				OR IN COMBINATION WITH YOUR PRODUCTS.
 *
 *				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL,
 INDIRECT, INCIDENTAL OR CONSEQUENTIAL
 *				DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *				OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE,
 *				REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION OF
 THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
 *				UNDER THEORY OF CONTRACT, TORT (INCLUDING
 NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN
 *				IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.
 *
 */

#include "MadMonk.h"
#include <AudioToolbox/AudioToolbox.h>
#include <Accelerate/Accelerate.h>
#include <semitoneGet.h>
#include <math.h>
#include <LoPassFilter.h>


#define MAXFRAMES 4096 //no smearing from 130hz - 1396hz

int CurrentSampleRate=44100;//just to initialize
FFTSetup fftSetup;
COMPLEX_SPLIT A;
int log2n, n, nOver2;
void *dataBuffer;
size_t bufferCapacity;	// In samples
size_t indexPitch;	// In samples
float lastFreq=0;
string keys[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
string keyTypes[2] = {"Major","Minor"};
string minorTypes[3] = {"Natural","Harmonic", "Melodic"};
int lastSemitoneVoiceOne=0;
int lastSemitoneVoiceTwo=0;
AudioBufferList* bufferList;
semitoneGet voiceOneSemitone;


double	mLastCutoff = 0;
double	mLastResonance;

void smbPitchShift1(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata);
void smbPitchShift2(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata);
void smbPitchShift3(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata);
void smbPitchShift4(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata);
void smbPitchDetect(long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//COMPONENT_ENTRY(MadMonk);

// new AUPlugIn macro
AUDIOCOMPONENT_ENTRY(AUBaseFactory, MadMonk)



static CFStringRef kCutoffFreq_Name = CFSTR("input cutoff frequency");
static CFStringRef kResonance_Name = CFSTR("input filter resonance");


const float kMinCutoffHz = 10.0;
const float kDefaultCutoff = 350.0;
const float kMinResonance = -50.0;
const float kMaxResonance = 50.0;
const float kDefaultResonance = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MadMonk::MadMonk
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MadMonk::MadMonk(AudioUnit component)
: AUEffectBase(component)
{
	CreateElements();
	Globals()->UseIndexedParameters(kNumberOfParameters);

	SetParameter(kParam_AutoMan, kDefaultValue_kParamName_AutoMan);
	SetParameter(kParam_VoiceOneLevel, kDefaultValue_VoiceOneLevel );
	SetParameter(kParam_VoiceTwoLevel, kDefaultValue_VoiceTwoLevel );
	SetParameter(kParam_CleanLevel, kDefaultValue_CleanLevel);
	
	SetParameter(kParam_VoiceOneInterval, kDefaultValue_kParamName_VoiceOneInterval);
	SetParameter(kParam_VoiceOneAutoInterval, kDefaultValue_kParamName_VoiceOneAutoInterval);
	SetParameter(kParam_VoiceTwoInterval, kDefaultValue_kParamName_VoiceTwoInterval);
	SetParameter(kParam_VoiceTwoAutoInterval, kDefaultValue_kParamName_VoiceTwoAutoInterval);
	SetParameter(kParam_MinorType, kDefaultValue_kParamName_MinorType);
	SetParameter(kParam_VoiceOneOctave, kDefaultValue_kParamName_VoiceOneOctave);	
	SetParameter(kParam_VoiceTwoOctave, kDefaultValue_kParamName_VoiceTwoOctave);
		//SetParameter(kParam_SourceNote, kDefaultValue_kParamName_SourceNote);
	SetParameter(kFilterParam_CutoffFrequency, kDefaultCutoff );
	SetParameter(kFilterParam_Resonance, kDefaultResonance );
	SetParameter(kParam_KeyType, kDefaultValue_kParamName_KeyType);
		// kFilterParam_CutoffFrequency max value depends on sample-rate
	SetParamHasSampleRateDependency(true );
	
	CurrentSampleRate = GetSampleRate();
	realFFTSetup();

	printf("sampleRate: %d\n\n",CurrentSampleRate);
	
	
#if AU_DEBUG_DISPATCHER
	mDebugDispatcher = new AUDebugDispatcher (this);
#endif
	
	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MadMonk::Initialize
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult		MadMonk::Initialize()
{
	ComponentResult result = AUEffectBase::Initialize();
	
	if(result == noErr )
	{
			// in case the AU was un-initialized and parameters were changed, the view can now
			// be made aware it needs to update the frequency response curve
		PropertyChanged(kAudioUnitCustomProperty_FilterFrequencyResponse, kAudioUnitScope_Global, 0 );
	}
	
	return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MadMonk::GetParameterValueStrings
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus			MadMonk::GetParameterValueStrings(AudioUnitScope		inScope,
													  AudioUnitParameterID	inParameterID,
													  CFArrayRef *		outStrings)
{
	

	
	if ((inScope == kAudioUnitScope_Global) &&             // 1
        (inParameterID == kParam_AutoMan)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_Auto,
            kMenuItem_Manual
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	else if ((inScope == kAudioUnitScope_Global) &&             // 1
			 (inParameterID == kParam_MinorType)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_Natural,
            kMenuItem_Harmonic,
			kMenuItem_Melodic
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	else if ((inScope == kAudioUnitScope_Global) &&             // 1
        (inParameterID == kParam_KeyType)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_Major,
            kMenuItem_Minor
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	else if ((inScope == kAudioUnitScope_Global) &&             // 1
			 (inParameterID == kParam_VoiceOneInterval)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_VoiceOneIntervalUnison,
			kMenuItem_VoiceOneIntervalMinorSecond,        
			kMenuItem_VoiceOneIntervalMajorSecond, 
			kMenuItem_VoiceOneIntervalMinorThird,
			kMenuItem_VoiceOneIntervalMajorThird,
			kMenuItem_VoiceOneIntervalPerfectFourth,
			kMenuItem_VoiceOneIntervalDimFifth,
			kMenuItem_VoiceOneIntervalPerfectFifth,
			kMenuItem_VoiceOneIntervalMinorSixth,
			kMenuItem_VoiceOneIntervalMajorSixth,
			kMenuItem_VoiceOneIntervalMinorSeventh,
			kMenuItem_VoiceOneIntervalMajorSeventh
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	else if ((inScope == kAudioUnitScope_Global) &&             // 1
			 (inParameterID == kParam_VoiceTwoInterval)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_VoiceTwoIntervalUnison,
			kMenuItem_VoiceTwoIntervalMinorSecond,        
			kMenuItem_VoiceTwoIntervalMajorSecond, 
			kMenuItem_VoiceTwoIntervalMinorThird,
			kMenuItem_VoiceTwoIntervalMajorThird,
			kMenuItem_VoiceTwoIntervalPerfectFourth,
			kMenuItem_VoiceTwoIntervalDimFifth,
			kMenuItem_VoiceTwoIntervalPerfectFifth,
			kMenuItem_VoiceTwoIntervalMinorSixth,
			kMenuItem_VoiceTwoIntervalMajorSixth,
			kMenuItem_VoiceTwoIntervalMinorSeventh,
			kMenuItem_VoiceTwoIntervalMajorSeventh
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	else if ((inScope == kAudioUnitScope_Global) &&             // 1
			 (inParameterID == kParam_VoiceOneAutoInterval)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_VoiceOneAutoIntervalUnison,
			kMenuItem_VoiceOneAutoIntervalSecond,
			kMenuItem_VoiceOneAutoIntervalThird,
			kMenuItem_VoiceOneAutoIntervalFourth,
			kMenuItem_VoiceOneAutoIntervalFifth,
			kMenuItem_VoiceOneAutoIntervalSixth,
			kMenuItem_VoiceOneAutoIntervalSeventh
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	else if ((inScope == kAudioUnitScope_Global) &&             // 1
			 (inParameterID == kParam_VoiceTwoAutoInterval)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_VoiceTwoAutoIntervalUnison,
			kMenuItem_VoiceTwoAutoIntervalSecond,
			kMenuItem_VoiceTwoAutoIntervalThird,
			kMenuItem_VoiceTwoAutoIntervalFourth,
			kMenuItem_VoiceTwoAutoIntervalFifth,
			kMenuItem_VoiceTwoAutoIntervalSixth,
			kMenuItem_VoiceTwoAutoIntervalSeventh
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	
	else if ((inScope == kAudioUnitScope_Global) &&             // 1
			 (inParameterID == kParam_Key)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_C,
            kMenuItem_Cs,
			kMenuItem_D,
			kMenuItem_Ds,
			kMenuItem_E,
			kMenuItem_F,
			kMenuItem_Fs,
			kMenuItem_G,
			kMenuItem_Gs,
			kMenuItem_A,
			kMenuItem_As,
			kMenuItem_B
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	else if ((inScope == kAudioUnitScope_Global) &&             // 1
			 (inParameterID == kParam_VoiceOneOctave)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_VoiceOneOctaveOff,
            kMenuItem_VoiceOneOctaveOn
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	else if ((inScope == kAudioUnitScope_Global) &&             // 1
			 (inParameterID == kParam_VoiceTwoOctave)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_VoiceTwoOctaveOff,
            kMenuItem_VoiceTwoOctaveOn
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	else if ((inScope == kAudioUnitScope_Global) &&             // 1
			 (inParameterID == kParam_SourceNote)) {
		
        if (outStrings == NULL) return noErr;              // 2
		
        CFStringRef strings [] = {                         // 3
            kMenuItem_C,
            kMenuItem_Cs,
			kMenuItem_D,
			kMenuItem_Ds,
			kMenuItem_E,
			kMenuItem_F,
			kMenuItem_Fs,
			kMenuItem_G,
			kMenuItem_Gs,
			kMenuItem_A,
			kMenuItem_As,
			kMenuItem_B
        };
		
        *outStrings = CFArrayCreate (                      // 4
									 NULL,
									 (const void **) strings,
									 (sizeof (strings) / sizeof (strings [0])),     // 5
									 NULL
									 );
        return noErr;
    }
	
	
    return kAudioUnitErr_InvalidParameter;
	
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MadMonk::GetParameterInfo
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus			MadMonk::GetParameterInfo(AudioUnitScope		inScope,
											  AudioUnitParameterID	inParameterID,
											  AudioUnitParameterInfo	&outParameterInfo )
{
	OSStatus result = noErr;
	
	outParameterInfo.flags = 	kAudioUnitParameterFlag_IsWritable
	|		kAudioUnitParameterFlag_IsReadable;
    
    if (inScope == kAudioUnitScope_Global) {
        switch(inParameterID)
        {
			case kParam_VoiceOneLevel:
                AUBase::FillInParameterName (outParameterInfo, kParameterVoiceOneLevel, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_LinearGain;
				outParameterInfo.minValue = 0.0;
				outParameterInfo.maxValue = 1.0;
				outParameterInfo.defaultValue = 0.5;
                break;
			case kParam_VoiceTwoLevel:
                AUBase::FillInParameterName (outParameterInfo, kParameterVoiceTwoLevel, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_LinearGain;
				outParameterInfo.minValue = 0.0;
				outParameterInfo.maxValue = 1.0;
				outParameterInfo.defaultValue = 0.5;
                break;
			case kParam_CleanLevel:
                AUBase::FillInParameterName (outParameterInfo, kParameterCleanLevel, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_LinearGain;
				outParameterInfo.minValue = 0.0;
				outParameterInfo.maxValue = 1.0;
				outParameterInfo.defaultValue = 0.5;
                break;
			case kParam_AutoMan:
                AUBase::FillInParameterName (outParameterInfo, kParamName_AutoMan, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kParamName_AutoMan1;
				outParameterInfo.maxValue = kParamName_AutoMan2;
				outParameterInfo.defaultValue = kDefaultValue_kParamName_AutoMan;
                break;
			case kParam_VoiceOneOctave:
                AUBase::FillInParameterName (outParameterInfo, kParamName_VoiceOneOctave, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kParamName_VoiceOneOctave1;
				outParameterInfo.maxValue = kParamName_VoiceOneOctave2;
				outParameterInfo.defaultValue = kDefaultValue_kParamName_VoiceOneOctave;
                break;
			case kParam_VoiceTwoOctave:
                AUBase::FillInParameterName (outParameterInfo, kParamName_VoiceTwoOctave, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kParamName_VoiceTwoOctave1;
				outParameterInfo.maxValue = kParamName_VoiceTwoOctave2;
				outParameterInfo.defaultValue = kDefaultValue_kParamName_VoiceTwoOctave;
                break;
			case kParam_VoiceOneInterval:
                AUBase::FillInParameterName (outParameterInfo, kParamName_VoiceOneInterval, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kParamName_VoiceOneInterval1;
				outParameterInfo.maxValue = kParamName_VoiceOneInterval12;
				outParameterInfo.defaultValue = kDefaultValue_kParamName_VoiceOneInterval;
                break;
			case kParam_VoiceOneAutoInterval:
                AUBase::FillInParameterName (outParameterInfo, kParamName_VoiceOneAutoInterval, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kParamName_VoiceOneAutoInterval1;
				outParameterInfo.maxValue = kParamName_VoiceOneAutoInterval7;
				outParameterInfo.defaultValue = kDefaultValue_kParamName_VoiceOneAutoInterval;
                break;
			case kParam_VoiceTwoInterval:
                AUBase::FillInParameterName (outParameterInfo, kParamName_VoiceTwoInterval, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kParamName_VoiceTwoInterval1;
				outParameterInfo.maxValue = kParamName_VoiceTwoInterval12;
				outParameterInfo.defaultValue = kDefaultValue_kParamName_VoiceTwoInterval;
                break;
			case kParam_VoiceTwoAutoInterval:
                AUBase::FillInParameterName (outParameterInfo, kParamName_VoiceTwoAutoInterval, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kParamName_VoiceTwoAutoInterval1;
				outParameterInfo.maxValue = kParamName_VoiceTwoAutoInterval7;
				outParameterInfo.defaultValue = kDefaultValue_kParamName_VoiceTwoAutoInterval;
                break;
			case kParam_KeyType:
                AUBase::FillInParameterName (outParameterInfo, kParamName_KeyType, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kParamName_KeyType1;
				outParameterInfo.maxValue = kParamName_KeyType2;
				outParameterInfo.defaultValue = kDefaultValue_kParamName_KeyType;
                break;
			case kParam_MinorType:
                AUBase::FillInParameterName (outParameterInfo, kParamName_MinorType, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kParamName_MinorType1;
				outParameterInfo.maxValue = kParamName_MinorType3;
				outParameterInfo.defaultValue = kDefaultValue_kParamName_MinorType;
                break;
			case kParam_Key:
                AUBase::FillInParameterName (outParameterInfo, kParamName_Key, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kParamName_Key1;
				outParameterInfo.maxValue = kParamName_Key12;
				outParameterInfo.defaultValue = kDefaultValue_kParamName_Key;
                break;
			case kParam_SourceNote:
                AUBase::FillInParameterName (outParameterInfo, kParamName_SourceNote, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Generic;
				outParameterInfo.minValue = 0;
				outParameterInfo.maxValue = 20;
				outParameterInfo.defaultValue = 0;
                break;
			case kFilterParam_CutoffFrequency:
				AUBase::FillInParameterName (outParameterInfo, kCutoffFreq_Name, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Hertz;
				outParameterInfo.minValue = kMinCutoffHz;
				outParameterInfo.maxValue = 2000;//GetSampleRate() * 0.5;
				outParameterInfo.defaultValue = kDefaultCutoff;
				outParameterInfo.flags += kAudioUnitParameterFlag_IsHighResolution;
				outParameterInfo.flags += kAudioUnitParameterFlag_DisplayLogarithmic;
				break;
				
			case kFilterParam_Resonance:
				AUBase::FillInParameterName (outParameterInfo, kResonance_Name, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Decibels;
				outParameterInfo.minValue = kMinResonance;
				outParameterInfo.maxValue = kMaxResonance;
				outParameterInfo.defaultValue = kDefaultResonance;
				outParameterInfo.flags += kAudioUnitParameterFlag_IsHighResolution;
				break;
			case kParam_SourcePan:
                AUBase::FillInParameterName (outParameterInfo, kParamName_SourcePan, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Pan;
				outParameterInfo.minValue = -1;
				outParameterInfo.maxValue = 1;
				outParameterInfo.defaultValue = 0;
                break;
			case kParam_VoiceOnePan:
                AUBase::FillInParameterName (outParameterInfo, kParamName_VoiceOnePan, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Pan;
				outParameterInfo.minValue = -1;
				outParameterInfo.maxValue = 1;
				outParameterInfo.defaultValue = 0;
                break;
			case kParam_VoiceTwoPan:
                AUBase::FillInParameterName (outParameterInfo, kParamName_VoiceTwoPan, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Pan;
				outParameterInfo.minValue = -1;
				outParameterInfo.maxValue = 1;
				outParameterInfo.defaultValue = 0;
                break;
            default:
                result = kAudioUnitErr_InvalidParameter;
                break;
		}
	} else {
        result = kAudioUnitErr_InvalidParameter;
    }
    
	
	
	return result;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MadMonk::GetPropertyInfo
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus			MadMonk::GetPropertyInfo (AudioUnitPropertyID	inID,
											  AudioUnitScope		inScope,
											  AudioUnitElement	inElement,
											  UInt32 &		outDataSize,
											  Boolean &		outWritable)
{
	if (inScope == kAudioUnitScope_Global) 
	{
		switch (inID) 
		{
			case kAudioUnitProperty_CocoaUI:
				outWritable = false;
				outDataSize = sizeof (AudioUnitCocoaViewInfo);
				return noErr;

			case kAudioUnitCustomProperty_FilterFrequencyResponse:	// our custom property
				if(inScope != kAudioUnitScope_Global ) return kAudioUnitErr_InvalidScope;
				outDataSize = kNumberOfResponseFrequencies * sizeof(FrequencyResponse);
				outWritable = false;
				return noErr;
		}
	}
	
	return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MadMonk::GetProperty
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus			MadMonk::GetProperty(	AudioUnitPropertyID inID,
										 AudioUnitScope 		inScope,
										 AudioUnitElement 	inElement,
										 void *				outData )
{
	if (inScope == kAudioUnitScope_Global) 
	{
		switch (inID) 
		{
			case kAudioUnitProperty_CocoaUI:
			{
				// Look for a resource in the main bundle by name and type.
				CFBundleRef bundle = CFBundleGetBundleWithIdentifier( CFSTR("com.loud.audiounit.MadMonk") );
				
				if (bundle == NULL) return fnfErr;
                
				CFURLRef bundleURL = CFBundleCopyResourceURL( bundle, 
															 CFSTR("MadMonk_CocoaViewFactory"), 
															 CFSTR("bundle"), 
															 NULL);
                
                if (bundleURL == NULL) return fnfErr;
				
				AudioUnitCocoaViewInfo cocoaInfo;
				cocoaInfo.mCocoaAUViewBundleLocation = bundleURL;
				cocoaInfo.mCocoaAUViewClass[0] = CFStringCreateWithCString(NULL, "MadMonk_CocoaViewFactory", kCFStringEncodingUTF8);
				
				*((AudioUnitCocoaViewInfo *)outData) = cocoaInfo;
				
				return noErr;
			}
				
				
			// This is our custom property which reports the current frequency response curve
			//
			case kAudioUnitCustomProperty_FilterFrequencyResponse:
			{
				if(inScope != kAudioUnitScope_Global) 	return kAudioUnitErr_InvalidScope;
				
					// the kernels are only created if we are initialized
					// since we're using the kernels to get the curve info, let
					// the caller know we can't do it if we're un-initialized
					// the UI should check for the error and not draw the curve in this case
				if(!IsInitialized() ) return kAudioUnitErr_Uninitialized;
				
				FrequencyResponse *freqResponseTable = ((FrequencyResponse*)outData);
				
					// each of our filter kernel objects (one per channel) will have an identical frequency response
					// so we arbitrarilly use the first one...
					//
				MadMonk *madMonk = dynamic_cast<MadMonk*>(mKernelList[0]);
				
				
				double cutoff = GetParameter(kFilterParam_CutoffFrequency);
				double resonance = GetParameter(kFilterParam_Resonance );
				
				float srate = GetSampleRate();
				
				cutoff = 2.0 * cutoff / srate;
				if(cutoff > 0.99) cutoff = 0.99;		// clip cutoff to highest allowed by sample rate...
				
				madMonk->CalculateLopassParams(cutoff, resonance);
				
				for(int i = 0; i < kNumberOfResponseFrequencies; i++ )
				{
					double frequency = freqResponseTable[i].mFrequency;
					
					freqResponseTable[i].mMagnitude = madMonk->GetFrequencyResponse(frequency);
				}
				
				return noErr;
			}
				
		}
	}
	
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}


#pragma mark ____MadMonkEffectKernel


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MadMonk::MadMonkKernel::Reset()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void		MadMonk::MadMonkKernel::Reset()
{
}

///////////////////////
/////////////////////////


///////////////
//process buffer
////////////////

OSStatus MadMonk::ProcessBufferLists(
									 AudioUnitRenderActionFlags &ioActionFlags,
									 const AudioBufferList &inBufferList,
									 AudioBufferList &outBufferList,
									 UInt32 inFramesToProcess )
{

    // silent = ioActionFlags & kAudioUnitRenderAction_OutputIsSilence;
	
    const AudioBuffer *srcBuffer = inBufferList.mBuffers;
    AudioBuffer *destBuffer = outBufferList.mBuffers;
	int framesToProcess = (int)inFramesToProcess;

	//Source - input
	float *src1 = (float *) srcBuffer[0].mData;
    float *src2 = (float *) srcBuffer[1 % inBufferList.mNumberBuffers].mData;

	//Destination - output
    float *dst1 = (float *) destBuffer[0].mData;
    float *dst2 = (float *) destBuffer[1 % outBufferList.mNumberBuffers].mData;
	
	//Output buffers for processed audio
	float * outputL1 = new float[inFramesToProcess];
	float * outputR1 = new float[inFramesToProcess];
	float * outputL2 = new float[inFramesToProcess];
	float * outputR2 = new float[inFramesToProcess];
	
	int semitonesVoiceOne=0;
	int semitonesVoiceTwo=0;
	
		//printf("auto/man: %f\n", GetParameter(kParam_AutoMan));
	
	if(GetParameter(kParam_AutoMan) == 0)
	{
		
		//render auto pitch detection and return semitone value for interval, given the key and mode
		string keySetting = keys[(int)GetParameter(kParam_Key)];
			//printf("keySetting: %s\n", keySetting.c_str());
		string keyType = keyTypes[(int)GetParameter(kParam_KeyType)];
			//printf("keyType: %s\n", keyType.c_str()); 
		string minorType = minorTypes[(int)GetParameter(kParam_MinorType)];
			//printf("minorType: %s\n", minorType.c_str());
		int voiceOneInterval = (int)GetParameter(kParam_VoiceOneAutoInterval);
		int voiceTwoInterval = (int)GetParameter(kParam_VoiceTwoAutoInterval);
		
		
		semitonesVoiceOne = autoPitchSemiTone((int)inFramesToProcess, src1, voiceOneInterval, keySetting,keyType,minorType,"none");
		//printf("semitonesVoiceOne -----: %d\n",semitonesVoiceOne);
		
		//note not found, so use last semitone value
		if(semitonesVoiceOne < 0)
		{
			semitonesVoiceOne = lastSemitoneVoiceOne;
		}
		else
		{
			lastSemitoneVoiceOne = semitonesVoiceOne;
		}
		
		semitonesVoiceTwo = autoPitchSemiTone((int)inFramesToProcess, src1, voiceTwoInterval, keySetting,keyType,minorType,"none");
		//printf("semitonesVoiceTwo -----: %d\n",semitonesVoiceTwo);
		
		
		if(semitonesVoiceTwo < 0)
		{
			semitonesVoiceTwo = lastSemitoneVoiceTwo;
		}
		else
		{
			lastSemitoneVoiceTwo = semitonesVoiceTwo;
		}
		
	}
	else
	{
		//get manual pitch shift values
		int tmpdelbuf = GetParameter(kParam_VoiceOneInterval);
		semitonesVoiceOne = tmpdelbuf;							
		
		int tmpdelbuf2 = GetParameter(kParam_VoiceTwoInterval);
		semitonesVoiceTwo = tmpdelbuf2;							
	}
	
	
	//Octave setting
	if(GetParameter(kParam_VoiceOneOctave)==1)
	{
		semitonesVoiceOne = getOctave(semitonesVoiceOne);
	}
	
	if(GetParameter(kParam_VoiceTwoOctave)==1)
	{
		semitonesVoiceTwo = getOctave(semitonesVoiceTwo);
	}
	
	//Create pitch shift values
	float pitchShift1 = pow(2., semitonesVoiceOne/12.);
	//printf("pitchShift1: %f\n", pitchShift1);

	float pitchShift2 = pow(2., semitonesVoiceTwo/12.);
	
	//Params - may need to adjust for user's settings
	int frameSize = 512 * 2;//1024;
	int overSampling = 4;
	
	//Call pitch shift function - two stereo pairs
	smbPitchShift1(pitchShift1, inFramesToProcess, frameSize, overSampling, CurrentSampleRate, src1 , outputL1);//left channel
	smbPitchShift2(pitchShift1, inFramesToProcess, frameSize, overSampling, CurrentSampleRate, src2 ,outputR1);//right channel
	
	smbPitchShift3(pitchShift2, inFramesToProcess, frameSize, overSampling, CurrentSampleRate, src1 , outputL2);//left channel
	smbPitchShift4(pitchShift2, inFramesToProcess, frameSize, overSampling, CurrentSampleRate, src2 ,outputR2);//right channel

		//panning
	float sourcePanVal = GetParameter( kParam_SourcePan );
	float v1PanVal = GetParameter( kParam_VoiceOnePan );
	float v2PanVal = GetParameter( kParam_VoiceTwoPan );
	
	float pSrc,pV1,pV2;
	pSrc=M_PI*(sourcePanVal+1)/4; 
	pV1 = M_PI*(v1PanVal+1)/4;
	pV2 = M_PI*(v2PanVal+1)/4;
	
	//Set levels
	for(int i=0; i < framesToProcess; i++)
	{
	
		//set gain on processed and clean buffers
		dst1[i] =(outputL1[i] * (GetParameter( kParam_VoiceOneLevel ) * cos(pV1))) 
			+ (outputL2[i] * (GetParameter( kParam_VoiceTwoLevel ) * cos(pV2))) 
			+ (src1[i] * (GetParameter( kParam_CleanLevel ) * cos(pSrc)));
		dst2[i] =(outputR1[i] * (GetParameter( kParam_VoiceOneLevel ) * sin(pV1))) 
			+ (outputR2[i] * (GetParameter( kParam_VoiceTwoLevel ) * sin(pV2))) 
			+ (src2[i] * (GetParameter( kParam_CleanLevel ) * sin(pSrc)));

	}

		//CLEANUP
	delete [] outputL1;
	delete [] outputR1;
	delete [] outputL2;
	delete [] outputR2;
	
	return noErr;
}

//Octave
float MadMonk::getOctave(float pitchValue)
{
	int pitchVal = (int)pitchValue;
	//NSLog(@"voice 1 pitch: %d",pitchVal);
	
	float octValue = 0;
	
	if(pitchVal > 0)
	{
		octValue = pitchVal - 12;
	}
	else
	{
		octValue = pitchVal + 12;
	}
	
	return octValue;
	
}

	//////////////////////////////////
///////////pitch detection ///////////
//////////////////////////////////////
 //Setup our FFT 
void MadMonk::realFFTSetup() {
	int maxFrames = (int)MAXFRAMES; //2048 * 4;//increase the size of the FFT to get a better frequency resolution
	dataBuffer = (void*)malloc(maxFrames * sizeof(SInt16));
	//outputBuffer = (float*)malloc(maxFrames *sizeof(float));
	log2n = log2(maxFrames);
	n = 1 << log2n;
	assert(n == maxFrames);
	nOver2 = maxFrames/2;
	bufferCapacity = maxFrames;
	indexPitch = 0;
	A.realp = (float *)malloc(nOver2 * sizeof(float));
	A.imagp = (float *)malloc(nOver2 * sizeof(float));
	fftSetup = vDSP_create_fftsetup(log2n, FFT_RADIX2);
	
	printf("n : %d\n", n );
	printf("log2n = %d\n", log2n );
}

float MadMonk::MagnitudeSquared(float x, float y) {
	return ((x*x) + (y*y));
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	FilterKernel::Reset()
//
//		It's very important to fully reset all filter state variables to their
//		initial settings here.  For delay/reverb effects, the delay buffers must
//		also be cleared here.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void		MadMonk::Reset()
{
	mX1 = 0.0;
	mX2 = 0.0;
	mY1 = 0.0;
	mY2 = 0.0;
	
		// forces filter coefficient calculation
	mLastCutoff = -1.0;
	mLastResonance = -1.0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MadMonk::CalculateLopassParams()
//
//		inFreq is normalized frequency 0 -> 1
//		inResonance is in decibels
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MadMonk::CalculateLopassParams(	double inFreq,
										 double inResonance )
{
    double r = pow(10.0, 0.05 * -inResonance);		// convert from decibels to linear
    
    double k = 0.5 * r * sin(M_PI * inFreq);
    double c1 = 0.5 * (1.0 - k) / (1.0 + k);
    double c2 = (0.5 + c1) * cos(M_PI * inFreq);
    double c3 = (0.5 + c1 - c2) * 0.25;
    
    mA0 = 2.0 *   c3;
    mA1 = 2.0 *   2.0 * c3;
    mA2 = 2.0 *   c3;
    mB1 = 2.0 *   -c2;
    mB2 = 2.0 *   c1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MadMonk::GetFrequencyResponse()
//
//		returns scalar magnitude response
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double MadMonk::GetFrequencyResponse( double inFreq /* in Hertz */ )
{
	float srate = GetSampleRate();
	double scaledFrequency = 2.0 * inFreq / srate;
	
		// frequency on unit circle in z-plane
	double zr = cos(M_PI * scaledFrequency);
	double zi = sin(M_PI * scaledFrequency);
	
		// zeros response
	double num_r = mA0*(zr*zr - zi*zi) + mA1*zr + mA2;
	double num_i = 2.0*mA0*zr*zi + mA1*zi;
	double num_mag = sqrt(num_r*num_r + num_i*num_i);
	
		// poles response
	double den_r = zr*zr - zi*zi + mB1*zr + mB2;
	double den_i = 2.0*zr*zi + mB1*zi;
	double den_mag = sqrt(den_r*den_r + den_i*den_i);
	
		// total response
	double response = num_mag  / den_mag;
	
	return response;
}


								
int MadMonk::autoPitchSemiTone(int numFrames, 
							   float * dataBufferInput, 
							   int intervalValue, 
							   string key, 
							   string keyType, 
							   string minorType, 
							   string mode) 
{

	static float dataBufferOP[MAXFRAMES];
	static float filterOutput[MAXFRAMES];
	
	memset(dataBufferOP, 0, MAXFRAMES*sizeof(float));
	memset(filterOutput, 0, MAXFRAMES*sizeof(float));
	
	uint32_t stride = 1;
	float detectedFreq;
	int voiceSemitone=0;
	float dominantFrequency = 0;

	/////////////////filter////////////////////////////////////////
	double cutoff = GetParameter(kFilterParam_CutoffFrequency);
    double resonance = GetParameter(kFilterParam_Resonance );
	
		// do bounds checking on parameters
    if(cutoff < kMinCutoffHz) cutoff = kMinCutoffHz;
	if(resonance < kMinResonance ) resonance = kMinResonance;
	if(resonance > kMaxResonance ) resonance = kMaxResonance;
	
		// convert to 0->1 normalized frequency
	float srate = GetSampleRate();

	cutoff = 2.0 * cutoff / srate;
	if(cutoff > 0.99) cutoff = 0.99;		// clip cutoff to highest allowed by sample rate...
	
		// only calculate the filter coefficients if the parameters have changed from last time
	if(cutoff != mLastCutoff || resonance != mLastResonance )
	{
		CalculateLopassParams(cutoff, resonance);
		mLastCutoff = cutoff;
		mLastResonance = resonance;	

	}
	
	const Float32 *sourceP = dataBufferInput;
	Float32 *destP = (Float32 *)filterOutput;
	int n = numFrames;
	
	// Apply the filter on the input and write to the output
	for(int f=0;f<numFrames;f++)
	{
		float input1 = *sourceP++;
		float output1 = mA0*input1 + mA1*mX1 + mA2*mX2 - mB1*mY1 - mB2*mY2;

		mX2 = mX1;
		mX1 = input1;
		mY2 = mY1;
		mY1 = output1;
		
		*destP++ = output1;
	}
	////////filter end ////////////

	
		//MAXFRAMES = 4096
	//Get input frequency (fourier transform)
	memcpy((float *)dataBufferOP, filterOutput, bufferCapacity*sizeof(float));
	
	//Copies the contents of an interleaved complex vector to a split complex vector
	vDSP_ctoz((COMPLEX*)dataBufferOP, 2, &A, 1, nOver2);

	// Carry out a Forward FFT transform.
	vDSP_fft_zrip(fftSetup, &A, stride, log2n, FFT_FORWARD);//original
	
		//absolute square?
		//vDSP_zvmags(&A,1,A.realp,1,numFrames/2);
		//bzero(A.imagp,(numFrames/2) * sizeof(float));

		//inverse fft
		//vDSP_fft_zrip(fftSetup, &A, stride, log2n, FFT_INVERSE);

		// Normalize
		//scale it by  2n. 
		//float scale = (float) 1.0 / (8 * n);
		//vDSP_vsmul(A.realp, 1, &scale, A.realp, 1, nOver2);
		//vDSP_vsmul(A.imagp, 1, &scale, A.imagp, 1, nOver2);

		//Copies the contents of a split complex vector A to an interleaved complex vector
	vDSP_ztoc(&A, 1, (COMPLEX *)dataBufferOP, 2, nOver2);//original
	
	
	// Determine the dominant frequency by taking the magnitude squared and 
	// saving the bin which it resides in.
	dominantFrequency = 0;
	int bin = -1.0; //peak index
	for (int i=0; i<n; i+=2) {
		float curFreq = MagnitudeSquared(dataBufferOP[i],dataBufferOP[i+1]);
			//printf("dataBufferOP[i]: %f\n", dataBufferOP[i]);
		
		if (curFreq > dominantFrequency) {
			dominantFrequency = curFreq;
			bin = (i+1)/2;
		}
	}

	//frequency in Hz
		detectedFreq = bin*((float)CurrentSampleRate/bufferCapacity);
		//printf("Detected frequency: %f\n", detectedFreq);

		//printf("intervalValue: %d\n", intervalValue);
		//printf("key: %s\n", key.c_str());
		//printf("keyType: %s\n", keyType.c_str());
		//printf("minorType: %s\n", minorType.c_str());
	
	voiceSemitone = voiceOneSemitone.getSemitone(detectedFreq, intervalValue, key,keyType,minorType,mode);

	Globals()->SetParameter(kParam_SourceNote, voiceOneSemitone.rootNoteIndex);

	//cleanup
	memset(dataBufferOP, 0,MAXFRAMES*sizeof(float));
	memset(filterOutput, 0,MAXFRAMES*sizeof(float));
	memset(destP,0,MAXFRAMES*sizeof(Float32));
	
	return voiceSemitone;

}

//////////////// end pitch detection //////////////////////



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	MadMonk::MadMonkKernel::Process
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MadMonk::MadMonkKernel::Process(	const Float32 	*inSourceP,
									 Float32		 	*inDestP,
									 UInt32 			inFramesToProcess,
									 UInt32			inNumChannels, // for version 2 AudioUnits inNumChannels is always 1
									 bool			&ioSilence )
{
	/*
	
	 //This code will pass-thru the audio data.
	 //This is where you want to process data to produce an effect.
	//printf("process method...");
	
	 UInt32 nSampleFrames = inFramesToProcess;
	 const Float32 *sourceP = inSourceP;
	 Float32 *destP = inDestP;
	Float32 gain = 0.7;//GetParameter( kParam_One );
	
	//pitch shift
	int tmpdelbuf = GetParameter( kParam_Four);
	long semitones = tmpdelbuf;							// shift up by 3 semitones
	float pitchShift = pow(2., semitones/12.);

	smbPitchShift(pitchShift, inFramesToProcess , 1024, 4, 44100, (float *)sourceP , (float *)destP);

	 
	 while (nSampleFrames-- > 0) {
	 Float32 inputSample = *sourceP;
	 
	 //The current (version 2) AudioUnit specification *requires* 
	 //non-interleaved format for all inputs and outputs. Therefore inNumChannels is always 1
	 
	 sourceP += inNumChannels;	// advance to next frame (e.g. if stereo, we're advancing 2 samples);
	 // we're only processing one of an arbitrary number of interleaved channels
	 
	 // here's where you do your DSP work
	 Float32 outputSample = inputSample * gain;
	 
	 *destP = outputSample;
	 destP += inNumChannels;
	 }
	*/
}


