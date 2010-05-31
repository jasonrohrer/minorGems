/*
 * Modification History 
 *
 * 2001-February-11		Jason Rohrer
 * Created. 
 *
 * 2001-February-12		Jason Rohrer
 * Added code to test serialization.
 *
 * 2001-February-24		Jason Rohrer
 * Fixed incorrect delete usage.
 *
 * 2001-April-12   Jason Rohrer
 * Changed to comply with new FileInput/OutputStream interface
 * (underlying File must be destroyed explicitly).
 */


#include "Expression.h"
#include "ConstantExpression.h"
#include "PowerExpression.h"
#include "ProductExpression.h"
#include "NegateExpression.h"
#include "InvertExpression.h"
#include "SinExpression.h"
#include "LnExpression.h"
#include "SumExpression.h"

#include "ExpressionSerializer.h"

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"
#include "minorGems/io/file/FileOutputStream.h"

// test function for expressions
int main() {
	
	ProductExpression *expression =
		new ProductExpression( 
			new PowerExpression( 
				new ConstantExpression( 5 ), 
				new ConstantExpression( 6 ) ),
			new NegateExpression(
				new SinExpression(
					new ConstantExpression( 19 ) ) ) );	
	
	InvertExpression *invExpression = new InvertExpression( expression );
	
	SumExpression *sumExpression = new SumExpression( 
		invExpression, new ConstantExpression( 2 ) );
			
	sumExpression->print();
	
	
	printf( "\n" );
	
	printf( "%f\n", sumExpression->evaluate() );
	
	printf( "Writing to file.\n" );
	
	char **pathSteps = new char*[2];
	pathSteps[0] = new char[10];
	pathSteps[1] = new char[10];
	
	sprintf( pathSteps[0], "test" );
	sprintf( pathSteps[1], "file" );
	
	int *stepLength = new int[2];
	stepLength[0] = 4;
	stepLength[1] = 4;
	
	Path *path = new Path( pathSteps, 2, stepLength, false );
	
	File *file = new File( path, "test.out", 8 );
	FileOutputStream *outStream = new FileOutputStream( file, false );
	
	char *error = outStream->getLastError();
	if( error != NULL ) {
		printf( "Error:  %s\n", error );
		delete error;
		}
	
	
	ExpressionSerializer::serializeExpression( sumExpression, outStream );
	
	delete outStream;
	delete file;
	
	printf( "Reading back in from file.\n" );
	
	pathSteps = new char*[2];
	pathSteps[0] = new char[10];
	pathSteps[1] = new char[10];
	
	sprintf( pathSteps[0], "test" );
	sprintf( pathSteps[1], "file" );
	
	stepLength = new int[2];
	stepLength[0] = 4;
	stepLength[1] = 4;
	
	path = new Path( pathSteps, 2, stepLength, false );
	
	
	file = new File( path, "test.out", 8 );
	FileInputStream *inStream = new FileInputStream( file );
	error = inStream->getLastError();
	if( error != NULL ) {
		printf( "Error:  %s\n", error );
		delete error;
		}
	
	
	Expression *readExpression;
	ExpressionSerializer::deserializeExpression( &readExpression, 
		inStream );
	
	delete inStream;
	delete file;
	
	readExpression->print();
	
	
	printf( "\n" );
	
	printf( "%f\n", readExpression->evaluate() );
	
	delete sumExpression;
	delete readExpression;
	
	return 0;
	} 
