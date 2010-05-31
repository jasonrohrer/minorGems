/*
 * Modification History
 *
 * 2001-February-19		Jason Rohrer
 * Created.  
 *
 * 2001-February-24		Jason Rohrer
 * Fixed incorrect delete usage.
 *
 * 2001-April-12   Jason Rohrer
 * Changed to comply with new FileInput/OutputStream interface
 * (underlying File must be destroyed explicitly).
 */

#include <stdio.h>

#include "minorGems/graphics/Image.h"
#include "BMPImageConverter.h"

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"
#include "minorGems/io/file/FileInputStream.h"

// test function for the BMPImageConverter class
int main( char inNumArgs, char**inArgs ) {
	if( inNumArgs != 2 ) {
		printf( "must pass in a file name to write to\n" );
		return 1;
		}
	
	int length = 0;
	while( inArgs[1][length] != '\0' ) {
		length++;
		}
	
	File *file = new File( NULL, inArgs[1], length ); 
	
	
	// read image in
	FileInputStream *stream = new FileInputStream( file );
	BMPImageConverter *converter = new BMPImageConverter();
	Image *image = converter->deformatImage( stream );
	
	if( image != NULL ) {
		// write image back out
		File *fileOut = new File( NULL, "testOut.bmp", 11 );
		FileOutputStream *outStream = new FileOutputStream( fileOut );
	
		converter->formatImage( image, outStream );
		
		delete outStream;
		delete image;
		}
		
	delete stream;
	delete file;
	delete converter;
	
	
	/*
	FileOutputStream *stream = new FileOutputStream( file );
	
	BMPImageConverter *converter = new BMPImageConverter();

	Image *image = new Image( 256, 256, 3 );
	
	double *red = image->getChannel( 0 );
	double *green = image->getChannel( 1 );
	
	for( int y=0; y<image->getHeight(); y++ ) {
		for( int x=0; x<image->getWidth(); x++ ) {
			long index = y * image->getWidth() + x;
			
			red[index] = (double)y / (double)( image->getHeight() );
			green[index] = (double)x / (double)( image->getWidth() );
			//red[index] = 1.0;
			}
		}
	
	converter->formatImage( image, stream );
	
	
	delete stream;
	// delete file explicitly
	delete file;
	delete converter;
	delete image;	
	*/
	return 0;
	} 
