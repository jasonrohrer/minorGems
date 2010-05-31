/*
 * Modification History 
 *
 * 2001-August-30		Jason Rohrer
 * Created. 
 *
 * 2001-August-31		Jason Rohrer
 * Added public access to the wrapped expression.
 * Fixed a comment and some bugs.
 * Fixed a memory leak.
 *
 * 2001-September-9		Jason Rohrer
 * Added public function for setting the wrapped expression after
 * construction.
 * Added an extractArgument() implemenation.
 * Added an extractWrappedExpression() function to work
 * around some destruction issues.
 */
 
 
#ifndef MULTI_CONSTANT_ARGUMENT_EXPRESSION_INCLUDED
#define MULTI_CONSTANT_ARGUMENT_EXPRESSION_INCLUDED 

#include "Expression.h" 
#include "ConstantExpression.h"
#include "minorGems/util/SimpleVector.h"


/**
 * An expression implementation that treats the constants
 * in another expression as assignable arguments.
 *
 * Most common usage pattern:
 * Set up an Expression containing one constant for each
 * assignable argument and pass this expression into
 * the constructor.
 *
 * Notes:
 *
 * getArugment() returns the actual constant expression
 * contained in the wrapped expression.  It therefore
 * should not be destroyed by the caller.
 *
 * setArgument() evaluates the passed-in expression
 * and then sets the value of the constant expression to the
 * value produced by the evaluation.
 * The passed-in expression must be destroyed by the caller,
 * and it can be destroyed as soon as setArgument() returns.
 * 
 * @author Jason Rohrer 
 */
class MultiConstantArgumentExpression : public Expression { 
		
	public:


		
		/**
		 * Constructs a new MultiArgumentExpression.
		 *
		 * @param inExpression the expression to use as the
		 *   body of this new expression, where each constant in
		 *   inExpression will be used as an argument to the
		 *   new expression.  Will be destroyed when this class
		 *   is destroyed.
		 */
		MultiConstantArgumentExpression( Expression *inExpression );

		~MultiConstantArgumentExpression();


		
		/**
		 * Sets the expression wrapped by this mult-argument expression.
		 *
		 * Note that calling this function destroys the currently wrapped
		 * expression.
		 *
		 * @param inExpression the expression to use as the
		 *   body of this new expression, where each constant in
		 *   inExpression will be used as an argument to the
		 *   new expression.  Will be destroyed when this class
		 *   is destroyed.
		 */
		void setWrappedExpression( Expression *inExpression );


		
		/**
		 * Gets the expression wrapped by this mult-argument expression.
		 *
		 * @return the expression wrapped by this expression.
		 *   Will be destroyed when this class is destroyed.
		 */
		Expression *getWrappedExpression();



		/**
		 * Gets the expression wrapped by this mult-argument expression,
		 * and sets the internal expression to NULL (thus, the returned
		 * argument will not be destroyed when this class is destroyed.
		 *
		 * @return the expression wrapped by this expression.  Must
		 *   be destroyed by caller.
		 */
		Expression *extractWrappedExpression();


		
		// these implement the Expression interface
		virtual double evaluate();
		virtual long getID();
		virtual long getNumArguments();
		virtual Expression *getArgument( long inArgumentNumber );
		virtual void setArgument( long inArgumentNumber, 
			Expression *inArgument );
		// note that extractArgument() always returns NULL for this class
		virtual Expression *extractArgument( long inArgumentNumber );
		virtual void print();
		virtual Expression *copy();



		/**
		 * A static version of getID().
		 */
		static long staticGetID();

		
	protected:
		Expression *mExpression;

		static long mID;

		long mNumConstants;
		ConstantExpression **mConstants;


		
		/**
		 * Finds the constants in an expression in a consistent way 
		 * (using depth-first, left-first search).
		 * 
		 * @param inExpression the expression to search for constants in.
		 * @param outExpressions a pointer where the array of found
		 *   expressions will be returned.
		 *
		 * @return the number of constants found.
		 */
		long findConstants( Expression *inExpression, 
			ConstantExpression ***outExpressions );

		
		/**
		 * Recursive proceedure used by findConstants() 
		 *
		 * @param inExpression the (sub)expression to find constants in.
		 * @param inConstantVector the vector to add found constants to.
		 */
		void findConstantsRecursive( Expression *inExpression, 
			SimpleVector<ConstantExpression*> *inConstantVector );
	};



// static init
long MultiConstantArgumentExpression::mID = 8;




inline MultiConstantArgumentExpression::
MultiConstantArgumentExpression( Expression *inExpression )
	: mExpression( NULL ), mConstants( NULL ) {

	setWrappedExpression( inExpression );

	}



inline MultiConstantArgumentExpression::
~MultiConstantArgumentExpression() {

	delete [] mConstants;

	delete mExpression;
	}



inline void MultiConstantArgumentExpression::
setWrappedExpression( Expression *inExpression ) {

	if( mExpression != NULL && mExpression != inExpression ) {
		delete mExpression;
		}
	mExpression = inExpression;

	
	if( mConstants != NULL ) {
		delete [] mConstants;
		}
	
	mNumConstants = findConstants( inExpression, &mConstants );	
	}


	
inline Expression *MultiConstantArgumentExpression::
getWrappedExpression() {
	return mExpression;
	}



inline Expression *MultiConstantArgumentExpression::
extractWrappedExpression() {
	Expression *wrappedExpression = mExpression;
	mExpression = NULL;
	return wrappedExpression;
	}



inline double MultiConstantArgumentExpression::evaluate() {
	return mExpression->evaluate();
	}



inline long MultiConstantArgumentExpression::getID() {
	return mID;
	}



inline long MultiConstantArgumentExpression::staticGetID() {
	return mID;
	}



inline long MultiConstantArgumentExpression::getNumArguments() {
	return mNumConstants;
	}



inline Expression *MultiConstantArgumentExpression::getArgument(
	long inArgumentNumber ) {

	if( inArgumentNumber >= 0 && inArgumentNumber < mNumConstants ) {
		return mConstants[inArgumentNumber];
		}
	else {
		return NULL;
		}
	}



inline void MultiConstantArgumentExpression::setArgument(
	long inArgumentNumber, Expression *inArgument ) {

	Expression *expressionToSet = getArgument( inArgumentNumber );

	if( expressionToSet != NULL ) {

		ConstantExpression *constantExpression =
			(ConstantExpression*)expressionToSet;

		// set the constant to the evaluation of inArgument
		constantExpression->setValue( inArgument->evaluate() );
		}
	else {
		printf( "MultiConstantArgumentExpression:  setting an out of range " );
		printf( "argument number, %ld\n", inArgumentNumber );
		}
	}



inline Expression *MultiConstantArgumentExpression::extractArgument(
	long inArgumentNumber ) {

	return NULL;
	}



inline void MultiConstantArgumentExpression::print() {
	mExpression->print();
	}



inline Expression *MultiConstantArgumentExpression::copy() {
	return new MultiConstantArgumentExpression( mExpression->copy() );
	}



inline long MultiConstantArgumentExpression::findConstants(
	Expression *inExpression, 
	ConstantExpression ***outExpressions ) {
	
	SimpleVector<ConstantExpression*> *constantVector
		= new SimpleVector<ConstantExpression*>();
	
	findConstantsRecursive( inExpression, constantVector );
	
	long numConstants = constantVector->size();
	
	ConstantExpression **returnArray
		= new ConstantExpression*[ numConstants ];
	
	for( int i=0; i<numConstants; i++ ) {
		
		returnArray[i] = *( constantVector->getElement(i) );
		
		}
	
	delete constantVector;
	
	// set passed-in pointer to our array
	*outExpressions = returnArray;	
	
	return numConstants;
	}



inline void MultiConstantArgumentExpression::findConstantsRecursive(
	Expression *inExpression, 
	SimpleVector<ConstantExpression*> *inConstantVector ) {
	
	if( inExpression->getID() == ConstantExpression::staticGetID() ) {
		// the passed-in expression is a constant
		inConstantVector->push_back( (ConstantExpression *)inExpression );
		return;
		}
	else {
		// call recursively on each argument
		
		for( int i=0; i<inExpression->getNumArguments(); i++ ) {

			Expression *argument = inExpression->getArgument( i );

			findConstantsRecursive( argument, inConstantVector );
			}
			
		return;		
		}
	}




#endif

