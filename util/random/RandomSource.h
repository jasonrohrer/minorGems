// Jason Rohrer
// RandomSource.h

/**
*
*	abstract interface for random number generation
*
*		Can be implemented by:
*			--stdlib rand() function calls
*			--seed file full of random numbers
*
*	Created 12-7-99
*	Mods:
*   	Jason Rohrer	9-28-2000	Added a getRandomBoundedInt()
*									interface to faciliate retrieving
*									an integer in a given range [a,b]
*									where each integer in the range
*									has the same probability of being
*									returned.
*   	Jason Rohrer	12-16-2000	Added a getRandomDouble() interface.
*   	Jason Rohrer	11-21-2005	Added a virtual destructor.
*   	Jason Rohrer	07-09-2006	Added a getRandomBoundedDouble interface.
*   	Jason Rohrer	07-27-2006	Added a getRandomBoolean interface.
*/

#include "minorGems/common.h"



#ifndef RANDOM_SOURCE_INCLUDED
#define RANDOM_SOURCE_INCLUDED


class RandomSource {

	public:		
		// pure virtual functions implemented by inheriting classes		
		
		virtual float getRandomFloat() = 0;	// in interval [0,1.0]
		virtual double getRandomDouble() = 0; // in interval [0,1.0]
		virtual unsigned int getRandomInt() = 0;		// in interval [0,MAX]
		virtual unsigned int getIntMax() = 0;	// returns MAX
		
		/**
		 * Returns a random integer in [rangeStart,rangeEnd]
		 * where each integer in the range has an equal
		 * probability of occuring.
		 */
		virtual int getRandomBoundedInt( int inRangeStart,
			int inRangeEnd ) = 0;


        /**
		 * Returns a random double in [rangeStart,rangeEnd].
		 */
		virtual double getRandomBoundedDouble( double inRangeStart,
			double inRangeEnd ) = 0;


        
        /**
         * Gets a random true/false value.
         */
        virtual char getRandomBoolean() = 0;

        
        
        virtual ~RandomSource();

        
	protected:
		unsigned int MAX;		// maximum integer random number
		float invMAX;	// floating point inverse of MAX
		double invDMAX;	// double invers of MAX			
	};



inline RandomSource::~RandomSource() {
    // does nothing
    // exists to ensure that subclass destructors are called
    }



#endif
