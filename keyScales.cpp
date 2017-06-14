/*
 *  keyDetect.cpp
 *  MadMonk
 *
 *  Created by bill on 2/5/11.
 *  Copyright 2011 _LoudSoftware_. All rights reserved.
 *
 */

#include "keyScales.h"

//#define MaxScales 10
#define MaxNotes 8
#define Semitones 12

string semiToneNames[Semitones] = {"A","A#","B","C","C#","D","D#","E","F","F#","G","G#"};
int majIntervals[MaxNotes] = {0,2,2,1,2,2,2,1};//interval in semitones
int CIntervals[MaxNotes] = {0,2,3,4,5,6,7,8};//interval in chord (i.e root(0), third(3), fifth(5)
int minIntervals[MaxNotes] = {0,2,1,2,2,1,2,2};


keyScales::keyScales(){}

void keyScales::getMajorScale(string keyNote,string mode, scaleType scale[])
{
	string semitoneOrder[Semitones];
	int scaleIndex = 0;
	
	for(int ri = 0;ri<Semitones;ri++)//scan semitones and get key root note semitone index
	{
		if(semiToneNames[ri] == keyNote)
		{
			for(int i=ri;i<Semitones;i++)//build new order of semitones starting with key note
			{
				semitoneOrder[scaleIndex] = semiToneNames[i];
				scaleIndex++;

			}
			
			if(scaleIndex < Semitones)//wraps back to beginning of semitones array
			{
				int stopIndex = Semitones - scaleIndex;
				for(int i=0;i<stopIndex;i++)
				{
					semitoneOrder[scaleIndex] = semiToneNames[i];
					scaleIndex++;
					//printf("scaleIndex2-----: %s\n",semiToneNames[i].c_str());
				}
			}
			
			break;
		}
	}
	
	//determine major scale
	//get new major scale based on major interval array values
	int interval=0;
	
	for(int z=0;z<MaxNotes-1;z++)
	{
		interval += majIntervals[z];
		scale[z].scaleNotes = semitoneOrder[interval];//returns scale
		scale[z].semitoneInterval = interval;
		scale[z].scaleInterval = CIntervals[z];
		//printf("theScale-----: %s\n",scale[z].c_str());
	}
}

void keyScales::getMinorScale(string keyNote,string mode, scaleType scale[], string minorType)
{
	
	string semitoneOrder[Semitones];
	int scaleIndex = 0;
	
	for(int ri = 0;ri<Semitones;ri++)//scan semitones and get key root note semitone index
	{
		if(semiToneNames[ri] == keyNote)
		{
			for(int i=ri;i<Semitones;i++)//build new order of semitones starting with key note
			{
				semitoneOrder[scaleIndex] = semiToneNames[i];
				scaleIndex++;
			}
			
			if(scaleIndex < Semitones)//wraps back to beginning of semitones array
			{
				int stopIndex = Semitones - scaleIndex;
				for(int i=0;i<stopIndex;i++)
				{
					semitoneOrder[scaleIndex] = semiToneNames[i];
					scaleIndex++;
				}
			}
			
			break;
		}
		
		
	}
	
	//determine minor scale
	//get new minor scale based on type and minor interval array values
	int interval=0;
	
	for(int z=0;z<MaxNotes-1;z++)
	{
		interval += minIntervals[z];
		
		if(minorType == "Harmonic" && z==6)
		{
			
			scale[z].scaleNotes = semitoneOrder[interval + 1];//returns scale
			scale[z].semitoneInterval = interval + 1;
			scale[z].scaleInterval = CIntervals[z];
		}
		else if(minorType == "Melodic" && (z==5 || z==6))
		{
			scale[z].scaleNotes = semitoneOrder[interval + 1];//returns scale
			scale[z].semitoneInterval = interval + 1;
			scale[z].scaleInterval = CIntervals[z];
		}
		else
		{
			scale[z].scaleNotes = semitoneOrder[interval];//returns scale
			scale[z].semitoneInterval = interval;
			scale[z].scaleInterval = CIntervals[z];
		}
	}
}
