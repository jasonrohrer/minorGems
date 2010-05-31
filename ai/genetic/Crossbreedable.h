/*
 * Modification History
 *
 * 2000-September-9		Jason Rohrer
 * Created.
 */

#ifndef CROSSBREEDABLE_INCLUDED
#define CROSSBREEDABLE_INCLUDED

#include <stdio.h>

/**
 * Abstract interface for any objects that can be crossbred. 
 *
 * @author Jason Rohrer 
 */
class Crossbreedable {

	public:
		
		/**
		 * Constructs a crossbreedable with generation set to 0;
		 */
		Crossbreedable();
		
		/**
		 * Constructs a crossbreedable by reading generation from a text file.
		 *
		 * @param inInputFile the text file to read the attributes from.
		 */
		Crossbreedable( FILE *inInputFile );
		
		/**
		 * Crossbreeds this Crossbreedable with another to produce
		 * a new one.
		 *
		 * @param inOther the other Crossbreedable to cross this one with.
		 * @param inFractionOther the fraction of the other's characteristics, 
		 *   in [0..1] that will end up in the offspring.
		 * @param inMutationProb the probability of mutations throughout the
		 *   breeding process (passing in 1 will create a mutation at 
		 *   each attribute of the offspring).
		 * @param inMaxMutationMagnitude the maximum "severity" of any
		 *   individual mutation, in [0..1].
		 * @return a pointer to the offspring Crossbreedable, or NULL
		 *   if the crossbreeding process fails.
		 */
		virtual void *crossbreed( Crossbreedable *inOther, 
			float inFractionOther, float inMutationProb,
			float inMaxMutationMagnitude ) = 0;
			
		/**
		 * Mutates this Crossbreedable.
		 *
		 * @param inMutationProb the probability of mutations throughout
		 *   this crossbreedable (passing in 1 will create a mutation at 
		 *   each attribute of the crossbreedable).
		 * @param inMaxMutationMagnitude the maximum "severity" of any
		 *   individual mutation, in [0..1].
		 */
		virtual void mutate( float inMutationProb, 
			float inMaxMutationMagnitude ) = 0;
			
		/**
		 * Gets the generation number for this Crossbreedable.
		 *
		 * @return the generation number.
		 */
		int getGeneration();
		
		/**
		 * Sets the generation number for this Crossbreedable given
		 * the generation numbers of its parents.
		 *
		 * @param inParentA first parent
		 * @param inParentB second parent.
		 */
		void setGeneration( Crossbreedable *inParentA, 
			Crossbreedable *inParentB );
		
		/**
		 * Writes this Crossbreedable out to a text file.
		 *
		 * @param inOutputFile the text file to write this network out to.
		 *
		 * @return true iff file was written to successfully.
		 */
		virtual char writeToFile( FILE *inOutputFile ); 
			
	protected:
		int mGeneration;
	}; 



inline Crossbreedable::Crossbreedable() 
	: mGeneration(0) {
	}

inline Crossbreedable::Crossbreedable( FILE *inInputFile ) {
	fscanf( inInputFile, "%d", &mGeneration );
	}

inline int Crossbreedable::getGeneration() {
	return mGeneration;
	}
	
inline void Crossbreedable::setGeneration( Crossbreedable *inParentA, 
	Crossbreedable *inParentB ) {

	int genA = inParentA->getGeneration();
	int genB = inParentB->getGeneration();

	mGeneration = genA;
	if( genB > genA ) {
		mGeneration = genB;
		}
	mGeneration++;
	}

inline char Crossbreedable::writeToFile( FILE *inOutputFile ) {
	fprintf( inOutputFile, "%d", mGeneration );
	fprintf( inOutputFile, "\n" );	// for human readability
	return true;
	}

#endif
