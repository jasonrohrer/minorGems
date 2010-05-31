/*
 * Modification History
 *
 * 2001-February-25	Jason Rohrer
 * Created.
 *
 * 2001-April-12   Jason Rohrer
 * Changed to comply with new FileInput/OutputStream interface
 * (underlying File must be destroyed explicitly).
 */
 
 
#ifndef FILE_STEREO_PROCESSOR_INCLUDED
#define FILE_STEREO_PROCESSOR_INCLUDED 

#include "StereoProcessor.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ImageColorConverter.h"

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"
#include "minorGems/graphics/converters/BMPImageConverter.h"

/**
 * Implementation of StereoProcessor that writes all images to file.
 *
 * @author Jason Rohrer
 */
class FileStereoProcessor : public StereoProcessor {
	
	public:
		

		// implements StereoProcessor interface
		virtual void addData( Image *inLeftImage, Image *inRightImage, 
			Image *inStereoData );	
				
	};



inline void FileStereoProcessor::addData( Image *inLeftImage, 
	Image *inRightImage, Image *inStereoData ) {
	
	BMPImageConverter *converter = new BMPImageConverter();
	
	File *leftOutFile = new File( NULL, "receivedLeft.bmp", 16 );
	FileOutputStream *leftOutStream = new FileOutputStream( leftOutFile );
	
	File *rightOutFile = new File( NULL, "receivedRight.bmp", 17 );
	FileOutputStream *rightOutStream = new FileOutputStream( rightOutFile );
	
	File *stereoOutFile = new File( NULL, "computedStereo.bmp", 18 );
	FileOutputStream *stereoOutStream = new FileOutputStream( stereoOutFile );
	
	Image *threeChannelImage =
		ImageColorConverter::grayscaleToRGB( inStereoData );
	
	printf( "Writing images to file.\n" );
		
	converter->formatImage( inLeftImage, leftOutStream );	
	converter->formatImage( inRightImage, rightOutStream );	
	converter->formatImage( threeChannelImage, stereoOutStream );
	
	delete leftOutStream;
	delete rightOutStream;
	delete leftOutFile;
	delete rightOutFile;
	
	delete stereoOutStream;
	delete converter;
	
	delete threeChannelImage;

	// delete the data
	delete inLeftImage;
	delete inRightImage;
	delete inStereoData;
	}
	
	
	
#endif
