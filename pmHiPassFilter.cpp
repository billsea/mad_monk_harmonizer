/*
 *  pmHiPassFilter.cpp
 *  VoxRemover
 *
 *  Created by Bill on 5/27/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "pmHiPassFilter.h"
#include "math.h"

namespace primoHPFilter {

pm_HiPassFilter::pm_HiPassFilter()
{
	
}


void pm_HiPassFilter::CalculateHipassParams(	double inFreq,
																double inResonance )
{
    double resonance = pow(10.0, 0.05 * -inResonance);		// convert from decibels to linear
    
	float c = tan(M_PI * inFreq);
	Hp_a1 =  1.0 / ( 1.0 + resonance * c + c * c); //rez amount, from sqrt(2) to ~ 0.1
	Hp_a2 = -2*Hp_a1;
	Hp_a3 = Hp_a1;
	Hp_b1 =  2.0 * ( c*c - 1.0) * Hp_a1; // 
	Hp_b2 = ( 1.0 - resonance * c + c * c) * Hp_a1;
	
}

float pm_HiPassFilter::processInput(float input)
{
	float H = Hp_a1*input + Hp_a2*mX1 + Hp_a3*mX2 - Hp_b1*mY1 - Hp_b2*mY2;
	// (newCoefficient * new input) + (-2(newCoefficient) * lastInput) + (newCoefficient * lastInput2) - (newCoeff_b1 * lastOutput) -   (newCoeff_b2 * lastOutput2)                        
	
	//update temp variables
	mX2 = mX1;//   last input2 (two inputs ago)
	mX1 = input; //last input
	mY2 = mY1; //  last output2
	mY1 = H; //    last output	
	
	return H;
	
}

void pm_HiPassFilter::reset()
{
	mX1=0.0; 
	mX2=0.0;
	mY1=0.0;
	mY2=0.0;
	
}


} //end namespace