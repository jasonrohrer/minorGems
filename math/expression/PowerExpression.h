/*
 * Modification History 
 *
 * 2001-February-11		Jason Rohrer
 * Created.
 *
 * 2001-March-10   Jason Rohrer
 * Added implementation of copy function. 
 */
 
 
#ifndef POWER_EXPRESSION_INCLUDED
#define POWER_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "BinaryOperationExpression.h" 
 
#include <math.h> 
 
/**
 * Expression implementation of a binary power operation.
 * Raises the first argument to the power of the second,
 * as in argA^argB. 
 *
 * @author Jason Rohrer 
 */
class PowerExpression : public BinaryOperationExpression { 
	
	public:
		
		/**
		 * Constructs a binary power operation expression.
		 *
		 * Both arguments are destroyed when the class is destroyed.
		 *
		 * @param inArgumentA the first argument.
		 * @param inArgumentB the second argument.
		 */
		PowerExpression( Expression *inArgumentA, 
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
long PowerExpression::mID = 4;



inline PowerExpression::PowerExpression( Expression *inArgumentA, 
	Expression *inArgumentB ) 
	: BinaryOperationExpression( inArgumentA, inArgumentB ) {
	
	}



inline double PowerExpression::evaluate() {
	return pow( mArgumentA->evaluate(), mArgumentB->evaluate() );
	}



inline long PowerExpression::getID() {	
	return mID;
	}	



inline long PowerExpression::staticGetID() {
	return mID;
	}



inline void PowerExpression::print() {
	printf( "( " );
	
	mArgumentA->print();
	printf( " ^ " );
	mArgumentB->print();
	printf( " )" );
	}



inline Expression *PowerExpression::copy() {
	PowerExpression *copy =
		new PowerExpression( mArgumentA->copy(),
							   mArgumentB->copy() );
	return copy;
	}



#endif
