/*
 * Modification History
 *
 * 2001-April-12   Jason Rohrer
 * Created.
 *
 * 2001-April-29   Jason Rohrer
 * Updated comments to cover failed image retrievals.
 */
 
 
#ifndef IMAGE_SOURCE_INCLUDED
#define IMAGE_SOURCE_INCLUDED 

#include "minorGems/graphics/Image.h"
	
/**
 * Interface for a class that can produce a series of
 * images.
 *
 * Designed to be used by StereoClient as a source images for
 * stereo image pairs.
 *
 * @author Jason Rohrer
 */
class ImageSource {
	
	public:

		/**
		 * Gets an image from this image source.
		 *
		 * May block until the next image is available.
		 *
		 * @return the next available image, or NULL if getting
		 *   the next image fails.  Must be destroyed by caller.
		 */
		virtual Image *getNextImage() = 0;
			
	};



#endif







