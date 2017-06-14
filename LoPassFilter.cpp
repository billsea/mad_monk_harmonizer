//
//  LoPassFilter.m
//  voxReducerMobile
//
//  Created by bill on 10/27/09.
//  Copyright 2009 _LoudSoftware_. All rights reserved.
//

#import "LoPassFilter.h"
#include "math.h"

LoPassFilter::LoPassFilter() {}
void LoPassFilter::CalculateLopassParams(double inFreq, double inResonance,int sampleRate)
{
	double f = inFreq;
	double r = pow(10.0, 0.05 * -inResonance);// inResonance;
	int sample_rate=sampleRate;
	
	double c = 1.0 / tan(M_PI * f / sample_rate);
	
	Lp_a1 = 1.0 / ( 1.0 + r * c + c * c);
	Lp_a2 = 2 * Lp_a1;
	Lp_a3 = Lp_a1;
	Lp_b1 = 2.0 * ( 1.0 - c*c) * Lp_a1;
	Lp_b2 = ( 1.0 - r * c + c * c) * Lp_a1;
}

void LoPassFilter::processInputLo(float *source, float *dest,int inFramesToProcess)
{
	//Float32 *destP = inDestP;
	int n = inFramesToProcess;
	
	while(n--)
	{
		float srcInput = *source++;
		
		//compute lowpass filter
		float output = Lp_a1*srcInput + Lp_a2*mX1 + Lp_a3*mX2 - Lp_b1*mY1 - Lp_b2*mY2;

		//update temp variables
		mX2 = mX1;
		mX1 = srcInput;
		mY2 = mY1;
		mY1 = output; //output lpass
		*dest++ = output;
	}
}

void LoPassFilter::resetLo()
{
	mX1=0.0; 
	mX2=0.0;
	mY1=0.0;
	mY2=0.0;
}



