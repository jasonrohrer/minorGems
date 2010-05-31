/*
 * Modification History
 *
 * 2001-February-17		Jason Rohrer
 * Created. 
 *
 * 2001-February-24		Jason Rohrer
 * Changed to take source images as well as a depth map.  
 */
 
 
#ifndef STEREO_PROCESSOR_INCLUDED
#define STEREO_PROCESSOR_INCLUDED 


#include "minorGems/graphics/Image.h"

/**
 * Interface for class that can process stereo disparity data.
 *
 * @author Jason Rohrer
 */
class StereoProcessor {
	
	public:
		
		/**
		 * Passes a new field of stereo disparity data to
		 * this processor.
		 *
		 * May block if this processor uses a synchronized queue.
		 *
		 * @param inLeftImage the left source image.  Does not need
		 *   to be destroyed by caller.
		 * @param inRightImage the right source image.  Does not need
		 *   to be destroyed by caller.
		 * @param inStereoData a stereo disparity map image.
		 *   A single-channel image with pixel values in [0,1]
		 *   where 0 represents no disparity and 1 represents
		 *   maximum disparity (and values are mapped linearly in between).
		 *   Does not need to be destroyed by caller, but should
		 *   not be accessed by caller after calling addData.
		 */
		virtual void addData( Image *inLeftImage, Image *inRightImage, 
			Image *inStereoData ) = 0;	
	};


#endif
