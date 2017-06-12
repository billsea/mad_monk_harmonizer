/*
*	File:		MadMonk.h
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

#include "AUEffectBase.h"
#include "MadMonkVersion.h"
#include <string>

#if AU_DEBUG_DISPATCHER
	#include "AUDebugDispatcher.h"
#endif


#ifndef __MadMonk_h__
#define __MadMonk_h__

using namespace std;

#pragma mark ____MadMonk Parameters
#define kNumberOfResponseFrequencies 512

// parameters
static const float kDefaultValue_VoiceOneLevel = 0.5;
static const float kDefaultValue_VoiceTwoLevel = 0.5;
static const float kDefaultValue_CleanLevel = 0.5;

//sliders for pitch and level
static CFStringRef kParameterVoiceOneLevel = CFSTR("Voice 1 Level");
static CFStringRef kParameterVoiceTwoLevel = CFSTR("Voice 2 Level");
static CFStringRef kParameterCleanLevel = CFSTR("Clean Level");
static CFStringRef kParamName_SourcePan = CFSTR("Source Pan");
static CFStringRef kParamName_VoiceOnePan = CFSTR("Voice One Pan");
static CFStringRef kParamName_VoiceTwoPan = CFSTR("Voice Two Pan");
//dropdown settings
static CFStringRef kParamName_AutoMan  = CFSTR ("Auto/Manual");     
static const int kParamName_AutoMan1     = 0;
static const int kParamName_AutoMan2  = 1;
static const int kDefaultValue_kParamName_AutoMan = kParamName_AutoMan1;
static CFStringRef kMenuItem_Auto       = CFSTR ("Auto");         
static CFStringRef kMenuItem_Manual    = CFSTR ("Manual");  

//voice one interval
static CFStringRef kParamName_VoiceOneInterval  = CFSTR ("Voice One Interval");     
static const int kParamName_VoiceOneInterval1     = 0;
static const int kParamName_VoiceOneInterval2  = 1;
static const int kParamName_VoiceOneInterval3  = 2;
static const int kParamName_VoiceOneInterval4  = 3;
static const int kParamName_VoiceOneInterval5  = 4;
static const int kParamName_VoiceOneInterval6  = 5;
static const int kParamName_VoiceOneInterval7  = 6;
static const int kParamName_VoiceOneInterval8  = 7;
static const int kParamName_VoiceOneInterval9  = 8;
static const int kParamName_VoiceOneInterval10  = 9;
static const int kParamName_VoiceOneInterval11  = 10;
static const int kParamName_VoiceOneInterval12  = 11;

static const int kDefaultValue_kParamName_VoiceOneInterval = kParamName_VoiceOneInterval1;
static CFStringRef kMenuItem_VoiceOneIntervalUnison   = CFSTR ("Unison");
static CFStringRef kMenuItem_VoiceOneIntervalMinorSecond       = CFSTR ("Minor 2nd");         
static CFStringRef kMenuItem_VoiceOneIntervalMajorSecond   = CFSTR ("Major 2nd"); 
static CFStringRef kMenuItem_VoiceOneIntervalMinorThird   = CFSTR ("Minor 3rd");
static CFStringRef kMenuItem_VoiceOneIntervalMajorThird   = CFSTR ("Major 3rd");
static CFStringRef kMenuItem_VoiceOneIntervalPerfectFourth   = CFSTR ("Perfect 4th");
static CFStringRef kMenuItem_VoiceOneIntervalDimFifth   = CFSTR ("Diminished 5th");
static CFStringRef kMenuItem_VoiceOneIntervalPerfectFifth   = CFSTR ("Perfect 5th");
static CFStringRef kMenuItem_VoiceOneIntervalMinorSixth   = CFSTR ("Minor 6th");
static CFStringRef kMenuItem_VoiceOneIntervalMajorSixth  = CFSTR ("Major 6th");
static CFStringRef kMenuItem_VoiceOneIntervalMinorSeventh   = CFSTR ("Minor 7th");
static CFStringRef kMenuItem_VoiceOneIntervalMajorSeventh = CFSTR ("Major 7th");

//voice one octave
static CFStringRef kParamName_VoiceOneOctave  = CFSTR ("Voice One Octave");     
static const int kParamName_VoiceOneOctave1     = 0;
static const int kParamName_VoiceOneOctave2  = 1;
static const int kDefaultValue_kParamName_VoiceOneOctave = kParamName_VoiceOneOctave1;
static CFStringRef kMenuItem_VoiceOneOctaveOff       = CFSTR ("Off");         
static CFStringRef kMenuItem_VoiceOneOctaveOn    = CFSTR ("On"); 

//voice one Auto - interval
static CFStringRef kParamName_VoiceOneAutoInterval  = CFSTR ("Voice One Auto Interval");     
static const int kParamName_VoiceOneAutoInterval1  = 0;
static const int kParamName_VoiceOneAutoInterval2  = 1;
static const int kParamName_VoiceOneAutoInterval3  = 2;
static const int kParamName_VoiceOneAutoInterval4  = 3;
static const int kParamName_VoiceOneAutoInterval5  = 4;
static const int kParamName_VoiceOneAutoInterval6  = 5;
static const int kParamName_VoiceOneAutoInterval7  = 6;


static const int kDefaultValue_kParamName_VoiceOneAutoInterval = kParamName_VoiceOneAutoInterval1;
static CFStringRef kMenuItem_VoiceOneAutoIntervalUnison   = CFSTR ("Unison");
static CFStringRef kMenuItem_VoiceOneAutoIntervalSecond   = CFSTR ("2nd");
static CFStringRef kMenuItem_VoiceOneAutoIntervalThird   = CFSTR ("3rd");
static CFStringRef kMenuItem_VoiceOneAutoIntervalFourth   = CFSTR ("4th");
static CFStringRef kMenuItem_VoiceOneAutoIntervalFifth   = CFSTR ("5th");
static CFStringRef kMenuItem_VoiceOneAutoIntervalSixth   = CFSTR ("6th");
static CFStringRef kMenuItem_VoiceOneAutoIntervalSeventh   = CFSTR ("7th");


//voice two interval
static CFStringRef kParamName_VoiceTwoInterval  = CFSTR ("Voice Two Interval");     
static const int kParamName_VoiceTwoInterval1     = 0;
static const int kParamName_VoiceTwoInterval2  = 1;
static const int kParamName_VoiceTwoInterval3  = 2;
static const int kParamName_VoiceTwoInterval4  = 3;
static const int kParamName_VoiceTwoInterval5  = 4;
static const int kParamName_VoiceTwoInterval6  = 5;
static const int kParamName_VoiceTwoInterval7  = 6;
static const int kParamName_VoiceTwoInterval8  = 7;
static const int kParamName_VoiceTwoInterval9  = 8;
static const int kParamName_VoiceTwoInterval10  = 9;
static const int kParamName_VoiceTwoInterval11  = 10;
static const int kParamName_VoiceTwoInterval12  = 11;

static const int kDefaultValue_kParamName_VoiceTwoInterval = kParamName_VoiceTwoInterval1;
static CFStringRef kMenuItem_VoiceTwoIntervalUnison   = CFSTR ("Unison");
static CFStringRef kMenuItem_VoiceTwoIntervalMinorSecond       = CFSTR ("Minor 2nd");         
static CFStringRef kMenuItem_VoiceTwoIntervalMajorSecond   = CFSTR ("Major 2nd"); 
static CFStringRef kMenuItem_VoiceTwoIntervalMinorThird   = CFSTR ("Minor 3rd");
static CFStringRef kMenuItem_VoiceTwoIntervalMajorThird   = CFSTR ("Major 3rd");
static CFStringRef kMenuItem_VoiceTwoIntervalPerfectFourth   = CFSTR ("Perfect 4th");
static CFStringRef kMenuItem_VoiceTwoIntervalDimFifth   = CFSTR ("Diminished 5th");
static CFStringRef kMenuItem_VoiceTwoIntervalPerfectFifth   = CFSTR ("Perfect 5th");
static CFStringRef kMenuItem_VoiceTwoIntervalMinorSixth   = CFSTR ("Minor 6th");
static CFStringRef kMenuItem_VoiceTwoIntervalMajorSixth  = CFSTR ("Major 6th");
static CFStringRef kMenuItem_VoiceTwoIntervalMinorSeventh   = CFSTR ("Minor 7th");
static CFStringRef kMenuItem_VoiceTwoIntervalMajorSeventh = CFSTR ("Major 7th");

//voice two Auto - interval
static CFStringRef kParamName_VoiceTwoAutoInterval  = CFSTR ("Voice Two Auto Interval");     
static const int kParamName_VoiceTwoAutoInterval1  = 0;
static const int kParamName_VoiceTwoAutoInterval2  = 1;
static const int kParamName_VoiceTwoAutoInterval3  = 2;
static const int kParamName_VoiceTwoAutoInterval4  = 3;
static const int kParamName_VoiceTwoAutoInterval5  = 4;
static const int kParamName_VoiceTwoAutoInterval6  = 5;
static const int kParamName_VoiceTwoAutoInterval7  = 6;

//voice one octave
static CFStringRef kParamName_VoiceTwoOctave  = CFSTR ("Voice Two Octave");     
static const int kParamName_VoiceTwoOctave1     = 0;
static const int kParamName_VoiceTwoOctave2  = 1;
static const int kDefaultValue_kParamName_VoiceTwoOctave = kParamName_VoiceTwoOctave1;
static CFStringRef kMenuItem_VoiceTwoOctaveOff       = CFSTR ("Off");         
static CFStringRef kMenuItem_VoiceTwoOctaveOn    = CFSTR ("On"); 


static const int kDefaultValue_kParamName_VoiceTwoAutoInterval = kParamName_VoiceTwoAutoInterval1;
static CFStringRef kMenuItem_VoiceTwoAutoIntervalUnison   = CFSTR ("Unison");
static CFStringRef kMenuItem_VoiceTwoAutoIntervalSecond   = CFSTR ("2nd");
static CFStringRef kMenuItem_VoiceTwoAutoIntervalThird   = CFSTR ("3rd");
static CFStringRef kMenuItem_VoiceTwoAutoIntervalFourth   = CFSTR ("4th");
static CFStringRef kMenuItem_VoiceTwoAutoIntervalFifth   = CFSTR ("5th");
static CFStringRef kMenuItem_VoiceTwoAutoIntervalSixth   = CFSTR ("6th");
static CFStringRef kMenuItem_VoiceTwoAutoIntervalSeventh   = CFSTR ("7th");


static CFStringRef kParamName_MinorType  = CFSTR ("Minor Type");     
static const int kParamName_MinorType1  = 0;
static const int kParamName_MinorType2  = 1;
static const int kParamName_MinorType3  = 2;
static const int kDefaultValue_kParamName_MinorType = kParamName_MinorType1;
static CFStringRef kMenuItem_Natural       = CFSTR ("Natural");         
static CFStringRef kMenuItem_Harmonic    = CFSTR ("Harmonic");
static CFStringRef kMenuItem_Melodic    = CFSTR ("Melodic");

	//key type///////
static CFStringRef kParamName_KeyType  = CFSTR ("Key Type");     
static const int kParamName_KeyType1     = 0;
static const int kParamName_KeyType2  = 1;
static const int kDefaultValue_kParamName_KeyType = kParamName_KeyType1;
static CFStringRef kMenuItem_Major       = CFSTR ("Major");         
static CFStringRef kMenuItem_Minor    = CFSTR ("Minor");       



static CFStringRef kParamName_Key  = CFSTR ("Key");     
static const int kParamName_Key1     = 0;
static const int kParamName_Key2  = 1;
static const int kParamName_Key3  = 2;
static const int kParamName_Key4  = 3;
static const int kParamName_Key5  = 4;
static const int kParamName_Key6  = 5;
static const int kParamName_Key7  = 6;
static const int kParamName_Key8  = 7;
static const int kParamName_Key9  = 8;
static const int kParamName_Key10  = 9;
static const int kParamName_Key11  = 10;
static const int kParamName_Key12  = 11;
static const int kDefaultValue_kParamName_Key = kParamName_Key1;
static CFStringRef kMenuItem_C       = CFSTR ("C");         
static CFStringRef kMenuItem_Cs    = CFSTR ("C#");
static CFStringRef kMenuItem_D    = CFSTR ("D");
static CFStringRef kMenuItem_Ds    = CFSTR ("D#");
static CFStringRef kMenuItem_E    = CFSTR ("E");
static CFStringRef kMenuItem_F    = CFSTR ("F");
static CFStringRef kMenuItem_Fs    = CFSTR ("F#");
static CFStringRef kMenuItem_G    = CFSTR ("G");
static CFStringRef kMenuItem_Gs    = CFSTR ("G#");
static CFStringRef kMenuItem_A    = CFSTR ("A");
static CFStringRef kMenuItem_As    = CFSTR ("A#");
static CFStringRef kMenuItem_B    = CFSTR ("B");

static CFStringRef kParamName_SourceNote  = CFSTR ("Note");     
static const int kParamName_SourceNote1     = 0;
static const int kParamName_SourceNote2  = 1;
static const int kParamName_SourceNote3  = 2;
static const int kParamName_SourceNote4  = 3;
static const int kParamName_SourceNote5  = 4;
static const int kParamName_SourceNote6  = 5;
static const int kParamName_SourceNote7  = 6;
static const int kParamName_SourceNote8  = 7;
static const int kParamName_SourceNote9  = 8;
static const int kParamName_SourceNote10  = 9;
static const int kParamName_SourceNote11  = 10;
static const int kParamName_SourceNote12  = 11;
static const int kDefaultValue_kParamName_SourceNote = kParamName_SourceNote1;



	//these must be identical to the cocoa view params in name and index

enum {
	kParam_AutoMan=0,
	kParam_CleanLevel=1,
	kFilterParam_CutoffFrequency=2,
	kFilterParam_Resonance=3,
	kParam_SourcePan=4,
	kParam_Key=5,
	kParam_KeyType=6,
	kParam_MinorType=7,
	kParam_VoiceOneInterval=8,
	kParam_VoiceOneAutoInterval=9,
	kParam_VoiceOneOctave=10,
	kParam_VoiceOneLevel=11,
	kParam_VoiceOnePan=12,
	kParam_VoiceTwoInterval=13,
	kParam_VoiceTwoAutoInterval=14,
	kParam_VoiceTwoOctave=15,
	kParam_VoiceTwoLevel=16,
	kParam_VoiceTwoPan=17,
	kParam_SourceNote=18,
	kNumberOfParameters=19
};




#pragma mark ____MadMonk
class MadMonk : public AUEffectBase
{
	
public:
	MadMonk(AudioUnit component);
	virtual ComponentResult		Initialize();
	
#if AU_DEBUG_DISPATCHER
	virtual ~MadMonk () { delete mDebugDispatcher; }
	
#endif

	
	
	virtual AUKernelBase *		NewKernel() { return new MadMonkKernel(this); }
	
	virtual	OSStatus			GetParameterValueStrings(AudioUnitScope			inScope,
														 AudioUnitParameterID		inParameterID,
														 CFArrayRef *			outStrings);
    
	virtual	OSStatus			GetParameterInfo(AudioUnitScope			inScope,
												 AudioUnitParameterID	inParameterID,
												 AudioUnitParameterInfo	&outParameterInfo);
    
	virtual OSStatus			GetPropertyInfo(AudioUnitPropertyID		inID,
												AudioUnitScope			inScope,
												AudioUnitElement		inElement,
												UInt32 &			outDataSize,
												Boolean	&			outWritable );
	
	virtual OSStatus			GetProperty(AudioUnitPropertyID inID,
											AudioUnitScope 		inScope,
											AudioUnitElement 		inElement,
											void *			outData);
	
 	virtual	bool				SupportsTail () { return false; }
	
	/*! @method Version */
	virtual OSStatus		Version() { return kMadMonkVersion; }
	
	//overrides process method
	
	virtual OSStatus            ProcessBufferLists(
												   AudioUnitRenderActionFlags &	ioActionFlags,
												   const AudioBufferList &			inBuffer,
												   AudioBufferList &				outBuffer,
												   UInt32							inFramesToProcess );
	
	
	int autoPitchSemiTone(int numFrames, 
						  float * dataBufferInput, 
						  int intervalValue, 
						  string key, 
						  string keyType, 
						  string minorType, 
						  string mode);
	
	void realFFTSetup();
	float MagnitudeSquared(float x, float y);
	float getOctave(float pitchValue);
	

	
		//filter
		// Here we define a custom property so the view is able to retrieve the current frequency
		// response curve.  The curve changes as the filter's cutoff frequency and resonance are
		// changed...
	
		// custom properties id's must be 64000 or greater
		// see <AudioUnit/AudioUnitProperties.h> for a list of Apple-defined standard properties
		//
	enum
	{
		kAudioUnitCustomProperty_FilterFrequencyResponse = 65536
	};
	
		// We'll define our property data to be a size kNumberOfResponseFrequencies array of structs
		// The UI will pass in the desired frequency in the mFrequency field, and the Filter AU
		// will provide the linear magnitude response of the filter in the mMagnitude field
		// for each element in the array.
	typedef struct FrequencyResponse
	{
		Float64		mFrequency;
		Float64		mMagnitude;
	} FrequencyResponse;
	
	
	void				CalculateLopassParams(	double inFreq, double inResonance );
	
		// used by the custom property handled in the Filter class below
	double				GetFrequencyResponse( double inFreq );
	
		// resets the filter state
	virtual void		Reset();
	

private:
		// filter coefficients
	double	mA0;
	double	mA1;
	double	mA2;
	double	mB1;
	double	mB2;
	
		// filter state
	double	mX1;
	double	mX2;
	double	mY1;
	double	mY2;
	
	double	mLastCutoff;
	double	mLastResonance;

	
		//end filter///
	
protected:
		class MadMonkKernel : public AUKernelBase		// most real work happens here
	{
public:
		MadMonkKernel(AUEffectBase *inAudioUnit )
		: AUKernelBase(inAudioUnit)
	{

	}
	
		// *Required* overides for the process method for this effect
		// processes one channel of interleaved samples
        virtual void 		Process(	const Float32 	*inSourceP,
										Float32		 	*inDestP,
										UInt32 			inFramesToProcess,
										UInt32			inNumChannels,
										bool			&ioSilence);
	
		
		
		
        virtual void		Reset();
		
		//private: //state variables...
		
	
	};
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#endif
