/*
 * Modification History
 *
 * 2001-February-26		Jason Rohrer
 * Created.     
 */
 
 
#ifndef NETWORKED_PARTIAL_STEREO_INCLUDED
#define NETWORKED_PARTIAL_STEREO_INCLUDED 


#include "PartialStereo.h"
#include "EdgeDetector.h"

#include "minorGems/network/SocketStream.h"

#include "minorGems/graphics/ImageColorConverter.h"

#include <float.h>
#include <limits.h>
#include <stdio.h>

/**
 * Partial stereo implementation that sends data over the network
 * to be processed.
 *
 * @author Jason Rohrer
 */
class NetworkedPartialStereo : public PartialStereo {
	
	public:
		
		/**
		 * Constructs a local window stereo object.
		 *
		 * @param inStream the network stream to send source
		 *   images and received produced images on.  Must be
		 *   destroyed by caller.
		 */
		NetworkedPartialStereo( SocketStream *inStream );
		
		
		// implements the stereo interface
		virtual Image *computeDepthMap( Image *inLeft, Image *inRight );
		
		// this is provided to fully implement Stereo, but
		// note that because of the underlying SocketStream, 
		// making a copy of an instance of this class doesn't
		// make sense.
		// Currently returns NULL.
		virtual Stereo *copy();
		
	private:
		SocketStream *mStream;
	};



inline NetworkedPartialStereo::NetworkedPartialStereo( 
	SocketStream *inStream )
	: PartialStereo( 1 ), mStream( inStream ) {
	
	// note the "dummy" inMaxDisparity value passed into
	// the PartialStereo constructor, since disparity
	// is not used on this end of the stream
	
	}



inline Stereo *NetworkedPartialStereo::copy() {

	return NULL;
	}



// note that this code is a little messy, because it was copied
// from	localStereo.cpp without much modification.
inline Image *NetworkedPartialStereo::computeDepthMap( Image *inLeft, 
	Image *inRight ) {
	
	// first, send ranges, x and then y
	mStream->writeDouble( mXStart );
	mStream->writeDouble( mXEnd );
	mStream->writeDouble( mYStart );
	mStream->writeDouble( mYEnd );
	
	
	// send left, then right images
	inLeft->serialize( mStream );
	inRight->serialize( mStream );
	
	// the receive the result
	Image *outImage = new Image( 1, 1, 1 );
	outImage->deserialize( mStream );
	
	
	return outImage;
	}
		
		
		
#endif
