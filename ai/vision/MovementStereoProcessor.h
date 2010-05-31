/*
 * Modification History
 *
 * 2001-February-25	Jason Rohrer
 * Created.
 * Added file writing capabilities for debugging.
 * Fixed multi-def guard.
 *
 * 2001-March-4		Jason Rohrer
 * Added a sleep call after sending each move to
 * ensure that the move has time to execute before we return.
 *
 * 2001-April-12   Jason Rohrer
 * Changed to comply with new FileInput/OutputStream interface
 * (underlying File must be destroyed explicitly).
 *
 * 2001-April-29   Jason Rohrer
 * Changed to use new MoveProcessor interface.
 *
 * 2001-May-7   Jason Rohrer
 * Added support for turning output image files on and off.
 */
 
 
#ifndef MOVEMENT_STEREO_PROCESSOR_INCLUDED
#define MOVEMENT_STEREO_PROCESSOR_INCLUDED 

#include "StereoProcessor.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ImageColorConverter.h"

#include "minorGems/ai/robotics/StereoMoveGenerator.h"
#include "minorGems/ai/robotics/MoveProcessor.h"

#include "minorGems/graphics/ImageColorConverter.h"

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"
#include "minorGems/graphics/converters/BMPImageConverter.h"

#include "minorGems/system/Thread.h"

#include <time.h>
#include <string.h>
#include <stdio.h>

/**
 * Implementation of StereoProcessor that 
 * passes data to a StereoMoveGenerator.
 *
 * @author Jason Rohrer
 */
class MovementStereoProcessor : public StereoProcessor {
	
	public:
		/**
		 * Constructs a MovementStereoProcessor.
		 *
		 * @param inGenerator the move generator that all image
		 *   data will be passed to.  Will be destroyed
		 *   when this class is destroyed.
		 * @param inMoveProcessor the processor that executes
		 *   robot moves.  Will be destroyed
		 *   when this class is destroyed.
		 * @param inOutputFiles set to true if this processor
		 *   should output each image set processed to file.
		 */
		MovementStereoProcessor( StereoMoveGenerator *inGenerator,
			MoveProcessor *inMoveProcessor, char inOutputFiles );
	
		~MovementStereoProcessor();

		// implements StereoProcessor interface
		virtual void addData( Image *inLeftImage, Image *inRightImage, 
			Image *inStereoData );	
	
	private:
		StereoMoveGenerator *mGenerator;
		MoveProcessor *mMoveProcessor;

		char mOutputFiles;
	
	};



inline MovementStereoProcessor::MovementStereoProcessor( 
	StereoMoveGenerator *inGenerator,
	MoveProcessor *inMoveProcessor, char inOutputFiles )
	: mGenerator( inGenerator ), mMoveProcessor( inMoveProcessor ),
	  mOutputFiles( inOutputFiles ) {
	
	}



inline MovementStereoProcessor::~MovementStereoProcessor() {
	delete mGenerator;
	delete mMoveProcessor;
	}



inline void MovementStereoProcessor::addData( Image *inLeftImage, 
	Image *inRightImage, Image *inStereoData ) {
	
	
	// first, write the data out to file
	if( mOutputFiles ) {
		printf( "Writing image data files.\n" );
		BMPImageConverter *converter = new BMPImageConverter();
	
		char *fileName = new char[100];
	
		long timeStamp = time( NULL );
		sprintf( fileName, "left%d.bmp", timeStamp );
		int nameLength = strlen( fileName );
	
		File *leftOutFile = new File( NULL, fileName, nameLength );
		FileOutputStream *leftOutStream = new FileOutputStream( leftOutFile );
	
		sprintf( fileName, "right%d.bmp", timeStamp );
		nameLength = strlen( fileName );
	
		File *rightOutFile = new File( NULL, fileName, nameLength );
		FileOutputStream *rightOutStream = new FileOutputStream( rightOutFile );
	
		sprintf( fileName, "stereo%d.bmp", timeStamp );
		nameLength = strlen( fileName );
	
		File *stereoOutFile = new File( NULL, fileName, nameLength );
		FileOutputStream *stereoOutStream = new FileOutputStream( stereoOutFile );
	
		Image *threeChannelImage =
			ImageColorConverter::grayscaleToRGB( inStereoData );
		
		converter->formatImage( inLeftImage, leftOutStream );	
		converter->formatImage( inRightImage, rightOutStream );	
		converter->formatImage( threeChannelImage, stereoOutStream );
	
		delete [] fileName;
		delete leftOutStream;
		delete rightOutStream;
		delete leftOutFile;
		delete rightOutFile;
	
		delete stereoOutStream;
		delete stereoOutFile;
		delete converter;
	
		delete threeChannelImage;

		}

	
	// now compute a move sequence
	
	mGenerator->generateMoves( 
		inLeftImage, inRightImage, inStereoData, mMoveProcessor );

	// delete the data
	delete inLeftImage;
	delete inRightImage;
	delete inStereoData;
	}
	
	
	
#endif
