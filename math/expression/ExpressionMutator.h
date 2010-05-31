/*
 * Modification History 
 *
 * 2001-September-9   Jason Rohrer
 * Created.
 *
 * 2001-September-13   Jason Rohrer
 * Made an error message more verbose.  
 */
 
 
#ifndef EXPRESSION_MUTATOR_INCLUDED
#define EXPRESSION_MUTATOR_INCLUDED 

#include "minorGems/util/random/RandomSource.h"

#include "Expression.h" 

#include "ConstantExpression.h"
#include "InvertExpression.h"
#include "ProductExpression.h"
#include "PowerExpression.h"
#include "SumExpression.h"
#include "NegateExpression.h"
#include "SinExpression.h"
#include "FixedConstantExpression.h"
#include "MultiConstantArgumentExpression.h"


 
/**
 * Utility class for mutating expressions. 
 *
 * Note that the number and position of constant expressions
 * within the expression will not change during mutation.  Thus,
 * the structure of the expression remains constant while the operations
 * being performed at each node are mutated.
 *
 * @author Jason Rohrer 
 */
class ExpressionMutator { 
	
	public:



		/**
		 * Mutates an expression.
		 *
		 * Note that this function's implementation is recursive.
		 *
		 * @param inExpression the expression to mutate.  Will be destroyed
		 *   by this function (or if not destroyed, then passed back through
		 *   the return value).  Only the return value, not inExpression,
		 *   can be accessed safely after this function returns.
		 * @param inMutationProb the probability of mutations at each
		 *   node in inExpression.
		 * @param inMaxMutation the fraction of a fixed constant's value
		 *   by which it can be mutated.  For example, if a fixed constant
		 *   has the value 100, and inMaxMutation is 0.5, then the mutated
		 *   constant will be a random value in the range [50, 150].
		 * @param inRandSource the source of random numbers to use.
		 *   Must be destroyed by caller.
		 *
		 * @return a new expression that is a mutated version of inExpression.
		 *   (Note that the returned expression may be inExpression).
		 */
		static Expression *mutateExpression( Expression *inExpression,
											 double inMutationProb,
											 double inMaxMutation,
											 RandomSource *inRandSource );


		
	};



inline Expression *ExpressionMutator::mutateExpression(
	Expression *inExpression,
	double inMutationProb,
	double inMaxMutation,
	RandomSource *inRandSource ) {
		
	long expressionID = inExpression->getID();
	
	// first, deal with constant case
	if( expressionID == ConstantExpression::staticGetID() ) {
		// mutate the constant's value
		ConstantExpression *c = (ConstantExpression *)inExpression;
		double value = c->getValue();

		double maxMutationAmount = inMaxMutation * value;

		// a value in [-1, 1]
		double mutator = ( inRandSource->getRandomDouble() * 2.0 ) - 1.0;

		value = value + mutator * maxMutationAmount;

		c->setValue( value );
		
		return c;
		}
	else if( expressionID == FixedConstantExpression::staticGetID() ) {
		// mutate the fixed constant's value
		FixedConstantExpression *c = (FixedConstantExpression *)inExpression;
		double value = c->getValue();

		double maxMutationAmount = inMaxMutation * value;

		// a value in [-1, 1]
		double mutator = ( inRandSource->getRandomDouble() * 2.0 ) - 1.0;

		value = value + mutator * maxMutationAmount;

		delete c;

		return new FixedConstantExpression( value );
		}
	// next deal with the multi-argument case
	else if( expressionID == MultiConstantArgumentExpression::staticGetID() ) {
		MultiConstantArgumentExpression *m =
			(MultiConstantArgumentExpression*)inExpression;

		Expression *wrappedExpression =
			m->extractWrappedExpression();


		wrappedExpression = mutateExpression( wrappedExpression,
											  inMutationProb,
											  inMaxMutation,
											  inRandSource );

		m->setWrappedExpression( wrappedExpression );

		return m;
		}
	else {
		// the general expression case
		Expression *newExpression = inExpression;
		
		if( inRandSource->getRandomDouble() <= inMutationProb ) {
			// we need to mutate this node

			long newID;
			if( expressionID == InvertExpression::staticGetID()
				|| expressionID == NegateExpression::staticGetID()
				|| expressionID == SinExpression::staticGetID() ) {
				// a unary expression

				// pick another expression type at random
				newID = expressionID;
				while( newID == expressionID ) {
					int randValue = inRandSource->getRandomBoundedInt( 0, 2 );
					switch( randValue ) {
						case 0:
							newID = InvertExpression::staticGetID();
							break;
						case 1:
							newID = NegateExpression::staticGetID();
							break;
						case 2:
							newID = SinExpression::staticGetID();
							break;
						default:
							// we should never hit this
							// note that this newID will cause a printed
							// error below
							newID = -1;
							break;
						}
					}
				}
			else {
				// a binary expression

				// pick another expression type at random
				newID = expressionID;
				while( newID == expressionID ) {
					int randValue = inRandSource->getRandomBoundedInt( 0, 2 );
					switch( randValue ) {
						case 0:
							newID = PowerExpression::staticGetID();
							break;
						case 1:
							newID = ProductExpression::staticGetID();
							break;
						case 2:
							newID = SumExpression::staticGetID();
							break;
						default:
							// we should never hit this
							// note that this newID will cause a printed
							// error below
							newID = -1;
							break;
						}
					}
				}
			
			// now build an expression based on newID
			if( newID == InvertExpression::staticGetID() ) {
				newExpression = new InvertExpression( NULL );
				}
			else if( newID == NegateExpression::staticGetID() ) {
				newExpression = new NegateExpression( NULL );
				}
			else if( newID == PowerExpression::staticGetID() ) {
				newExpression = new PowerExpression( NULL, NULL );
				}
			else if( newID == ProductExpression::staticGetID() ) {
				newExpression = new ProductExpression( NULL, NULL );
				}
			else if( newID == SinExpression::staticGetID() ) {
				newExpression = new SinExpression( NULL );
				}
			else if( newID == SumExpression::staticGetID() ) {
				newExpression = new SumExpression( NULL, NULL );
				}
			else {
				printf( "Error in ExpressionMutator:  ID does not " );
				printf( "match a known expression type: %d\n", newID );
				}


			// at this point, we have constructed a new expression node
			// with the same number of arguments as the old  node

			// we need to fill in the arguments from inExpression
			
			for( int i=0; i<inExpression->getNumArguments(); i++ ) {
				// use extractArgument so that the argument
				// won't be destroyed when inExpression is destroyed
				Expression *argument = inExpression->extractArgument( i );
				
				newExpression->setArgument( i, argument );
				}

			// destroy the old node
			delete inExpression;
			}

		// mutate the sub nodes
		for( int i=0; i<newExpression->getNumArguments(); i++ ) {
			Expression *argument = newExpression->extractArgument( i );
			
			// mutate the argument
			argument = mutateExpression( argument,
										 inMutationProb,
										 inMaxMutation,
										 inRandSource );
			
			newExpression->setArgument( i, argument );
			}

		return newExpression;
		}

	}


	
#endif
