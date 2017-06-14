/*
 *  scaleType.h
 *  MadMonk
 *
 *  Created by bill on 2/11/11.
 *  Copyright 2011 _LoudSoftware_. All rights reserved.
 *
 */
#ifndef _scaleType_H_
#define _scaleType_H_

#include <iostream>
#include <string>

using namespace std;

class scaleType {
	
public:
	scaleType();
	string scaleNotes;
	
private:
    int semitoneInterval;
    int scaleInterval;
};
#endif
