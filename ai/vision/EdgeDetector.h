/*
 * Modification History
 *
 * 2001-February-13		Jason Rohrer
 * Created.  
 *
 * 2001-February-16		Jason Rohrer
 * Added a copy() function.  
 *
 * 2001-February-21		Jason Rohrer
 * Added a constructor for setting defaults.
 */
 
 
#ifndef EDGE_DETECTOR_INCLUDED
#define EDGE_DETECTOR_INCLUDED 


#include "minorGems/graphics/Image.h"

/**
 * Interface for class that can detect edges.
 *
 * @author Jason Rohrer
 */
class EdgeDetector {
	
	public:
		
		/**
		 * Sets the image channel to be processed.
		 * Note that if this channel is out of range for an image pair,
		 * channel 0 should be processed by default.
		 *
		 * @param inChannel the channel number.  Defaults to 0.
		 */
		void setImageChannel( int inChannel );
		
		
		/**
		 * Gets the image channel to be processed.
		 *
		 * @return the channel number.
		 */
		int getImageChannel();
		
		
		/**
		 * Finds edges in an image.
		 *
		 *
		 * Note that specific implementations may use the
		 * channels in the image in different ways.
		 * Base functionality only guarentees that the channel
		 * specified by getImageChannel() is used.
		 *
		 * @param inImage the image to find edges in.
		 *   Different implentations may use multiple channels
		 *   in inImage differently.  Base functionality
		 *   only guarentees that channel 0 is used.
		 *
		 * @return an edge map for the image.  Pixels
		 *   in the edge map are set to 0 where there
		 *   is no edge, and 1 where there is an edge.  Must be
		 *   destroyed by caller.
		 */
		virtual Image *findEdges( Image *inImage ) = 0;
	
		
		/**
		 * Creates a edge detector object identical to this one.
		 *
		 * @return a copy of this edge detector object.
		 */
		virtual EdgeDetector *copy() = 0;
	
	protected:
		int mChannelNumber;
		
		EdgeDetector();
	};



inline EdgeDetector::EdgeDetector() 
	: mChannelNumber( 0 ) {
	
	}



inline void EdgeDetector::setImageChannel( int inChannel ) {
	mChannelNumber = inChannel;
	}


		
inline int EdgeDetector::getImageChannel() {

	return mChannelNumber;
	}


	
#endif
