/*
 * Modification History
 *
 * 2001-February-20		Jason Rohrer
 * Created.   
 */
 
 
#ifndef IMAGE_PAIR_SIDE_PROCESSOR_INCLUDED
#define IMAGE_PAIR_SIDE_PROCESSOR_INCLUDED 


#include "minorGems/graphics/Image.h"

/**
 * Interface for class that can side process (non-destructive use)
 * a stereo image pair.
 *
 * @author Jason Rohrer
 */
class ImagePairSideProcessor {
	
	public:
		
		/**
		 * Passes a pair of images to this processor.  All
		 * processing is done before the return of this method.
		 *
		 * Parameters must be destroyed by the caller.
		 *
		 * @param inLeft the left image.
		 * @param inRight the right image.
		 */
		virtual void process( Image *inLeft, Image *inRight ) = 0;	
	};


#endif
