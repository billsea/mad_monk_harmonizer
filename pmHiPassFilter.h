
#ifndef __pm_HiPassFilter__
#define __pm_HiPassFilter__
/*
 *  pmHiPassFilter.h
 *  Highpass filter class
 *
 *  Created by Bill on 5/27/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

namespace primoHPFilter
{

class pm_HiPassFilter
{
public:
	pm_HiPassFilter();
	
	void CalculateHipassParams(	double inFreq, double inResonance );
	float processInput(float input);
	void reset();
	
//properties

	private:
		
	//highpass filter coefficients
	double 	Hp_a1;
	double	Hp_a2;
	double	Hp_a3;
	double	Hp_b1;
	double	Hp_b2;
	
	// filter state
	
	double	mX1; 
	double	mX2;
	double	mY1;
	double	mY2;
	
};
}
#endif	// end __SIMPLE_LIMIT_H__