// Jason Rohrer
// Noise.h

/**
*
*	Noise generation interface
*
*
*	Created 11-3-99
*	Mods:
*		Jason Rohrer	12-20-2000	Added a fractal noise function
*									that fills a double array.
*
*/

#include "minorGems/common.h"



#ifndef NOISE_INCLUDED
#define NOISE_INCLUDED


#include <stdlib.h>
#include <math.h>

#include "RandomSource.h"




// fills 2d image with RGBA noise
void genRandNoise2d(unsigned long *buff, int buffHigh, int buffWide);



// returns a random floating point between 0 and 1
inline float floatRand() {
	float invRandMax = 1 / ((float)RAND_MAX);
	return (float)(rand()) * invRandMax;
	}


// fills 2d image with ARGB fractal noise
void genFractalNoise2d(unsigned long *buff, int buffHigh, int buffWide);


/**
 * Fills a 2d array with 1/f fractal noise.
 *
 * @param inBuffer a pre-allocated buffer to fill.
 * @param inWidth the width and height of the 2d array 
 *   contained in the buffer.
 *   Must be a power of 2.
 * @param inMaxFrequency the maximum frequency of noise modulation to
 *   include, in [2,inWidth].  Lower values produce more "blurry" or
 *   blocky noise.
 * @param inFPower power to raise F to whene generating noise.
 *   Amplitude of modulation = 1 / (F^inFPower).
 * @param inInterpolate set to true to perform interpolation of
 *   each frequency modulation.  Setting to false produces a "blockier"
 *   noise, while setting to true makes the noise more cloud-like.
 * @param inRandSource the source to use for random numbers.
 */
void genFractalNoise2d( double *inBuffer, int inWidth, int inMaxFrequency,
	double inFPower, char inInterpolate, RandomSource *inRandSource );


/**
 * Fills a 1d array with 1/f fractal noise.
 *
 * @param inBuffer a pre-allocated buffer to fill.
 * @param inWidth the width of the 2d array 
 *   contained in the buffer.
 *   Must be a power of 2.
 * @param inMaxFrequency the maximum frequency of noise modulation to
 *   include, in [2,inWidth].  Lower values produce more "blurry" or
 *   blocky noise.
 * @param inFPower power to raise F to whene generating noise.
 *   Amplitude of modulation = 1 / (F^inFPower). 
 * @param inInterpolate set to true to perform interpolation of
 *   each frequency modulation.  Setting to false produces a "blockier"
 *   noise, while setting to true makes the noise more cloud-like.
 * @param inRandSource the source to use for random numbers.
 */
void genFractalNoise( double *inBuffer, int inWidth, int inMaxFrequency,
	double inFPower, char inInterpolate, RandomSource *inRandSource );

#endif
