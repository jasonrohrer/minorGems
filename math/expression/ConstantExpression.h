/*
 * Modification History 
 *
 * 2001-February-11		Jason Rohrer
 * Created.  
 *
 * 2001-March-10   Jason Rohrer
 * Added implementation of copy function.
 *
 * 2001-September-9   Jason Rohrer
 * Added an extractArgument() implemenation.
 */
 
 
#ifndef CONSTANT_EXPRESSION_INCLUDED
#define CONSTANT_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
 
/**
 * Expression implementation of a constant. 
 *
 * @author Jason Rohrer 
 */
class ConstantExpression : public Expression { 
	
	public:
		
		/**
		 * Constructs a constant expression.
		 *
		 * @param inValue the constant value.
		 */
		ConstantExpression( double inValue );
		
		
		/**
		 * Sets the constant value.
		 *
		 * @param inValue the constant value.
		 */
		void setValue( double inValue );
		
		
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
long ConstantExpression::mID = 1;


inline ConstantExpression::ConstantExpression( double inValue ) 
	: mValue( inValue ) {
	
	}
		
		
		
inline void ConstantExpression::setValue( double inValue ) {
	mValue = inValue;
	}
	


inline double ConstantExpression::getValue() {
	return mValue;
	}



inline double ConstantExpression::evaluate() {
	return mValue;
	}


	
long ConstantExpression::getNumArguments() {
	return 0;
	}	



inline long ConstantExpression::getID() {	
	return mID;
	}



inline Expression *ConstantExpression::getArgument( 
	long inArgumentNumber ) {
	
	return NULL;
	}


	
inline void ConstantExpression::setArgument( long inArgumentNumber, 
	Expression *inArgument ) {
	
	// do nothing
	return;
	}



inline Expression *ConstantExpression::extractArgument( 
	long inArgumentNumber ) {
	
	return NULL;
	}



inline Expression *ConstantExpression::copy() {
	ConstantExpression *copy = new ConstantExpression( mValue );
	return copy;
	}



inline long ConstantExpression::staticGetID() {
	return mID;
	}



inline void ConstantExpression::print() {
	printf( "( %f )", mValue );
	}


	
#endif
