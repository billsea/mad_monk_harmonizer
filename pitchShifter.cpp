/*
 *  pitchShifter.cpp
 *  MadMonk
 *
 *  Created by bill on 11/30/10.
 *  Copyright 2010 _LoudSoftware_. All rights reserved.
 *
 */


#include "pitchShifter.h"
#include "AUEffectBase.h"
#include "math.h"

#define PI 3.14159265
//#define sampleBufferSize 512//this is 7500 in java app

// Fixed delay period with which to do pitch shifting
int FIXEDDELAYINMS = 100;//100;

// Fade in/out times
int CROSSFADETIMEINMS = 12;//12;

// Constant by which one tone differs from the next when the
// interval is a halftone.
double twelvethRootOfTwo = pow(2, 1.0 / 12.0);

pitchShifter::pitchShifter()
{

}

void pitchShifter::pitchShifterInit(int nSampleRate,int bufFrames)
	{

		
		printf("Sample Rate: %d\n",sampleRate);
		
		sweepUp = true;
		numberOfChannels = 1;
		sweep = 0.0;		// Initial value for sweep rate
		channelA = true;	// Setup to use channel A sweep
		blendA = 1.0;		// Blend values for the two delay channels
		blendB = 0.0;
		
        // Allocate local sample buffer
		//localBufferL = new float[sampleBufferSize];
		//localBufferR = new float[sampleBufferSize];
		
		// See if we have the necessary data to initialize delay
		if ((sampleRate != 0) && (numberOfChannels != 0) &&
            (!initializationComplete)) {
			
			// Allocate delay buffer for the fixed delay time
			numberOfDelaySamples = 
			(FIXEDDELAYINMS * sampleRate * numberOfChannels) / 1000;
			printf("numberOfDelaySamples: %d\n",numberOfDelaySamples);
			
			// Total buffer size
			delayBufferSize = sampleBufferSize + numberOfDelaySamples; //AudioConstants.sampleBufferSize + numberOfDelaySamples;
			printf("delayBufferSize: %d\n",delayBufferSize);

			// Allocate new delay buffer
			delayBufferL = new float[delayBufferSize];//remember to clean up...(delete [] delayBuffer and delayBuffer=NULL);
			delayBufferR = new float[delayBufferSize];
			
			printf("delay buffer...\n");
			
            // Initialize indices in the delay buffer
		    setIndices();
			
			// Calculate the number of cross fade samples
			numberOfCrossFadeSamples = (CROSSFADETIMEINMS * sampleRate) / 1000;
			printf("numberOfCrossFadeSamples: %d\n",numberOfCrossFadeSamples);
			
			// Allocate arrays for cross fade coefficients
			fadeIn  = new double[numberOfCrossFadeSamples];
			fadeOut = new double[numberOfCrossFadeSamples];
			
			// Fill in the arrays with fade in/out values. Sin and Cos
			// values are used for smooth results.
			for (int i=0; i < numberOfCrossFadeSamples; i++) {
				double angle = (i * PI) / (2.0 * numberOfCrossFadeSamples);
				fadeIn[i]  = cos(angle);
				fadeOut[i] = sin(angle);
				//printf("fadeIn[i]: %f\n",fadeIn[i]);
				//printf("fadeOut[i]: %f\n",fadeOut[i]);
			}
            // Indicate initialization is complete
			initializationComplete = true;
			
			printf("delay init completed...\n");
		}

	}
void pitchShifter:: setSampleBufferSize(int bufFrames)
	{
		sampleBufferSize = bufFrames;
	}

	void  pitchShifter::setSampleRate( int nSampleRate )
	{
		sampleRate = nSampleRate;
	}

	// Called when the user changes the dry level. 
	void pitchShifter:: setDryLevel(int dryLevelIn) {
		
		// Value in the range 0..100
		dryLevel = dryLevelIn;
		//printf("dryLevel: %d\n",dryLevel);
	}

	// Called when the user changes the wet level. 
	void pitchShifter:: setWetLevel(int wetLevelIn) {
		
		// Value in the range 0..100
		wetLevel = wetLevelIn;
		//printf("wetLevel: %d\n",wetLevel);
	}

	// Called when the user changes the feedback level.
	void pitchShifter:: setFeedbackLevel(int feedbackLevel) {
		
		// Value in the range 0..100
		feedbackLevel = feedbackLevel;
		//printf("feedbackLevel: %d\n",feedbackLevel);
	}

	// Set read/write indices depending upon audio format and
	// frequency change direction
	void pitchShifter::setIndices() {
		
		// Index where dry sample is written
		writeIndex = 0;
		readIndexBLow = 0;
		readIndexBHigh = 0;
		
		//printf("set indices...\n");
		
		if (sweepUp) {
			// Sweeping upward, start at max delay
			readIndexALow = sampleBufferSize;// AudioConstants.sampleBufferSize;
			
		}	else	{
			
			// Sweeping downward, start at min delay
			//non interleaved channels, so number of channels is always one
			readIndexALow = delayBufferSize - 2;
			/*
			if (numberOfChannels == 1)
				readIndexALow = delayBufferSize - 2;
			else
				readIndexALow = delayBufferSize - 4;
			 */
		}
		
		//printf("numberOfChannels: %d\n",numberOfChannels);
		
		// Initialize other read ptr
		//non interleaved channels, so number of channels is always one
		readIndexAHigh = readIndexALow + 1;
		/*
		if (numberOfChannels == 1)
			readIndexAHigh = readIndexALow + 1;
		else
			readIndexAHigh = readIndexALow + 2;
		 */
	}


	// Called when the user changes the pitch shift value
	void pitchShifter:: setPitchShift(int pitchShift) {
		
		// Values are in half steps (semitones) in the 
		// range -12..0..+12 corresponding to -/+ 1 octave for
		// a range of 2 octaves.
		
		// Determine which direction the sweep is going
		sweepUp = (pitchShift >= 0);
			
		setIndices();
		
		double newStep = 1.0;
		
		// If pitch shift is 0 short circuit calculations
		if (pitchShift == 0)
			step = 0;
		
		else	{
			// Step is rate at which samples read out
			for (int i=0; i < abs(pitchShift); i++) {
				if (pitchShift > 0)
					newStep *= twelvethRootOfTwo;
				else
					newStep /= twelvethRootOfTwo;
			}
			step = fabs(newStep - 1.0);
		}
		
		// Reset the following values whenever pitch shift value changes
		sweep = 0.0;
		crossFadeCount = 0;
		activeSampleCount = numberOfDelaySamples - (int)(numberOfCrossFadeSamples * (newStep - 1.0) - 2); 
		
		//printf("step: %f\n",step);
		//printf("pitchShift: %d\n",pitchShift);
	}

	void pitchShifter::processInput(float * srcInL, float * srcInR,long numFrames)
		{
			//printf(" inFramesProcess=%d",numFrames);
			
			double delaySampleA, delaySampleB;
			
			for(int i=0;i<numFrames;i++)
			{
				//gets sample value at each frame index and PROCESS EACH SAMPLE INDIVIDUALLY!
				//float sample = srcInL[i];
				//printf(" sample=%f",sample);
				
				float  sampleL = srcInL[i] ;
				float  sampleR = srcInR[i] ;

				//test - adjust gain only
				//srcInL[i] = sampleL * 0.5;
				//srcInR[i] = sampleR * 0.8;
			
				float dsALow  = delayBufferL[readIndexALow];
				float dsAHigh = delayBufferL[readIndexAHigh];
				float dsBLow  = delayBufferL[readIndexBLow];
				float dsBHigh = delayBufferL[readIndexBHigh];
			
				/*
				printf("readIndexALow: %d\n",readIndexALow);
				printf("readIndexAHigh: %d\n",readIndexAHigh);
				printf("readIndexBLow: %d\n",readIndexBLow);
				printf("readIndexBHigh: %d\n",readIndexBHigh);
				
				printf("sweep: %f\n",sweep);
				printf("dsALow: %f\n",dsALow);
				printf("dsAHigh: %f\n",dsAHigh);
				printf("dsBLow: %f\n",dsBLow);
				printf("dsBHigh: %f\n",dsBHigh);
				*/
				
				// Do the linear interpolation
				if (sweepUp) {
					delaySampleA = (dsAHigh * sweep) + (dsALow * (1.0 - sweep));
					delaySampleB = (dsBHigh * sweep) + (dsBLow * (1.0 - sweep));
					//printf("sweepUP: %f\n",sweep);
				}	else	{
					delaySampleA = (dsAHigh * (1.0 - sweep) + (dsALow * sweep));
					delaySampleB = (dsBHigh * (1.0 - sweep) + (dsBLow * sweep));
					//printf("sweep down coef: %f\n",(1.0 - sweep));
				}
				
				//printf("delaySampleA: %f\n",delaySampleA);
				//printf("delaySampleB: %f\n",delaySampleB);
				

				
				// Combine delay channels A and B with appropriate blending
				double outputSampleL = 
				(delaySampleA * blendA) + (delaySampleB * blendB);
				
				double outputSampleR = 
				(delaySampleA * blendA) + (delaySampleB * blendB);
				
				//printf("outputSampleL: %f\n",outputSampleL);
				
				// Store left and right samples in delay buffers
				delayBufferL[writeIndex] = (float)
				(sampleL + ((outputSampleL * feedbackLevel) / 100));
				
				delayBufferR[writeIndex] = (float)
				(sampleR + ((outputSampleR * feedbackLevel) / 100));
				
				//printf("delayBuffer[writeIndex]: %f\n",delayBuffer[writeIndex]);
				
				// Update write index
				writeIndex = (writeIndex + 1) % delayBufferSize;
				
				//printf("writeIndex: %d\n",writeIndex);
				
				// Prepare sample for output by combining wet and dry
				// values
				outputSampleL = 
				((sampleL  * dryLevel) / 100) +
				((outputSampleL * wetLevel) / 100);
				//printf("drylevel: %d\n",dryLevel);
				
				//printf("-outputSampleL: %f\n",outputSampleL);
				
				outputSampleR = 
				((sampleR  * dryLevel) / 100) +
				((outputSampleR * wetLevel) / 100);
				
				
				// Clamp output to legal range
				if (outputSampleL > 32767)
					outputSampleL = 32767;
				if (outputSampleL < -32768)
					outputSampleL = -32768;
				if (outputSampleR > 32767)
					outputSampleR = 32767;
				if (outputSampleR < -32768)
					outputSampleR = -32768;
				
				//printf("--outputSampleL: %f\n",outputSampleL);
				
				// Store output sample in outgoing buffer
				srcInL[i] = outputSampleL;
				srcInR[i] = outputSampleR;
				
				// Update cross fade blending values each sample interval
				if (crossFadeCount != 0) {
					crossFadeCount--;
					
					// Get new blending values for both channels
					blendA = fadeA[crossFadeCount];
					blendB = fadeB[crossFadeCount];
					
					//printf("blendA: %f\n",blendA);
					//printf("blendB: %f\n",blendB);
				}
				
				// Update sweep value for each pass if processing
				// mono signal and every other pass if processing
				// stereo.
				if ((numberOfChannels == 1) || ((i + 1) % 2 == 0))
					sweep += step;
				
				
				if (sweepUp) {
					
					//printf("sweepUp: %d\n",sweepUp);
					
					// Upward frequency change
					
					// Advance indices to reduce delay
					readIndexALow = readIndexAHigh;
					readIndexAHigh = (readIndexAHigh + 1) % delayBufferSize;
					readIndexBLow = readIndexBHigh;
					readIndexBHigh = (readIndexBHigh + 1) % delayBufferSize;
					
					// Check for overflow
					if (sweep < 1.0) {
						// No overflow, continue with next sample
						//printf("sweepUP: %f\n",sweep);
						continue;
					}
					
					// Octave exceeded bump ptrs again
					sweep = 0.0;
					readIndexALow = readIndexAHigh;
					readIndexAHigh = (readIndexAHigh + 1) % delayBufferSize;
					readIndexBLow = readIndexBHigh;
					readIndexBHigh = (readIndexBHigh + 1) % delayBufferSize;
					
					// See if it is time to switch to other delay channel
					if (activeCount-- == 0) {
						
						// Reset fade in/out count
						crossFadeCount = numberOfCrossFadeSamples;
						activeCount = activeSampleCount;
						
						if (channelA) {
							//printf("channelAUP: %d\n",channelA);
							//printf("crossFadeCountUP: %d\n",crossFadeCount);
							
							channelA = false;
							readIndexBHigh = (writeIndex + sampleBufferSize) % delayBufferSize;
							
							// Swap blend coefficient arrays
							fadeA = fadeOut;
							fadeB = fadeIn;
							//printf("fadeAUP1: %f\n",fadeA[0]);
							//printf("fadeBUP1: %f\n",fadeB[0]);
							
						}	else	{
							//printf("channelAUP: %d\n",channelA);
							//printf("crossFadeCountUP: %d\n",crossFadeCount);
							channelA = true;
							readIndexAHigh = (writeIndex + sampleBufferSize) % delayBufferSize;
							
							// Swap blend coefficient arrays
							fadeA = fadeIn;
							fadeB = fadeOut;
							//printf("fadeAUP2: %f\n",fadeA[0]);
							//printf("fadeBUP2: %f\n",fadeB[0]);
							
						}
					}
				
					
				}	else	{
					
					// Downward frequency change
					
					//printf("sweepDown: %d\n",sweepUp);
					// Check for overflow
					if (sweep < 1.0) {
						// No overflow, advance indices
						readIndexALow = readIndexAHigh;
						readIndexAHigh = (readIndexAHigh + 1) % delayBufferSize;
						readIndexBLow = readIndexBHigh;
						readIndexBHigh = (readIndexBHigh + 1) % delayBufferSize;
						//printf("sweepDown: %f\n",sweep);
						// Continue with processing the next sample
						continue;
					}
					
					// Octave exceeded don't bump indices so the delay
					// is increased
					sweep = 0.0;

					// See if it is time to switch to other delay channel
					if (activeCount-- == 0) {
						
						// Reset fade in/out count
						crossFadeCount = numberOfCrossFadeSamples;
						activeCount = activeSampleCount;
						

						if (channelA) 
						{
							//printf("channelADown: %d\n",channelA);
							//printf("crossFadeCountDown: %d\n",crossFadeCount);
							channelA = false;
							readIndexBHigh = (writeIndex + sampleBufferSize) % delayBufferSize;

							// Swap blend coefficient arrays
							fadeA = fadeOut;
							fadeB = fadeIn;
							//printf("fadeADown1: %f\n",fadeA[0]);
							//printf("fadeBDown1: %f\n",fadeB[0]);
							
						}	
						else	
						{
							//printf("channelADown: %d\n",channelA);
							//printf("crossFadeCountDown: %d\n",crossFadeCount);
							channelA = true;
							readIndexAHigh = (writeIndex + sampleBufferSize) % delayBufferSize;
							
							// Swap blend coefficient arrays
							fadeA = fadeIn;
							fadeB = fadeOut;
							//printf("fadeADown2: %f\n",fadeA[0]);
							//printf("fadeBDown2: %f\n",fadeB[0]);
						}
					 
					 
					}
					
				}
				
				
			}
			
			
		}


	
	







