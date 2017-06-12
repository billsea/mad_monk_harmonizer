/*
 *  semitoneGet.h
 *  MadMonk
 *
 *  Created by bill on 2/8/11.
 *  Copyright 2011 _LoudSoftware_. All rights reserved.
 *
 */

#ifndef _semitoneGet_H_
#define _semitoneGet_H_

#include <iostream>
#include <string>
#include <math.h>
#include "scaleType.h"

using namespace std;

class semitoneGet
{
	
public:
	semitoneGet();
	int getSemitone(float srcFrequency,int selInterval, string key, string keyType, string minorType, string mode);
	int rootNoteIndex;
	
private:
	bool FindClosestNote(double frequency, double &closestFrequency, string &noteName,string key, string keyType, string minorType, string mode);
	int findIntervalNote(int selInterval, string key, string keyType, string minorType, string mode,string rootNote);
	void buildScale(string key,string keyType, string minorType, string mode, scaleType scale[],int scaleElements);
	
	double round(double number);
	
};


#endif
