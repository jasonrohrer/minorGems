/*
 * Modification History
 *
 * 2001-November-17   Jason Rohrer
 * Created.
 * Added a constructor for explicit initialization of elements.
 * Made serializable.
 *
 * 2001-November-18   Jason Rohrer
 * Added function for finding the minimum probability value.
 * Added a print function.
 *
 * 2011-February-1   Jason Rohrer
 * Got rid of legacy Serializable code.  Fixed warnings.
 *
 * 2011-February-3   Jason Rohrer
 * Added check to deal with rounding errors or 0-length probability 
 * distributions.
 */



#ifndef PROBABILITY_MASS_FUNCTION_INCLUDED
#define PROBABILITY_MASS_FUNCTION_INCLUDED


#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/random/RandomSource.h"




/**
 * A discrete probability distribution.
 * All probabilities are constrained to sum to 1.
 *
 * @author Jason Rohrer
 */
class ProbabilityMassFunction {

	public:


		
		/**
		 * Constructs a probability mass function.
		 *
		 * @param inRandSource the random source to use when
		 *   sampling from this function.
		 *   Must be destroyed by caller after this class is destroyed.
		 */
		ProbabilityMassFunction( RandomSource *inRandSource );
		

		
		/**
		 * Constructs a probability mass function with elements
		 * already inserted.
		 *
		 * @param inRandSource the random source to use when
		 *   sampling from this function.
		 *   Must be destroyed by caller after this class is destroyed.
		 * @param inNumElements the number of elements
		 *   this PMF should be initialized with.
		 * @param inProbabilities the probabilities to start with.
		 *   Note that these values will be normalized before
		 *   being stored.
		 *   Destroyed by caller
		 */
		ProbabilityMassFunction( RandomSource *inRandSource,
								 int inNumElements, double *inProbabilities );

		
		
		virtual ~ProbabilityMassFunction();



		/**
		 * Adds an element and a probability to
		 * this mass function.
		 *
		 * Note that since the distribution
		 * always sums to 1, inProbability will
		 * be normalized and thus the value set will likely
		 * not exactly equal the value specified by the caller.
		 *
		 * @param inProbability the probability of the new element.
		 *
		 * @return the index of the new element in the distribution/
		 *   Note that the actual index of this element
		 *   may change in the future as elements are added to
		 *   and removed from this mass function.
		 */
		virtual int addElement( double inProbability );



		/**
		 * Removes an element from this mass function.
		 *
		 * @param inIndex the index of the element.
		 */
		virtual void removeElement( int inIndex );

		

		/**
		 * Gets the probability of an element.
		 *
		 * @param inIndex the index of the element.
		 *
		 * @return the probability of the element at inIndex,
		 *   or 0 if inIndex is not a valid index.
		 */
		virtual double getProbability( int inIndex );


		
		/**
		 * Sets the probability of an element.
		 *
		 * Note that since the distribution
		 * always sums to 1, inProbability will
		 * be normalized and thus the value set will likely
		 * not exactly equal the value specified by the caller.
		 *
		 * @param inIndex the index of the element.
		 * @param inProbability the probability of the element at inIndex.
		 */
		virtual void setProbability( int inIndex,
									 double inProbability );



		/**
		 * Samples an element at random according to this distribution.
		 *
		 * @return the index of the sampled element.
		 */
		virtual int sampleElement();
		


		/**
		 * Gets the minimum probability value of this function.
		 *
		 * @return the minimum probability value.
		 */
		virtual double getMinimumProbability();



		/**
		 * Prints the probabilities in this mass function to standard out.
		 */
		virtual void print();

		



		
	protected:
		RandomSource *mRandSource;
		
		SimpleVector< double > *mProbabilityVector;



		/**
		 * Normalizes this distribution so that it sums to 1.
		 */
		virtual void normalize();

		

		/**
		 * Gets the net sum of this probability distribution.
		 *
		 * @return the sum of this distribution.
		 */
		virtual double getProbabilitySum();


		
	};



inline ProbabilityMassFunction::ProbabilityMassFunction(
	RandomSource *inRandSource )
	: mRandSource( inRandSource ),
	  mProbabilityVector( new SimpleVector< double >() ) {
	}



inline ProbabilityMassFunction::ProbabilityMassFunction(
	RandomSource *inRandSource,
	int inNumElements, double *inProbabilities )
	: mRandSource( inRandSource ),
	  mProbabilityVector( new SimpleVector< double >( inNumElements ) ){

	for( int i=0; i<inNumElements; i++ ) {
		mProbabilityVector->push_back( inProbabilities[i] );
		}

	normalize();
	}



inline ProbabilityMassFunction::~ProbabilityMassFunction() {

	// we can simply delete this vector, since its elements are primitives
	delete mProbabilityVector;	
	}



inline int ProbabilityMassFunction::addElement(
	double inProbability ) {

	mProbabilityVector->push_back( inProbability );

	normalize();
	
	return mProbabilityVector->size();
	}



inline void ProbabilityMassFunction::removeElement(
	int inIndex ) {

	mProbabilityVector->deleteElement( (int)inIndex );

	normalize();	
	}



inline double ProbabilityMassFunction::getProbability(
	int inIndex ) {

	if( inIndex >= 0 && inIndex < mProbabilityVector->size() ) {
		return *( mProbabilityVector->getElement( inIndex ) );
		}
	else {
		return 0;
		}
	}



inline void ProbabilityMassFunction::setProbability( int inIndex,
													 double inProbability ) {

	if( inIndex >= 0 && inIndex < mProbabilityVector->size() ) {
		*( mProbabilityVector->getElement( inIndex ) ) = inProbability;
		normalize();
		}
	}



inline int ProbabilityMassFunction::sampleElement() {

	double randVal = mRandSource->getRandomDouble();
	
	// compute a running CDF value until we surpass randVal
	double currentCDFVal = 0;
	int currentIndex = -1;
	
    int numValues = mProbabilityVector->size();
    

	while( currentCDFVal < randVal && 
           currentIndex < numValues - 1 ) {
		
        currentIndex++;

		double currentProb =
			*( mProbabilityVector->getElement( currentIndex ) );

		currentCDFVal += currentProb;
		}
	// when we exit the while loop, we are at the index of the
	// element whose probability pushed the CDF past randVal

	
	if( currentIndex == -1 ) {
		// randVal was 0
		return 0;
		}
	else {
		return currentIndex;
		}
	}



inline double ProbabilityMassFunction::getMinimumProbability() {
	// works, since all values <= 1
	double minValue = 2;

	int numElements = mProbabilityVector->size();
	
	for( int i=0; i<numElements; i++ ) {

		double prob = *( mProbabilityVector->getElement( i ) );

		if( prob < minValue ) {
			minValue = prob;
			}
		}
	
	return minValue;
	}



inline void ProbabilityMassFunction::normalize() {

	double currentSum = getProbabilitySum();

	if( currentSum != 1 ) {
		double invCurrentSum = 1.0 / currentSum;

		int numElements = mProbabilityVector->size();

		for( int i=0; i<numElements; i++ ) {

			double *prob = mProbabilityVector->getElement( i );

			*prob = (*prob) * invCurrentSum;
			}

		}
	}



inline double ProbabilityMassFunction::getProbabilitySum() {
	double sum = 0;

	int numElements = mProbabilityVector->size();

	for( int i=0; i<numElements; i++ ) {
		
		sum += *( mProbabilityVector->getElement( i ) );
		}
	
	return sum;
	}



inline void ProbabilityMassFunction::print() {
	int numElements = mProbabilityVector->size();

	for( int i=0; i<numElements; i++ ) {
		double prob = *( mProbabilityVector->getElement( i ) );

		printf( "%lf ", prob );
		}
	}



#endif
