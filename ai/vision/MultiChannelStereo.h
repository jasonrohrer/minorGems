/*
 * Modification History
 *
 * 2001-February-13		Jason Rohrer
 * Created. 
 *
 * 2001-February-16		Jason Rohrer
 * Changed to overload the disparity functions from Stereo.  
 */
 
 
#ifndef MULTI_CHANNEL_STEREO_INCLUDED
#define MULTI_CHANNEL_STEREO_INCLUDED 


#include "Stereo.h"

/**
 * Stereo implementation interface that uses a single-channel
 * stereo implementation to process all channels in an image pair.
 *
 * Results are combined so that the output depth map has only one channel.
 *
 * @author Jason Rohrer
 */
class MultiChannelStereo : public Stereo {
	
	public:
		// override these from Stereo
		virtual void setMaxDisparity( int inMax );
		virtual int getMaxDisparity();
	
	protected:
		
		/**
		 * Constructs a multi-channel stereo object.
		 *
		 * @param inStereo the stereo object to be used
		 *   on each channel individually.  For the sake of 
		 *   efficiency, inStereo should be an implementation
		 *   that only processes the channel specified by
		 *   a call to its getImageChannel() function.
		 *   Is destroyed when the class is destroyed.
		 */
		MultiChannelStereo( Stereo *inStereo );
		
		
		~MultiChannelStereo();
		
		
		Stereo *mStereo;		
	};
	


inline MultiChannelStereo::MultiChannelStereo( Stereo *inStereo ) 
	: Stereo( 0 ), mStereo( inStereo ) {
	// pass in 0 for disparity to Stereo constructor, since
	// we're overriding the relevant functions anyway
	
	}
		
		
		
inline MultiChannelStereo::~MultiChannelStereo() {
	delete mStereo;
	}


// we override these to give outside control to the mStereo
// disparity settings

inline void MultiChannelStereo::setMaxDisparity( int inMax ) {
	mStereo->setMaxDisparity( inMax );
	}
	
	
	
inline int MultiChannelStereo::getMaxDisparity() {
	return mStereo->getMaxDisparity();
	}	
		
		
		
#endif
