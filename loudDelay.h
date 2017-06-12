/*
 *  loudDelay.h
 *
 *  Created by bill on 12/1/10.
 *  Copyright 2010 _LoudSoftware_. All rights reserved.
 *
 */

#ifndef _LOUDDELAY_H_
#define _LOUDDELAY_H_

#include "AUEffectBase.h"
#include "math.h"
#include <AudioToolbox/AudioUnitUtilities.h>




class loudDelay
{
	
public:
	loudDelay();
	void setDelayTime(Float32 delTime); 
	void setWetDryLevel(Float32 level);
	void processInput(Float32 &srcInL, Float32 &srcInR);
	void setSampleRate( int sampleRate );
	void setWriteIndex( int wIndex );
	int getWriteIndex();
	
	//properties
private:
	int	pNormalizeIndex(int i);
	TAUBuffer<Float32> delayBuffer_L;
	TAUBuffer<Float32> delayBuffer_R;
	Float32 delayTime;
	Float32 wdLevel;
	int	pmWriteIndex;
	int	pmMaxDelayFrames;
	int sampRate;
	
};


#endif
