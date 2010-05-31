/*
 * Modification History 
 *
 * 2001-August-30  	Jason Rohrer
 * Created.
 *
 * 2001-August-31  	Jason Rohrer
 * Finished implementation.
 * Fixed a compile bug.
 *
 * 2001-September-4   Jason Rohrer
 * Added support for FixedConstantExpressions.   
 */
 
 
#ifndef RANDOM_EXPRESSION_FACTORY_INCLUDED
#define RANDOM_EXPRESSION_FACTORY_INCLUDED 
 
#include "Expression.h"

#include "ConstantExpression.h"
#include "FixedConstantExpression.h"
#include "InvertExpression.h"
#include "ProductExpression.h"
#include "PowerExpression.h"
#include "SumExpression.h"
#include "NegateExpression.h"
#include "SinExpression.h"


#include "minorGems/util/random/RandomSource.h"


/**
 * Utility class for constructing random expressions.
 *
 * @author Jason Rohrer 
 */
class RandomExpressionFactory { 
	
	public:


		
		/**
		 * Constructs a random expression factory.
		 *
		 * @param inRandSource the source for random numbers
		 *   to use while constructing expressions.
		 *   Must be destroyed by caller after this class is destroyed.
		 */
		RandomExpressionFactory( RandomSource *inRandSource );


		
		/**
		 * Recursively constructs a random expression with all parameters
		 * filled.
		 *
		 * @param inProbOfStopping the probability of stopping at each
		 *   branch of the expression.  Upon stopping on a certain branch
		 *   of the recursion, it simply returns a constant expression.
		 * @param inProbOfFixedConstant the probability of a fixed
		 *   constant expression being inserted upon stopping (as opposed
		 *   to a (mutable) constant expression).
		 * @param inMaxDepth if this is reached, the recursion
		 *   stops regardless of inProbOfStopping.
		 * @param inConstantMax the maximum value for a constant expression.
		 *
		 * @return the constructed expression.
		 */
		Expression *constructRandomExpression(
			double inProbOfStopping, double inProbOfFixedConstant,
			int inMaxDepth, double inConstantMax );


		
	protected:

		RandomSource *mRandSource;
				
	};



inline RandomExpressionFactory::RandomExpressionFactory(
	RandomSource *inRandSource )
	: mRandSource( inRandSource ) {

	}



inline Expression *RandomExpressionFactory::constructRandomExpression( 
	double inProbOfStopping,
	double inProbOfFixedConstant,
	int inMaxDepth, double inConstantMax ) {
	
	// fill in constant expressions only at the leaves
	if( inMaxDepth == 0 || 
		mRandSource->getRandomDouble() <= inProbOfStopping ) {
		// stop
		if( mRandSource->getRandomDouble() <= inProbOfFixedConstant ) {
			return new FixedConstantExpression( 
				inConstantMax * ( mRandSource->getRandomDouble() ) );
			}
		else {
			return new ConstantExpression( 
				inConstantMax * ( mRandSource->getRandomDouble() ) );
			}
		}
	else {
		// keep filling in non constant expressions randomly
		
		// FILL IN HERE
		
		// we have 6 expression types
		int randVal = mRandSource->getRandomBoundedInt( 0, 3 );
		
		Expression *outExpression;

		// pick an expression type
		
		switch( randVal ) {
			case 0:
				outExpression = new NegateExpression( NULL );
				break;
			case 1:
				outExpression = new ProductExpression( NULL, NULL );
				break;
			case 2:
				outExpression = new SinExpression( NULL );
				break;
			case 3:
				outExpression = new SumExpression( NULL, NULL );
				break;				
			default:
				// should never happen, but...
				printf( "RandomExpressionFactory: " );
				printf( "Error while generating random expression\n" );
				
				// default to a constant expression of 0
				outExpression = new ConstantExpression( 0 );
				break;
			}

		// now recursively fill in the arguments in succession
		for( int i=0; i<outExpression->getNumArguments(); i++ ) {
			
			// create a random expression as the argument
			// note that we decrement inMaxDepth here
			Expression *argument = constructRandomExpression( 
				inProbOfStopping, inProbOfFixedConstant,
				inMaxDepth - 1, inConstantMax );

			// set the argument into our expression	
			outExpression->setArgument( i, argument );
			}	
		
		// now expression is complete.
		return outExpression;
		
		}	// end of non-constant else case
	
	}	// end of constructRandomExpression()



#endif
