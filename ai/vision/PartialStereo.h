/*
 * Modification History
 *
 * 2001-February-25		Jason Rohrer
 * Created.  
 *
 * 2001-February-26		Jason Rohrer
 * Added a missing include.   
 */
 
 
#ifndef PARTIAL_STEREO_INCLUDED
#define PARTIAL_STEREO_INCLUDED 


#include "minorGems/graphics/Image.h"

#include "Stereo.h"

/**
 * Extension of Stereo for a class that can compute 
 * depth maps from stereo pairs
 * for part of an image.
 *
 * @author Jason Rohrer
 */
class PartialStereo : public Stereo {
	
	public:
		/**
		 * Sets the range of an input image that the stereo will
		 * be computed for.
		 *
		 * All parameters are in [0,1].
		 *
		 * @param inXStart the start x range.  Defaults to 0.
		 * @param inXEnd the end x range.  Defaults to 1.
		 * @param inYStart the start y range.  Defaults to 0.
		 * @param inYEnd the end y range.  Defaults to 1.
		 */
		void setRange( double inXStart, double inXEnd, 
			double inYStart, double inYEnd );
		
	
	protected:
		
		// sets defaults for range
		PartialStereo( int inMaxDisparity );
		
		double mYStart;
		double mYEnd;
		double mXStart;
		double mXEnd;
		
	};



inline PartialStereo::PartialStereo( int inMaxDisparity )
	: Stereo( inMaxDisparity ),
	mYStart( 0 ), mYEnd( 1.0 ), mXStart( 0 ), mXEnd( 1.0 ) {
	
	}



inline void PartialStereo::setRange( double inXStart, double inXEnd, 
	double inYStart, double inYEnd ) {
	mXStart = inXStart;
	mXEnd = inXEnd;
	mYStart = inYStart;
	mYEnd = inYEnd;
	}


	
#endif
