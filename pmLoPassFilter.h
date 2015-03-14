/*
 *  pmLoPassFilter.h
 *  Lowpass filter class
 *
 *  Created by Bill on 5/27/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "math.h"
#include <iostream>

class pm_LoPassFilter
{
public:
	pm_LoPassFilter();
	
	void CalculateLopassParams(	double inFreq, double inResonance );
	float processInput(float input);
	void reset();
	
//Properties

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

