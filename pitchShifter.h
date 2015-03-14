/*
 *  pitchShifter.h
 *  MadMonk
 *
 *  Created by bill on 11/30/10.
 *  Copyright 2010 _LoudSoftware_. All rights reserved.
 *
 */

#ifndef _PITCHSHIFTER_H_
#define _PITCHSHIFTER_H_

#include "AUEffectBase.h"
#include "math.h"
#include <AudioToolbox/AudioUnitUtilities.h>



	
class pitchShifter
{
		
	public:
		pitchShifter();
		void pitchShifterInit(int nSampleRate,int bufFrames);
		void processInput(float * srcInL, float * srcInR,long numFrames);
		void setSampleRate( int nSampleRate );
		void setDryLevel(int dryLevelIn);
		void setWetLevel(int wetLevelIn);
		void setFeedbackLevel(int feedbackLevel);
		void setPitchShift(int pitchShift);
		void setIndices();
		void setSampleBufferSize(int bufFrames);
	
	//properties
	private:
		int sampleBufferSize;
		Float32 wdLevel;
		bool initializationComplete;
		float * localBufferL;
		float * delayBufferL;
		float * localBufferR;
		float * delayBufferR;
		bool sweepUp;
		double step;
		double sweep;
		int dryLevel;
		int wetLevel;
		int feedbackLevel;
		
		int sampleRate;
		int numberOfChannels;
		int delayBufferSize;
		int readIndexALow;
		int readIndexAHigh;
		int readIndexBLow;
		int readIndexBHigh;
		int writeIndex;
		int numberOfDelaySamples;
		int numberOfCrossFadeSamples;
		int crossFadeCount;
		int activeSampleCount;
		int activeCount;
		bool channelA;
		double blendA;
		double blendB;
		
		double * fadeIn;
		double * fadeOut;
		double * fadeA;
		double * fadeB;
};


#endif
