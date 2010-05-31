/*
 * Modification History
 *
 * 2001-May-14   Jason Rohrer
 * Created.
 */
 
 
#ifndef IMAGE_UTILITIES_INCLUDED
#define IMAGE_UTILITIES_INCLUDED 


#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ImageColorConverter.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"
#include "minorGems/graphics/converters/BMPImageConverter.h"

/**
 * Container class for static image utility functions.
 *
 * @author Jason Rohrer
 */ 
class ImageUtilities {
	
	public:

		/**
		 * Outputs an image to file as a 24-bit BMP.  The
		 * file names are stamped with the current time.
		 *
		 * @param inImage the image to output.
		 *   If the image has 3 channels, it is interpreted
		 *   as an RGB image.  If the image has 1 channel,
		 *   it is interpreted as a grayscale image.
		 * @param inFileNamePrefix the prefix for the
		 *   name of the output file.  Must be '\0' terminated.
		 *   Must be destroyed by caller if not const.
		 *   File names are of the form:  PrefixTimestamp.bmp
		 */
		static void imageToBMPFile( Image *inImage,
									char *inFileNamePrefix );
									
				
	};



inline void ImageUtilities::imageToBMPFile( Image *inImage,
											char *inFileNamePrefix ) {


	BMPImageConverter *converter = new BMPImageConverter();

	char *fileName = new char[100];

	long timeStamp = time( NULL );

	sprintf( fileName, "%s%d.bmp", inFileNamePrefix, timeStamp );

	int nameLength = strlen( fileName );

	File *outFile = new File( NULL, fileName, nameLength );
	FileOutputStream *outStream = new FileOutputStream( outFile );

	int numChannels = inImage->getNumChannels();

	
	if(  numChannels != 3 ) {
		Image *threeChannelImage =
			ImageColorConverter::grayscaleToRGB( inImage );

		converter->formatImage( threeChannelImage, outStream );

		delete threeChannelImage;
		}

	else {
		converter->formatImage( inImage, outStream );
		}

	delete converter;
	delete [] fileName;

	delete outFile;
	delete outStream;
	}


		
#endif
