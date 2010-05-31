/*
 * Modification History 
 *
 * 2001-September-4		Jason Rohrer
 * Created.  
 *
 * 2001-September-9		Jason Rohrer
 * Added an extractArgument() implemenation.
 */
 
 
#ifndef FIXED_CONSTANT_EXPRESSION_INCLUDED
#define FIXED_CONSTANT_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
 
/**
 * Expression implementation of a fixed constant. 
 *
 * @author Jason Rohrer 
 */
class FixedConstantExpression : public Expression { 
	
	public:
		
		/**
		 * Constructs a FixedConstant expression.
		 *
		 * @param inValue the constant value.
		 */
		FixedConstantExpression( double inValue );
		
		
		/**
		 * Gets the constant value.
		 *
		 * @return the constant value.
		 */
		double getValue();
		
		
		/**
		 * A static version of getID().
		 */
		static long staticGetID();
		
		
		// implements the Expression interface
		virtual double evaluate();
		virtual long getNumArguments();
		virtual long getID();
		virtual Expression *copy();
		
		virtual Expression *getArgument( long inArgumentNumber );
		virtual void setArgument( long inArgumentNumber, 
			Expression *inArgument );
		virtual Expression *extractArgument( long inArgumentNumber );
		virtual void print();
		
	protected:
		double mValue;
		
		static long mID;
	};


// static init
long FixedConstantExpression::mID = 9;


inline FixedConstantExpression::FixedConstantExpression( double inValue ) 
	: mValue( inValue ) {
	
	}



inline double FixedConstantExpression::getValue() {
	return mValue;
	}



inline double FixedConstantExpression::evaluate() {
	return mValue;
	}


	
long FixedConstantExpression::getNumArguments() {
	return 0;
	}	



inline long FixedConstantExpression::getID() {	
	return mID;
	}



inline Expression *FixedConstantExpression::getArgument( 
	long inArgumentNumber ) {
	
	return NULL;
	}


	
inline void FixedConstantExpression::setArgument( long inArgumentNumber, 
	Expression *inArgument ) {
	
	// do nothing
	return;
	}



inline Expression *FixedConstantExpression::extractArgument( 
	long inArgumentNumber ) {
	
	return NULL;
	}



inline Expression *FixedConstantExpression::copy() {
	FixedConstantExpression *copy = new FixedConstantExpression( mValue );
	return copy;
	}



inline long FixedConstantExpression::staticGetID() {
	return mID;
	}



inline void FixedConstantExpression::print() {
	printf( "( %f )", mValue );
	}


	
#endif







