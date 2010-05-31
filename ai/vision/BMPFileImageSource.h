/*
 * Modification History
 *
 * 2001-April-12   Jason Rohrer
 * Created.
 * Finished implementation.
 */
 
 
#ifndef BMP_FILE_IMAGE_SOURCE_INCLUDED
#define BMP_FILE_IMAGE_SOURCE_INCLUDED 


#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"
#include "minorGems/graphics/converters/BMPImageConverter.h"

#include "ImageSource.h"

/**
 * Implementaion of ImageSource that repeatedly reads (and converts)
 * and image from a BMP file.
 *
 * @author Jason Rohrer
 */
class BMPFileImageSource : public ImageSource {
	
	public:

		/**
		 * Constructs a BMPFileImageSource.
		 *
		 * @param inFile the file to read the BMP information from.
		 *   Is destroyed when this class is destroyed.
		 */
		BMPFileImageSource( File *inFile );


		~BMPFileImageSource();
		
		
		// implements ImageSource interface
		virtual Image *getNextImage();


		
	private:
		File *mFile;
		BMPImageConverter *mConverter;
		
	};



inline BMPFileImageSource::BMPFileImageSource( File *inFile )
	: mFile( inFile ), mConverter( new BMPImageConverter ) {
	
	}



inline BMPFileImageSource::~BMPFileImageSource() {
	delete mFile;
	delete mConverter;
	}



inline Image *BMPFileImageSource::getNextImage() {

	FileInputStream *fileStream = new FileInputStream( mFile );

	Image *returnImage = mConverter->deformatImage( fileStream );

	// deleting file stream does not delete file
	delete fileStream;

	return returnImage;
	}



#endif







