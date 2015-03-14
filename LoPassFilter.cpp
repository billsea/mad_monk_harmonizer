//
//  LoPassFilter.m
//  voxReducerMobile
//
//  Created by bill on 10/27/09.
//  Copyright 2009 _LoudSoftware_. All rights reserved.
//

#import "LoPassFilter.h"
#include "math.h"


LoPassFilter::LoPassFilter()
{
	
	
}
void LoPassFilter::CalculateLopassParams(double inFreq, double inResonance,int sampleRate)
{
	/*
    double resonance = powf(10.0, 0.05 * -inResonance);		// convert from decibels to linear
    //printf("resonance: %f\n",resonance);
	
	float c = 1.0 / tanf(M_PI * inFreq);
	printf("---------c: %f\n",c);
	
	Lp_a1 = 1.0 / ( 1.0 + resonance * c + c * c);
	Lp_a2 = 2* Lp_a1;
	Lp_a3 = Lp_a1;
	Lp_b1 = 2.0 * ( 1.0 - c*c) * Lp_a1;
	Lp_b2 = ( 1.0 - resonance * c + c * c) * Lp_a1;
	*/
	
	double f = inFreq;
	double r = pow(10.0, 0.05 * -inResonance);// inResonance;
	int sample_rate=sampleRate;
	
	double c = 1.0 / tan(M_PI * f / sample_rate);
	
	Lp_a1 = 1.0 / ( 1.0 + r * c + c * c);
	Lp_a2 = 2 * Lp_a1;
	Lp_a3 = Lp_a1;
	Lp_b1 = 2.0 * ( 1.0 - c*c) * Lp_a1;
	Lp_b2 = ( 1.0 - r * c + c * c) * Lp_a1;
	
	
	///////
	
	/*
	double r = powf(10.0, 0.05 * -inResonance);		// convert from decibels to linear
    double k = 0.5 * r * sinf(M_PI * inFreq);
    double c1 = 0.5 * (1.0 - k) / (1.0 + k);
    double c2 = (0.5 + c1) * cosf(M_PI * inFreq);
    double c3 = (0.5 + c1 - c2) * 0.25f;

    Lp_a1 = 2.0 *   c3;
    Lp_a2 = 2.0 *   2.0 * c3;
    Lp_a3 = 2.0 *   c3;
    Lp_b1 = 2.0 *   -c2;
    Lp_b2 = 2.0 *   c1;
	*/
	
	//printf("---------r: %f\n",r);
	//printf("---------k: %f\n",k);
	//printf("---------c1: %f\n",c1 );
	//printf("---------c2: %f\n",c2 );
	//printf("---------c3: %f\n",c3 );
	printf("---------Lp_a1: %f\n",Lp_a1 );
	printf("---------Lp_a2: %f\n",Lp_a2 );
	printf("---------Lp_a3: %f\n",Lp_a3 );
	printf("---------Lp_b1: %f\n",Lp_b1 );
	printf("---------Lp_b2: %f\n",Lp_b2 );
	
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

		//printf("---------Lp_a1: %f\n",Lp_a1 );
		//printf("---------Lp_a2: %f\n",Lp_a2 );
		//printf("---------Lp_a3: %f\n",Lp_a3 );
		//printf("---------Lp_b1: %f\n",Lp_b1 );
		//printf("---------Lp_b2: %f\n",Lp_b2 );
		
		
		//update temp variables
		mX2 = mX1;
		mX1 = srcInput;
		mY2 = mY1;
		mY1 = output; //output lpass

		//*source++ = output;
		*dest++ = output;
		
		//printf("---------output: %f\n",output );
	}
	
	
}

void LoPassFilter::resetLo()
{
	
	mX1=0.0; 
	mX2=0.0;
	mY1=0.0;
	mY2=0.0;
	
}



