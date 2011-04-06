/*
 * Modification History
 *
 * 2001-April-27   Jason Rohrer
 * Created.
 *
 * 2001-April-29   Jason Rohrer
 * Finished implementation.
 * Added an optimization to formatImage, but it did not improve
 * performance, so it has been commented out.
 *
 * 2011-April-5     Jason Rohrer
 * Fixed float-to-int conversion.  
 */
 
/**
 * Unix-specific JPEGImageConverter implementation
 *
 * Code for compression and decompression modeled after IJG's
 * libjpeg example code.
 *
 * For now, it use libjpeg to write converted data out to
 * file, and then reads it back in.
 */


#include "minorGems/graphics/converters/JPEGImageConverter.h"
#include "minorGems/io/file/File.h"


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <string.h>

// include the jpeg library as a C file.
// (yuk... spent way too much time trying to figure this one out!)    
extern "C" {
#include<jpeglib.h>
	}

/*
 * <setjmp.h> is used for the decompression
 * error recovery mechanism.
 */

#include <setjmp.h>



void JPEGImageConverter::formatImage( Image *inImage, 
									  OutputStream *inStream ) {
	
	if( inImage->getNumChannels() != 3 ) {
		printf( "JPEGImageConverter only works on 3-channel images.\n" );
		return;
		}

	// most of this code was copied without modification from
	// IJG's example.c
	
	// This struct contains the JPEG compression parameters and pointers to
	// working space (which is allocated as needed by the JPEG library).
	// It is possible to have several such structures, representing multiple
	// compression/decompression processes, in existence at once.  We refer
	// to any one struct (and its associated working data) as a "JPEG object".
	 
	struct jpeg_compress_struct cinfo;
 
	// This struct represents a JPEG error handler.  It is declared separately
	// because applications often want to supply a specialized error handler
	// (see the second half of this file for an example).  But here we just
	// take the easy way out and use the standard error handler, which will
	// print a message on stderr and call exit() if compression fails.
	// Note that this struct must live as long as the main JPEG parameter
	// struct, to avoid dangling-pointer problems.
	 
	struct jpeg_error_mgr jerr;

	// More stuff 
	FILE * outfile;		// target file 
	JSAMPROW row_pointer[1];	// pointer to JSAMPLE row[s] 
	int row_stride;		// physical row width in image buffer 

	// Step 1: allocate and initialize JPEG compression object 

	// We have to set up the error handler first, in case the initialization
	// step fails.  (Unlikely, but it could happen if you are out of memory.)
	// This routine fills in the contents of struct jerr, and returns jerr's
	// address which we place into the link field in cinfo.
	 
	cinfo.err = jpeg_std_error( &jerr );
	// Now we can initialize the JPEG compression object. 
	jpeg_create_compress( &cinfo );

	// Step 2: specify data destination (eg, a file) 
	// Note: steps 2 and 3 can be done in either order. 

	// use a temp file with a random name to make this more
	// thread-safe
	char *fileName = new char[99];
	sprintf( fileName, "temp%d.dat", rand() );
	
	// Here we use the library-supplied code to send compressed data to a
	// stdio stream.  You can also write your own code to do something else.
	// VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	// requires it in order to write binary files.
	 
	if( ( outfile = fopen( fileName, "wb" ) ) == NULL ) {
		printf( "can't open jpeg conversion temp file %s\n", fileName );
		return;
		}
	
	jpeg_stdio_dest( &cinfo, outfile );
	
	
	// Step 3: set parameters for compression 

	// First we supply a description of the input image.
	// Four fields of the cinfo struct must be filled in:
	 

	// image width and height, in pixels 
	cinfo.image_width = inImage->getWidth(); 	
	cinfo.image_height = inImage->getHeight();
	cinfo.input_components = 3;		// # of color components per pixel 
	cinfo.in_color_space = JCS_RGB; 	// colorspace of input image 
	// Now use the library's routine to set default compression parameters.
	// (You must set at least cinfo.in_color_space before calling this,
	// since the defaults depend on the source color space.)
	
	jpeg_set_defaults( &cinfo );
	// Now you can set any non-default parameters you wish to.
	// Here we just illustrate the use of
	// quality (quantization table) scaling:
	 
	jpeg_set_quality( &cinfo, mQuality,
					  TRUE ); // limit to baseline-JPEG values

	// Step 4: Start compressor 

	// TRUE ensures that we will write a complete interchange-JPEG file.
	// Pass TRUE unless you are very sure of what you're doing.
	 
	jpeg_start_compress( &cinfo, TRUE );

	// Step 5: while (scan lines remain to be written) 
	//           jpeg_write_scanlines(...); 

	// Here we use the library's state variable cinfo.next_scanline as the
	// loop counter, so that we don't have to keep track ourselves.
	// To keep things simple, we pass one scanline per call; you can pass
	// more if you wish, though.
	 
	// JSAMPLEs per row in image_buffer 
	row_stride = cinfo.image_width * 3;

	// channels of inImage, which we will need to pull pixel values out of
	double *redChannel = inImage->getChannel(0);
	double *greenChannel = inImage->getChannel(1);
	double *blueChannel = inImage->getChannel(2);

	// array that we will copy inImage pixels into
	// one scanline at a time
	row_pointer[0] = new JSAMPLE[ row_stride ];
	
	//int rowNumber = 0;
	
	while( cinfo.next_scanline < cinfo.image_height ) {
		// jpeg_write_scanlines expects an array of pointers to scanlines.
		// Here the array is only one element long, but you could pass
		// more than one scanline at a time if that's more convenient.
		 

		// make a scanline
		
		
 		int yOffset = cinfo.next_scanline * cinfo.image_width;
		
		// for each pixel in the row
		for( int p=0; p<cinfo.image_width; p++ ) {
			// index into inImage
			int pixelIndex = p + yOffset;

			// index into this row
			int startRowIndex = p * 3;

			// red
			row_pointer[0][ startRowIndex ] =
				(JSAMPLE)( lrint( redChannel[ pixelIndex ] * 255 ) );
			// green
			row_pointer[0][ startRowIndex + 1 ] =
				(JSAMPLE)( lrint( greenChannel[ pixelIndex ] * 255 ) );
			// blue
			row_pointer[0][ startRowIndex + 2 ] =
				(JSAMPLE)( lrint( blueChannel[ pixelIndex ] * 255 ) );
			}


		// now pass the scanline into libjpeg
		(void) jpeg_write_scanlines( &cinfo, row_pointer, 1 );

		//rowNumber++;
		}

	delete [] ( row_pointer[0] );
	
	
	// Step 6: Finish compression 

	jpeg_finish_compress( &cinfo );
	// After finish_compress, we can close the output file. 
    fclose( outfile );

	// Step 7: release JPEG compression object 

	// This is an important step since it
	// will release a good deal of memory.
	
	jpeg_destroy_compress( &cinfo );


	// now read the compressed data back in from file
	File *file = new File( NULL, fileName, strlen( fileName ) );

	FILE *inFile = fopen( fileName, "rb" );
	
	if( inFile  == NULL ) {
		printf( "can't open jpeg conversion temp file %s\n", fileName );
		return;
		}

	// read entire file into memory
	int fileLength = file->getLength();
	unsigned char *fileBuffer = new unsigned char[ fileLength ];
	fread( fileBuffer, 1, fileLength, inFile );
	
	// now write the entire buffer to our output stream
	inStream->write( fileBuffer,
						   fileLength );

	delete [] fileBuffer;
	
	delete file;
		
	fclose( inFile );

	// delete this temporary file
	remove( fileName );
	
	delete [] fileName;

	
	// And we're done!
	
	}



// copied this directly from IJG's example.c
//extern "C" {

	/*
	 * ERROR HANDLING:
	 *
	 * The JPEG library's standard error handler (jerror.c) is divided into
	 * several "methods" which you can override individually.  This lets you
	 * adjust the behavior without duplicating a lot of code, which you might
	 * have to update with each future release.
	 *
	 * Our example here shows how to override the "error_exit" method so that
	 * control is returned to the library's caller when a fatal error occurs,
	 * rather than calling exit() as the standard error_exit method does.
	 *
	 * We use C's setjmp/longjmp facility to return
	 * control.  This means that the
	 * routine which calls the JPEG library must
	 * first execute a setjmp() call to
	 * establish the return point.  We want the replacement error_exit to do a
	 * longjmp().  But we need to make the setjmp buffer accessible to the
	 * error_exit routine.  To do this, we make a private extension of the
	 * standard JPEG error handler object.  (If we were using C++, we'd say we
	 * were making a subclass of the regular error handler.)
	 *
	 * Here's the extended error handler struct:
	 */

	struct my_error_mgr {
			struct jpeg_error_mgr pub;	/* "public" fields */

			jmp_buf setjmp_buffer;	/* for return to caller */
		};

	typedef struct my_error_mgr * my_error_ptr;
	
	
	/*
	 * Here's the routine that will replace the standard error_exit method:
	 */

	METHODDEF(void) my_error_exit( j_common_ptr cinfo ) {
		/* cinfo->err really points to a my_error_mgr struct,
		   so coerce pointer
		*/
		my_error_ptr myerr = (my_error_ptr)( cinfo->err );
		
		/* Always display the message. */
		/* We could postpone this until after returning, if we chose. */
		(*cinfo->err->output_message)( cinfo );
		
		/* Return control to the setjmp point */
		longjmp( myerr->setjmp_buffer, 1 );
		}
	
//	}



Image *JPEGImageConverter::deformatImage( InputStream *inStream ) {

	// use a temp file with a random name to make this more
	// thread-safe
	char *fileName = new char[99];
	sprintf( fileName, "temp%d.dat", rand() );

	FILE *tempFile = fopen( fileName, "wb" );

	if( tempFile == NULL ) {
		printf( "can't open jpeg conversion temp file %s\n", fileName );
		return NULL;
		}

	// buffer for dumping stream to temp file
	unsigned char *tempBuffer = new unsigned char[1]; 
	unsigned char previousByte = 0;

	// dump the JPEG stream from the input stream into tempFile
	// so that we can pass this file to libjpeg

	/*
	// optimization:  use a buffer to prevent too many fwrite calls
	int bufferLength = 5000;
	unsigned char *fileBuffer = new unsigned char[ bufferLength ];
	int currentBufferPosition = 0;
	
	while( !( tempBuffer[0] == 0xD9 && previousByte == 0xFF ) ) {
		previousByte = tempBuffer[0];
		
		inStream->read( tempBuffer, 1 );

		fileBuffer[currentBufferPosition] = tempBuffer[0];

		if( currentBufferPosition == bufferLength - 1 ) {
			// at the end of the file buffer
			fwrite( fileBuffer, 1, bufferLength, tempFile );
			currentBufferPosition = 0;
			}
		else {
			// keep filling the fileBuffer
			currentBufferPosition++;
			}
		}
	// now write remaining fileBuffer data to file
	fwrite( fileBuffer, 1, currentBufferPosition + 1, tempFile );

	delete [] fileBuffer;
	*/
	
	// write until EOI sequence seen (0xFFD9)
	while( !( tempBuffer[0] == 0xD9 && previousByte == 0xFF ) ) {
		previousByte = tempBuffer[0];
		
		inStream->read( tempBuffer, 1 );

		fwrite( tempBuffer, 1, 1, tempFile );
		}
		
	// end of jpeg stream reached.
	fclose( tempFile );
	delete [] tempBuffer;
	

	// the remainder of this method was mostly copied from
	// IJG's example.c



	/* This struct contains the JPEG decompression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).
	 */
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	 * Note that this struct must live as long as the main JPEG parameter
	 * struct, to avoid dangling-pointer problems.
	 */
	struct my_error_mgr jerr;
	/* More stuff */
	FILE * infile;		/* source file */
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */

	/* In this example we want to open the input
	 * file before doing anything else,
	 * so that the setjmp() error recovery below can assume the file is open.
	 * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	 * requires it in order to read binary files.
	 */

	if( ( infile = fopen( fileName, "rb" ) ) == NULL ) {
		printf( "can't open jpeg conversion temp file %s\n", fileName );
		return NULL;
		}

	/* Step 1: allocate and initialize JPEG decompression object */
	
	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if( setjmp( jerr.setjmp_buffer ) ) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object,
		 * close the input file, and return.
		 */
		jpeg_destroy_decompress( &cinfo );
		fclose( infile );
		printf( "error in decompressing jpeg from stream.\n" );
		return NULL;
		}
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress( &cinfo );

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src( &cinfo, infile );

	/* Step 3: read file parameters with jpeg_read_header() */

	(void) jpeg_read_header( &cinfo, TRUE );
	/* We can ignore the return value from jpeg_read_header since
	 *   (a) suspension is not possible with the stdio data source, and
	 *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	 * See libjpeg.doc for more info.
	 */

	/* Step 4: set parameters for decompression */

	/* In this example, we don't need to change any of the defaults set by
	 * jpeg_read_header(), so we do nothing here.
	 */

	/* Step 5: Start decompressor */

	(void) jpeg_start_decompress( &cinfo );
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	/* We may need to do some setup of our own at this point before reading
	 * the data.  After jpeg_start_decompress() we have the correct scaled
	 * output image dimensions available, as well as the output colormap
	 * if we asked for color quantization.
	 * In this example, we need to make an output work buffer of the right size.
	 */ 
	/* JSAMPLEs per row in output buffer */

	int imageWidth = cinfo.output_width;
	int imageHeight = cinfo.output_height;

	// the return image with 3 channels
	Image *returnImage = new Image( imageWidth, imageHeight, 3, false );

	// channels of returnImage,
	// which we will need to put pixel values into of
	double *redChannel = returnImage->getChannel(0);
	double *greenChannel = returnImage->getChannel(1);
	double *blueChannel = returnImage->getChannel(2);
	int currentIndex = 0;
	
	
	row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that
	 * will go away when done with image
	 */
	buffer = ( *cinfo.mem->alloc_sarray )
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1 );

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 */
	int rowNumber = 0;
	double inv255 = 1.0 / 255.0;
	
	while( cinfo.output_scanline < cinfo.output_height ) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void) jpeg_read_scanlines( &cinfo, buffer, 1 );

		// write the scanline into returnImage
		
		int yOffset = rowNumber * cinfo.output_width;
		
		// for each pixel in the row
		// copy it into the return image channels
		for( int p=0; p<cinfo.output_width; p++ ) {

			// index into inImage
			int pixelIndex = p + yOffset;

			// index into this row
			int startRowIndex = p * 3;

			// red
			redChannel[ pixelIndex ] =
				buffer[0][ startRowIndex ] * inv255;
			// green
			greenChannel[ pixelIndex ] =
				buffer[0][ startRowIndex + 1 ] * inv255;
			// blue
			blueChannel[ pixelIndex ] =
				buffer[0][ startRowIndex + 2 ] * inv255;
			}
		
		rowNumber++;
		}
	
	/* Step 7: Finish decompression */

	(void) jpeg_finish_decompress( &cinfo );
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will
	 * release a good deal of memory.
	 */
	jpeg_destroy_decompress( &cinfo );

	/* After finish_decompress, we can close the input file.
	 * Here we postpone it until after no more JPEG errors are possible,
	 * so as to simplify the setjmp error logic above.  (Actually, I don't
	 * think that jpeg_destroy can do an error exit,
	 * but why assume anything...)
	 */
	fclose( infile );

	/* At this point you may want to check to see whether any corrupt-data
	 * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	 */

	// delete this temporary file
	remove( fileName );
	
	delete [] fileName;
	
	/* And we're done! */
	return returnImage;
	}

