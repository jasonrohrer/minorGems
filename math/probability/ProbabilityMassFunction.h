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
 */



#ifndef PROBABILITY_MASS_FUNCTION_INCLUDED
#define PROBABILITY_MASS_FUNCTION_INCLUDED


#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/random/RandomSource.h"

#include "minorGems/io/Serializable.h"



/**
 * A discrete probability distribution.
 * All probabilities are constrained to sum to 1.
 *
 * @author Jason Rohrer
 */
class ProbabilityMassFunction : public Serializable {

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
		 *   Will be destroyed by this constructor.
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
		virtual unsigned long addElement( double inProbability );



		/**
		 * Removes an element from this mass function.
		 *
		 * @param inIndex the index of the element.
		 */
		virtual void removeElement( unsigned long inIndex );

		

		/**
		 * Gets the probability of an element.
		 *
		 * @param inIndex the index of the element.
		 *
		 * @return the probability of the element at inIndex,
		 *   or 0 if inIndex is not a valid index.
		 */
		virtual double getProbability( unsigned long inIndex );


		
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
		virtual void setProbability( unsigned long inIndex,
									 double inProbability );



		/**
		 * Samples an element at random according to this distribution.
		 *
		 * @return the index of the sampled element.
		 */
		virtual unsigned long sampleElement();
		


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

		

		// implement the Serializable interface
		virtual int serialize( OutputStream *inOutputStream );
		virtual int deserialize( InputStream *inInputStream );


		
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



inline unsigned long ProbabilityMassFunction::addElement(
	double inProbability ) {

	mProbabilityVector->push_back( inProbability );

	normalize();
	
	return mProbabilityVector->size();
	}



inline void ProbabilityMassFunction::removeElement(
	unsigned long inIndex ) {

	mProbabilityVector->deleteElement( (int)inIndex );

	normalize();	
	}



inline double ProbabilityMassFunction::getProbability(
	unsigned long inIndex ) {

	if( inIndex >= 0 && inIndex < mProbabilityVector->size() ) {
		return *( mProbabilityVector->getElement( inIndex ) );
		}
	else {
		return 0;
		}
	}



inline void ProbabilityMassFunction::setProbability( unsigned long inIndex,
													 double inProbability ) {

	if( inIndex >= 0 && inIndex < mProbabilityVector->size() ) {
		*( mProbabilityVector->getElement( inIndex ) ) = inProbability;
		normalize();
		}
	}



inline unsigned long ProbabilityMassFunction::sampleElement() {

	double randVal = mRandSource->getRandomDouble();
	
	// compute a running CDF value until we surpass randVal
	double currentCDFVal = 0;
	int currentIndex = -1;
	
	while( currentCDFVal < randVal ) {
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

		double newSum = getProbabilitySum();
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
	long numElements = mProbabilityVector->size();

	for( int i=0; i<numElements; i++ ) {
		double prob = *( mProbabilityVector->getElement( i ) );

		printf( "%lf ", prob );
		}
	}



inline int ProbabilityMassFunction::serialize( OutputStream *inOutputStream ) {

	int numBytes = 0;

	long numElements = mProbabilityVector->size();

	numBytes += inOutputStream->writeLong( numElements );
	
	for( int i=0; i<numElements; i++ ) {
		numBytes += inOutputStream->writeDouble(
			*( mProbabilityVector->getElement( i ) ) );
		}
	
	return numBytes;
	}
	
	
	
inline int ProbabilityMassFunction::deserialize( InputStream *inInputStream ) {
	int numBytes = 0;

	delete mProbabilityVector;

	long numElements;

	numBytes += inInputStream->readLong( &numElements );

	mProbabilityVector = new SimpleVector< double >( numElements );

	for( int i=0; i<numElements; i++ ) {
		double prob;

		numBytes += inInputStream->readDouble( &prob );

		mProbabilityVector->push_back( prob );
		}

	normalize();
	
	return numBytes;
	}



#endif
