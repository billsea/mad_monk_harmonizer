/*
 *  keyDetect.h
 *  MadMonk
 *
 *  Created by bill on 2/5/11.
 *  Copyright 2011 _LoudSoftware_. All rights reserved.
 *
 */
#include <iostream>
#include <string>
#include "scaleType.h"


#ifndef _keyScales_H_
#define _keyScales_H_

using namespace std;

class keyScales
{
	
public:
	keyScales();
	void getMajorScale(string keyNote, string mode, scaleType scale[]);
	void getMinorScale(string keyNote,string mode, scaleType scale[],string minorType);

private:
	
};


#endif