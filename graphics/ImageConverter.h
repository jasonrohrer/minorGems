/*
 * Modification History
 *
 * 2001-February-19		Jason Rohrer
 * Created.
 *
 * 2001-April-28   Jason Rohrer
 * Updated comments to deal with a failed deformatting operation.
 *
 * 2007-October-30   Jason Rohrer
 * Added virtual destructor.
 */
 
 
#ifndef IMAGE_CONVERTER_INCLUDED
#define IMAGE_CONVERTER_INCLUDED 


#include "Image.h"
#include "minorGems/io/InputStream.h"
#include "minorGems/io/OutputStream.h"

/**
 * Interface for a class that can convert Images to and from
 * various image formats.
 *
 * @author Jason Rohrer
 */
class ImageConverter {
	
	public:
		
		/**
		 * Sends an image out to a stream as a particular
		 * format.
		 *
		 * None of the parameters are destroyed by this call.
		 *
		 * @param inImage the image to convert.
		 * @param inStream the stream to write the formatted image to.
		 */
		virtual void formatImage( Image *inImage, 
			OutputStream *inStream ) = 0;
			
		/**
		 * Reads an image in from a stream as a particular
		 * format.
		 *
		 * None of the parameters are destroyed by this call.
		 *
		 * @param inStream the stream to read the formatted image.
		 *
		 * @return the deformatted image, or NULL if the deformatting
		 *   operation fails.  Must be destroyed by caller.
		 */
		virtual Image *deformatImage( InputStream *inStream ) = 0;		
        


        // to ensure proper destruction of implementing classes
        virtual ~ImageConverter() {};
      
  
	};


#endif
