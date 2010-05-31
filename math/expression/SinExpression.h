/*
 * Modification History 
 *
 * 2001-February-11		Jason Rohrer
 * Created.
 *
 * 2001-March-10   Jason Rohrer
 * Added implementation of copy function. 
 */
 
 
#ifndef SIN_EXPRESSION_INCLUDED
#define SIN_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "UnaryOperationExpression.h" 

#include <math.h>
 
/**
 * Expression implementation of a unary sin operation. 
 *
 * @author Jason Rohrer 
 */
class SinExpression : public UnaryOperationExpression { 
	
	public:
		
		/**
		 * Constructs a unary sin operation expression.
		 *
		 * Argument is destroyed when the class is destroyed.
		 *
		 * @param inArgument the argument.
		 */
		SinExpression( Expression *inArgument );
		
		
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
long SinExpression::mID = 6;



inline SinExpression::SinExpression( Expression *inArgument ) 
	: UnaryOperationExpression( inArgument ) {
	
	}



inline double SinExpression::evaluate() {
	return sin( mArgument->evaluate() );
	}



inline long SinExpression::getID() {	
	return mID;
	}	



inline long SinExpression::staticGetID() {
	return mID;
	}



inline void SinExpression::print() {
	printf( "( sin" );
	
	mArgument->print();
	printf( " )" );
	}
			


inline Expression *SinExpression::copy() {
	SinExpression *copy =
		new SinExpression( mArgument->copy() );
	
	return copy;
	}		


	
#endif
