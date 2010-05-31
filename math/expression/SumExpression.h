/*
 * Modification History 
 *
 * 2001-February-10		Jason Rohrer
 * Created.  
 *
 * 2001-March-10   Jason Rohrer
 * Added implementation of copy function.  
 */
 
 
#ifndef SUM_EXPRESSION_INCLUDED
#define SUM_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "BinaryOperationExpression.h" 
 
/**
 * Expression implementation of a binary sum operation. 
 *
 * @author Jason Rohrer 
 */
class SumExpression : public BinaryOperationExpression { 
	
	public:
		
		/**
		 * Constructs a binary sum operation expression.
		 *
		 * Both arguments are destroyed when the class is destroyed.
		 *
		 * @param inArgumentA the first argument.
		 * @param inArgumentB the second argument.
		 */
		SumExpression( Expression *inArgumentA, 
			Expression *inArgumentB );
		
		
		/**
		 * A static version of getID().
		 */
		static long staticGetID();
		
		
		// implements the Expression interface
		virtual double evaluate();
		virtual long getID();
		virtual void print();
		virtual Expression *copy();
		
	protected:
		static long mID;
		
	};



// static init
long SumExpression::mID = 7;



inline SumExpression::SumExpression( Expression *inArgumentA, 
	Expression *inArgumentB ) 
	: BinaryOperationExpression( inArgumentA, inArgumentB ) {
	
	}



inline double SumExpression::evaluate() {
	return mArgumentA->evaluate() + mArgumentB->evaluate();
	}



inline long SumExpression::getID() {	
	return mID;
	}



inline long SumExpression::staticGetID() {
	return mID;
	}



inline void SumExpression::print() {
	printf( "( " );
	
	mArgumentA->print();
	printf( " + " );
	mArgumentB->print();
	printf( " )" );
	}
			


inline Expression *SumExpression::copy() {
	SumExpression *copy =
		new SumExpression( mArgumentA->copy(),
							   mArgumentB->copy() );
	return copy;
	}				


	
#endif
