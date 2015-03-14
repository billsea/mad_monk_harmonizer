//
//  LoPassFilter.h
//  voxReducerMobile
//
//  Created by bill on 10/27/09.
//  Copyright 2009 _LoudSoftware_. All rights reserved.
//

#include <iostream>
#include <string>


#ifndef _LoPassFilter_H_
#define _LoPassFilter_H_

using namespace std;

class LoPassFilter
{

	
public:
LoPassFilter();
void CalculateLopassParams(double inFreq, double inResonance,int sampleRate);
	void processInputLo(float *source, float *dest,int inFramesToProcess);
void resetLo();

	
private:
	
	//lowpass filter coefficients	
	double	Lp_a1;
	double	Lp_a2;
	double	Lp_a3;
	double	Lp_b1;
	double	Lp_b2;
	
	// filter state
	double	mX1; 
	double	mX2;
	double	mY1;
	double	mY2;


};
#endif
