/*
 * Modification History
 *
 * 2001-February-20		Jason Rohrer
 * Created.   
 */
 
 
#ifndef DEFAULT_IMAGE_PAIR_SIDE_PROCESSOR_INCLUDED
#define DEFAULT_IMAGE_PAIR_SIDE_PROCESSOR_INCLUDED 


#include "minorGems/graphics/Image.h"

/**
 * Implementation of ImagePairSideProcessor that does nothing.
 *
 * @author Jason Rohrer
 */
class DefaultImagePairSideProcessor : public ImagePairSideProcessor {
	
	public:
		
		// implements ImagePairSideProcessor interface
		virtual void process( Image *inLeft, Image *inRight );	
	};



inline void DefaultImagePairSideProcessor::process( 
	Image *inLeft, Image *inRight ) {
	// do nothing
	
	}
	
	
	
#endif
