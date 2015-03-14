/*
 *  pmLoPassFilter.cpp
 *  VoxRemover
 *
 *  Created by Bill on 5/27/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "pmLoPassFilter.h"
#include "math.h"



pm_LoPassFilter::pm_LoPassFilter()
{
	
}

void pm_LoPassFilter::CalculateLopassParams(	double inFreq,
											double inResonance )
{
    double resonance = pow(10.0, 0.05 * -inResonance);		// convert from decibels to linear
    
	float c = 1.0 / tan(M_PI * inFreq);
	Lp_a1 = 1.0 / ( 1.0 + resonance * c + c * c);
	Lp_a2 = 2* Lp_a1;
	Lp_a3 = Lp_a1;
	Lp_b1 = 2.0 * ( 1.0 - c*c) * Lp_a1;
	Lp_b2 = ( 1.0 - resonance * c + c * c) * Lp_a1;
	
}

float pm_LoPassFilter::processInput(float input)

{
	//compute lowpass filter
	float L = Lp_a1 * input + Lp_a2*mX1 + Lp_a3*mX2 - Lp_b1*mY1 - Lp_b2*mY2;
	//update temp variables
	mX2 = mX1;
	mX1 = input;
	mY2 = mY1;
	mY1 = L; //output lpass
	//printf("---------inputSrc: %f\n", input);
	return L;
	
}

void pm_LoPassFilter::reset()
{
	mX1=0.0; 
	mX2=0.0;
	mY1=0.0;
	mY2=0.0;
	
}