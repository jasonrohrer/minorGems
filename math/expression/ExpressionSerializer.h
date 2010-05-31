/*
 * Modification History 
 *
 * 2001-February-11		Jason Rohrer
 * Created. 
 *
 * 2001-March-7		Jason Rohrer
 * Removed and extra if statement from the deserialization function.   
 *
 * 2001-August-31   Jason Rohrer
 * Added support for MultiConstantArgumentExpressions.   
 *
 * 2001-September-4   Jason Rohrer
 * Added support for FixedConstantExpressions.
 *
 * 2001-October-9   Jason Rohrer
 * Fixed a bug in deserializing MultiConstantArgumentExpressions.
 */
 
 
#ifndef EXPRESSION_SERIALIZER_INCLUDED
#define EXPRESSION_SERIALIZER_INCLUDED 
 
#include "Expression.h" 
#include "UnaryOperationExpression.h" 
#include "BinaryOperationExpression.h"

#include "ConstantExpression.h"
#include "InvertExpression.h"
#include "ProductExpression.h"
#include "PowerExpression.h"
#include "SumExpression.h"
#include "NegateExpression.h"
#include "SinExpression.h"
#include "FixedConstantExpression.h"
#include "MultiConstantArgumentExpression.h"

#include "minorGems/io/OutputStream.h"
#include "minorGems/io/InputStream.h"
#include "minorGems/io/TypeIO.h"

 
/**
 * Utility class for serializing expressions 
 *
 * @author Jason Rohrer 
 */
class ExpressionSerializer { 
	
	public:
		
		
		/**
		 * Serializes an expression onto a stream.
		 *
		 * @param inExpression the expression to serialize.
		 * @param inOutputStream the stream to write to.
		 *
		 * @return the number of bytes written.
		 */ 
		static long serializeExpression( Expression *inExpression,
			OutputStream *inOutputStream );
		
		
		/**
		 * Deserializes an expression from a stream.
		 *
		 * @param outExpression a pointer to where the expression
		 *   pointer will be returned.  A new expression is created,
		 *   and it must be destroyed by the caller.
		 * @param inInputStream the stream to read from.
		 *
		 * @return the number of bytes read.
		 */ 
		static long deserializeExpression( Expression **outExpression,
			InputStream *inInputStream );
		
	};



// these can both be implemented recursively... exciting!

inline long ExpressionSerializer::serializeExpression( 
	Expression *inExpression, OutputStream *inOutputStream ) {
	
	int numBytes = 0;
	
	long expressionID = inExpression->getID();
	
	// write the expression ID
	numBytes += inOutputStream->writeLong( expressionID );
	
	
	// first, deal with constant case
	if( expressionID == ConstantExpression::staticGetID() ) {
		// write the constant
		ConstantExpression *c = (ConstantExpression *)inExpression;
		numBytes += inOutputStream->writeDouble( c->getValue() );
		}
	else if( expressionID == FixedConstantExpression::staticGetID() ) {
		// write the constant
		FixedConstantExpression *c = (FixedConstantExpression *)inExpression;
		numBytes += inOutputStream->writeDouble( c->getValue() );
		}
	// next deal with the multi-argument case
	else if( expressionID == MultiConstantArgumentExpression::staticGetID() ) {
		MultiConstantArgumentExpression *m =
			(MultiConstantArgumentExpression*)inExpression;

		Expression *wrappedExpression =
			m->getWrappedExpression();

		numBytes += serializeExpression( 
			wrappedExpression,
			inOutputStream );
		}
	// finally, deal with the general case
	else {
		// write each of the expression's arguments in succession
		for( int i=0; i<inExpression->getNumArguments(); i++ ) {
			// call serialize recursively
			numBytes += serializeExpression( 
				inExpression->getArgument( i ),
				inOutputStream );
			}
		}
	
	return numBytes;
	}
 
 
 
inline long ExpressionSerializer::deserializeExpression( 
	Expression **outExpression, InputStream *inInputStream ) {
	
	int numBytes = 0;
	
	long expressionID;
	
	// read the expression ID
	numBytes += inInputStream->readLong( &expressionID );
	
	// first, deal with constant case
	if( expressionID == ConstantExpression::staticGetID() ) {
		double constantValue;
		numBytes += inInputStream->readDouble( &constantValue );
			
		*outExpression = new ConstantExpression( constantValue );
		}
	else if( expressionID == FixedConstantExpression::staticGetID() ) {
		double constantValue;
		numBytes += inInputStream->readDouble( &constantValue );
			
		*outExpression = new FixedConstantExpression( constantValue );
		}
	// next deal with the multi-argument case
	else if( expressionID == MultiConstantArgumentExpression::staticGetID() ) {
		// call deserialize recursively
		Expression *wrappedExpression;
		numBytes += deserializeExpression( 
			&wrappedExpression,
			inInputStream );

		MultiConstantArgumentExpression *m =
			new MultiConstantArgumentExpression( wrappedExpression );		

		*outExpression = (Expression *)m;
		}
	// finally, deal with the general case
	else {
		// switch based on expression type
		
		// note that we can't use switch/case here because
		// staticGetID doesn't return a constant
		if( expressionID == InvertExpression::staticGetID() ) {
			*outExpression = new InvertExpression( NULL );
			}
		else if( expressionID == NegateExpression::staticGetID() ) {
			*outExpression = new NegateExpression( NULL );
			}
		else if( expressionID == PowerExpression::staticGetID() ) {
			*outExpression = new PowerExpression( NULL, NULL );
			}
		else if( expressionID == ProductExpression::staticGetID() ) {
			*outExpression = new ProductExpression( NULL, NULL );
			}
		else if( expressionID == SinExpression::staticGetID() ) {
			*outExpression = new SinExpression( NULL );
			}
		else if( expressionID == SumExpression::staticGetID() ) {
			*outExpression = new SumExpression( NULL, NULL );
			}
		else {
			printf( "Unknown expression type %d read from stream\n",
				expressionID );
			// default to a constant expression of 0
			*outExpression = new ConstantExpression( 0 );
			}

		// now deserialize the arguments	
		
		// read each of the expression's arguments in succession
		for( int i=0; i<(*outExpression)->getNumArguments(); i++ ) {
			
			// call deserialize recursively
			Expression *argument;
			numBytes += deserializeExpression( 
				&argument,
				inInputStream );
			
			// set the argument into our expression	
			(*outExpression)->setArgument( i, argument );
			}	
		
		}	// end non-constant case
	
	return numBytes;
	}


	
#endif
