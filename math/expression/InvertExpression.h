/*
 * Modification History 
 *
 * 2001-February-11		Jason Rohrer
 * Created.
 *
 * 2001-March-10   Jason Rohrer
 * Added implementation of copy function. 
 */
 
 
#ifndef INVERT_EXPRESSION_INCLUDED
#define INVERT_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "UnaryOperationExpression.h" 
 
/**
 * Expression implementation of a unary invert operation. 
 *
 * @author Jason Rohrer 
 */
class InvertExpression : public UnaryOperationExpression { 
	
	public:
		
		/**
		 * Constructs a unary invert operation expression.
		 *
		 * Argument is destroyed when the class is destroyed.
		 *
		 * @param inArgument the argument.
		 */
		InvertExpression( Expression *inArgument );
		
		
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
long InvertExpression::mID = 2;


inline InvertExpression::InvertExpression( Expression *inArgument ) 
	: UnaryOperationExpression( inArgument ) {
	
	}



inline double InvertExpression::evaluate() {
	return 1 / ( mArgument->evaluate() );
	}
	
	

long InvertExpression::getID() {	
	return mID;
	}



inline long InvertExpression::staticGetID() {
	return mID;
	}



inline void InvertExpression::print() {
	printf( "( 1/" );
	mArgument->print();
	printf( " )" );
	}



inline Expression *InvertExpression::copy() {
	InvertExpression *copy = new InvertExpression( mArgument->copy() );
	return copy;
	}		


	
#endif
