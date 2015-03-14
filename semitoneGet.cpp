/*
 *  semitoneGet.cpp
 *  MadMonk
 *
 *  Created by bill on 2/8/11.
 *  Copyright 2011 _LoudSoftware_. All rights reserved.
 *
 */

#include "semitoneGet.h"
#include "keyScales.h"


#define NoteMax 12
#define ScaleNotes 7

string NoteNames[NoteMax] = {"A","A#","B","C","C#","D","D#","E","F","F#","G","G#"};
string cMajScale[7] = {"C","D","E","F","G","A","B"};
string inputNote="null";//for testing


keyScales scale1;


double ToneStep = pow(2, 1.0 / 12);

semitoneGet::semitoneGet()
{
	
}

void semitoneGet::buildScale(string key, string keyType, string minorType, string mode, scaleType scale[], int scaleElements)
{
	//if we designate 0 index as the key of C, and move through circle of fifths
	if(keyType=="Major")
	{
		scale1.getMajorScale(key,mode,scale);
	}
	else 
	{
		scale1.getMinorScale(key,mode,scale,minorType);
		
	}

	/*
	for(int i=0;i<scaleElements;i++)
	{
		printf("scale -----: %s\n",scale[i].scaleNotes.c_str());
		//printf("scale interval -----: %d\n",scale[i].scaleInterval);
		//printf("semitone interval -----: %d\n",scale[i].semitoneInterval);
	}
	 */
}

bool semitoneGet::FindClosestNote(double frequency, double &closestFrequency, string &noteName,string key, string keyType, string minorType, string mode)
{
	double AFrequency = 441.430664;//440.0;
	int ToneIndexOffsetToPositives = 120;

	int toneIndex = (int)round(log(frequency / AFrequency)/log(ToneStep));
	noteName = NoteNames[(ToneIndexOffsetToPositives + toneIndex) % NoteMax];
	closestFrequency = pow(ToneStep, toneIndex) * AFrequency;
	
	//see if found note is in selected key
	scaleType Scale[ScaleNotes];//scale for key/mode/etc...
	buildScale(key, keyType, minorType,mode, Scale,ScaleNotes);//return scale (includes notes and intervals)

	bool noteFound = false;
	
	for(int i=0;i<(int)ScaleNotes;i++)
	{
		if(noteName == Scale[i].scaleNotes)
		{
			noteFound=true;
			break;
		}
	}
	
		//get rootNoteIndex
	for(int n=0;n<NoteMax;n++)
	{
			if(noteName == NoteNames[n])
			{
				rootNoteIndex = n;
			}
	}
	
	
	
	/////////////////search up and down by halfsteps///////////////////////////////
	/*
	if(noteFound ==false)
	{
		//printf("note not found in key -----: %s\n",noteName.c_str());
	
		string nNote = noteName.substr(0,1);//get note with out sharp/flat
		
		for(int t=0;t<(int)ScaleNotes;t++)
		{
			//printf("t up-----: %d\n",t);
			
			if(nNote == Scale[t].scaleNotes)
			{
				//get next scale index
				
				if(t==ScaleNotes-1)
				{
					t=0;
					noteName = Scale[t+1].scaleNotes;
				}
				else
				{
					
					noteName = Scale[t+1].scaleNotes;
				}
				
				for(int j=0;j<(int)ScaleNotes;j++)
				{
					if(noteName == Scale[j].scaleNotes)
					{
						noteFound=true;
						break;
					}
				}
				
				printf("substituted note next-----: %s\n",noteName.c_str());
				break;
			}
		}
		
		if(!noteFound)
		{
			for(int t=0;t<(int)ScaleNotes;t++)
			{
				//printf("t down-----: %d\n",t);
				
				if(nNote == Scale[t].scaleNotes)
				{
					//get previous scale index
					if(t==0)
					{
						t=ScaleNotes;
						noteName = Scale[t-1].scaleNotes;
					}
					else
					{
						noteName = Scale[t-1].scaleNotes;
					}
					
					
					
					for(int j=0;j<(int)ScaleNotes;j++)
					{
						if(noteName == Scale[j].scaleNotes)
						{
							noteFound=true;
							break;
						}
					}
				
					printf("substituted note previous -----: %s\n",noteName.c_str());
					break;
				}
			}
		
		}
		
	}
	*/
	//////////////////////////////////////////////////
	
	return noteFound;
}

int semitoneGet::findIntervalNote(int selInterval, string key, string keyType, string minorType, string mode,string rootNote)
{

	//build scale for selected key
	int semitoneValue = 0;
	int rootIndex = 0;
	string intervalNote;
	
	scaleType Scale[ScaleNotes];//scale for key/mode/etc...
	buildScale(key, keyType, minorType,mode, Scale,ScaleNotes);//return scale (includes notes and intervals)
	
	//find root note in the returned scale
	for(int i = 0;i<ScaleNotes;i++)
	{
		if(Scale[i].scaleNotes == rootNote)
		{
			rootIndex = i;
			break;
		}
	}

	//printf("Key -----: %s\n",key.c_str());
	
	//get interval note in scale starting at root note(which is 1)
	//need to check beginning of array
	int limit = rootIndex + selInterval;

	if(limit < ScaleNotes)
	{
		intervalNote = Scale[limit].scaleNotes;
	}
	else
	{
		limit = limit - ScaleNotes;
		intervalNote = Scale[(limit)].scaleNotes;
	}
	
	//scan the semitones array starting at the root and ending at interval note
	rootIndex = 0;
	int z;
	
	for(int i=0;i<NoteMax;i++)
	{
		if(rootNote==NoteNames[i])
		{
			rootIndex = i;
			for(z=0;z<NoteMax;z++)
			{
				if(intervalNote==NoteNames[z])
				{
					semitoneValue = z-rootIndex;//need to check when z < rootIndex
					break;
				}
			}
			
			if(z < NoteMax)//wraps back to beginning of semitones array
			{
				int stopIndex = NoteMax - z;
				for(int z=0;i<stopIndex;z++)
				{
					if(intervalNote==NoteNames[z])
					{
						semitoneValue = z-rootIndex;//need to check when z < rootIndex
						break;
					}
				}
			}
			
			if(semitoneValue < 0)
			{
					semitoneValue = semitoneValue + 12;
			}
		
			break;
		}
		
	}
	
	if(inputNote != rootNote)
	{
		inputNote = rootNote;
		//printf("*******root note -----: %s\n",inputNote.c_str());
		//printf("----Interval note: %s\n",intervalNote.c_str());
		//printf("+++++++semitoneValue: %d\n",semitoneValue);
	}


	return semitoneValue;

}

double semitoneGet::round(double number)
{
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}



int semitoneGet::getSemitone(float srcFrequency,int selInterval, string key, string keyType, string minorType, string mode)
{
	int semitoneValue=0;
	double closestFreq;
	string note;
	string rootNote;
	bool noteFound;
	
	if(srcFrequency > 0)
	{
		//gets closest note given the key. if note not found in key, this returns false
		noteFound = FindClosestNote(srcFrequency, closestFreq, note, key, keyType, minorType, mode);//returns closest note
	
		rootNote = note;
			//printf("*******rootNote note -----: %s\n",rootNote.c_str());
		semitoneValue=findIntervalNote(selInterval, key, keyType, minorType, mode, rootNote);
		
			//use last if not found
/*
		if(noteFound == true)
		{
			rootNote = note;
				//printf("*******rootNote note -----: %s\n",rootNote.c_str());
			
			//gets semitone value of next voice
			semitoneValue=findIntervalNote(selInterval, key, keyType, minorType, mode, rootNote);	
		}
		else//use last semitone value
		{
			semitoneValue = -1;
			//printf("note NOT found -----: %s\n",note.c_str());
		}
*/
		
	}
	
	
	
	//return semitones from input note to interval note
	return semitoneValue;
	
}