/*
 * Modification History
 *
 * 2001-February-13		Jason Rohrer
 * Created.
 *
 * 2001-February-16		Jason Rohrer
 * Changed disparity functions to be virtual for overloading.
 *
 * 2001-February-21		Jason Rohrer
 * Added a default for the channel number.   
 */
 
 
#ifndef STEREO_INCLUDED
#define STEREO_INCLUDED 


#include "minorGems/graphics/Image.h"

/**
 * Interface for class that can compute depth maps from stereo pairs.
 *
 * @author Jason Rohrer
 */
class Stereo {
	
	public:
		
		/**
		 * Creates a stereo object identical to this one.
		 *
		 * @return a copy of this stereo object.
		 */
		virtual Stereo *copy() = 0;
		
		
		/**
		 * Computes a depth map from stereo pairs.
		 *
		 * Note that specific implementations may use the
		 * channels in the image pair in different ways.
		 * Base functionality only guarentees that the channel
		 * specified by getImageChannel() is used.
		 *
		 * @param inLeft the left image.
		 * @param inRight the right image.  Must have the same
		 *   dimensions as inLeft.
		 *
		 * @return an image containing the right map, where
		 *   pixel disparity values are represented
		 *   by linearly mapping [0,maxDisparity] to [0,1].
		 *   Must be destroyed by caller.  Returns NULL in the 
		 *   case of an error.
		 */
		virtual Image *computeDepthMap( Image *inLeft, Image *inRight ) = 0;
		
		
		/**
		 * Sets the maximum disparity for any pixel in the image.
		 * Pixels in the left image map into the right image
		 * by moving right by a certain number of pixels.  In general,
		 * pixels with larger disparities correspond to objects that
		 * are closer to the camera pair.
		 *
		 * @param inMax the maximum disparity in pixels.
		 */
		virtual void setMaxDisparity( int inMax );
	
	
		/**
		 * Gets the maximum disparity for pixels.
		 *
		 * @return the maximum disparity in pixels.
		 */ 
		virtual int getMaxDisparity();
	
		
		/**
		 * Sets the image channel to be processed from each image in the pair.
		 * Note that if this channel is out of range for an image pair,
		 * channel 0 should be processed by default.
		 *
		 * @param inChannel the channel number.  Defaults to 0.
		 */
		void setImageChannel( int inChannel );
		
		
		/**
		 * Gets the image channel to be processed from each image in the pair.
		 *
		 * @return the channel number.
		 */
		int getImageChannel();
		
	
	protected:
		
		// to be called by subclasses to set the disparity
		Stereo( int inMaxDisparity );
		
		int mMaxDisparity;
		
		int mChannelNumber;
		
	};



inline void Stereo::setMaxDisparity( int inMax ) {
	mMaxDisparity = inMax;
	}
	
	
	
inline int Stereo::getMaxDisparity() {
	return mMaxDisparity;
	}



inline Stereo::Stereo( int inMaxDisparity ) 
	: mMaxDisparity( inMaxDisparity ), mChannelNumber( 0 ) {
	
	}
	


inline void Stereo::setImageChannel( int inChannel ) {
	mChannelNumber = inChannel;
	}


		
inline int Stereo::getImageChannel() {

	return mChannelNumber;
	}


	
#endif
