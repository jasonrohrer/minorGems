/*
 * Modification History
 *
 * 2001-February-17		Jason Rohrer
 * Created.   
 *
 * 2001-February-24		Jason Rohrer
 * Changed to take source images as well as a depth map. 
 */
 
 
#ifndef DEFAULT_STEREO_PROCESSOR_INCLUDED
#define DEFAULT_STEREO_PROCESSOR_INCLUDED 


#include "StereoProcessor.h"

/**
 * Default stereo processor implementation.  
 * Does nothing except for deleting the data.
 *
 * @author Jason Rohrer
 */
class DefaultStereoProcessor : public StereoProcessor {
	
	public:
		
		// implents StereoProcessor interface
		virtual void addData( Image *inLeftImage, Image *inRightImage, 
			Image *inStereoData );	
	};



inline void DefaultStereoProcessor::addData( Image *inLeftImage, 
	Image *inRightImage, Image *inStereoData ) {
	
	delete inLeftImage;
	delete inRightImage;
	delete inStereoData;
	}



#endif
