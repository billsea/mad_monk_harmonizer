/*
 *  loudDelay.cpp
 *
 *  Created by bill on 12/1/10.
 *  Copyright 2010 _LoudSoftware_. All rights reserved.
 *
 */

#include "loudDelay.h"



#include "loudDelay.h"
#include "AUEffectBase.h"
#include "math.h"



loudDelay::loudDelay()
{
	const float pkMaxDelayTime =5.0;
	loudDelay::setSampleRate(44100);
	pmMaxDelayFrames = int(sampRate * pkMaxDelayTime + 10);
	delayBuffer_L.AllocateClear(pmMaxDelayFrames);
	delayBuffer_R.AllocateClear(pmMaxDelayFrames);
	pmWriteIndex = pmMaxDelayFrames - 1;
	
	
	printf("delay init...");
	
}


void loudDelay::processInput(Float32 &srcInL, Float32 &srcInR)
{
	//printf("process Delay...");
	
	
	//single parameter values
	float	level;
	int		readIndex;
	float phaseGain = 1.0; 
	
	int frames1 = delayTime;// * (sampRate); //delay frames
	//printf("--frames=%d",frames1);
	if (frames1 < 1) frames1 = 1;
	else if (frames1 >= pmMaxDelayFrames) frames1 = pmMaxDelayFrames - 1;
	
	
	level = phaseGain;
	readIndex = pNormalizeIndex(pmWriteIndex - frames1);	// read head lags write head...
	
	// intensity
	float wetDry = -1 * ((wdLevel / 100.) - 1);	// percent to ratio - inverted
	float wet = sqrt(wetDry), dry = sqrt(1.0 - wetDry);
	
	
	//write to audio buffer index
	delayBuffer_L[pmWriteIndex] = srcInL;
	delayBuffer_R[pmWriteIndex] = srcInR;
	pmWriteIndex = (pmWriteIndex + 1) % pmMaxDelayFrames;
	
	float outputL = dry * srcInL ;
	float outputR = dry * srcInR ;
	
	
	//read buffer index
	int readIndex1 = readIndex;
	float delayReadL = delayBuffer_L[readIndex1];
	float delayReadR = delayBuffer_R[readIndex1];
	
	readIndex = (readIndex1 + 1) % pmMaxDelayFrames;
	
	outputL += delayReadL * level * wet ;
	outputR += delayReadR * level * wet ;
	
	srcInL = outputL; 
	srcInR = outputR;
	
	
}

void loudDelay::setWriteIndex( int wIndex )
{
	pmWriteIndex = wIndex;
}

int loudDelay::getWriteIndex()
{
	
	return pmWriteIndex;
}

int	loudDelay::pNormalizeIndex(int i) 
{
	return (i + pmMaxDelayFrames) % pmMaxDelayFrames;
}


void  loudDelay::setSampleRate( int sampleRate )
{
	sampRate = sampleRate;
	
}

void loudDelay::setDelayTime(Float32 delTime)
{
	delayTime = delTime;
}
void loudDelay::setWetDryLevel(Float32 level)
{
	wdLevel = level;
}




