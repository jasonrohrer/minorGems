/*
 * Modification History
 *
 * 2001-January-19		Jason Rohrer
 * Created.
 *
 * 2006-September-25		Jason Rohrer
 * Added control over blending curve.
 * Reorderd loop for efficiency.
 */
 
 
#ifndef SEAMLESS_FILTER_INCLUDED
#define SEAMLESS_FILTER_INCLUDED 
 
#include "minorGems/graphics/ChannelFilter.h" 


#include <math.h>



/**
 * Filter that turns any image into a seamless tile.
 *
 * @author Jason Rohrer 
 */
class SeamlessFilter : public ChannelFilter { 
	
	public:

        /**
         * Constructs a filter.
         *
         * @param inBlendCurveExponent the exponent of the blending curve
         *   as we near the edge of the image.  Defaults to 1 for a linear
         *   blending curve.  Setting to 2 would result in a parabolic
         *   curve which would push the blended area closer to the edge
         *   and leave more high-contrast, unblended image data near
         *   the middle.
         */
		SeamlessFilter( double inBlendCurveExponent = 1 );
		
		// implements the ChannelFilter interface
		virtual void apply( double *inChannel, int inWidth, int inHeight );


    private:

        double mBlendingCurveExponent;
		
	};



inline SeamlessFilter::SeamlessFilter( double inBlendCurveExponent )
    : mBlendingCurveExponent( inBlendCurveExponent ) {

    }



inline void SeamlessFilter::apply( double *inChannel, 
	int inWidth, int inHeight ) {
	
	int numPixels = inWidth * inHeight;
	
	int halfHigh = inHeight/2;
	int halfWide = inWidth/2;
	
	// mix the texture with itself.  
	// as we move closer and closer to the texture edge, we should
	// start mixing in more and more of the center values
	
	double *channelCopy = new double[ numPixels ];
	
	
	// first, create an image with no seams if tiled horizontally
    // (getting rid of vertical seams)
    
    for( int x=0; x<inWidth; x++ ) {
        // overlay x coord
        int oX;
        // fractional overlay contribution
        double oXFraction;
        if( x<halfWide ) {
            // as x approaches 0, oX moves left closer to the center
            oX = halfWide + x;
            oXFraction = 1.0 - x / (double) halfWide;
            }
        else {
            // as x approaches inWidth, oX moves right towards the center
            oX = x - halfWide;
            oXFraction = oX / (double)halfWide;
            }
        
        // skip calling pow if exponent 1
        if( mBlendingCurveExponent != 1 ) {
            oXFraction = pow( oXFraction, mBlendingCurveExponent );
            }

        for( int y=0; y<inHeight; y++ ) {

			// add in x overlay
			channelCopy[ y * inWidth + x ] =
				( 1 - oXFraction ) * inChannel[ y * inWidth + x ] +
				oXFraction * inChannel[ y * inWidth + oX ];			
			}
		}
	
	
	memcpy( inChannel, channelCopy, sizeof( double ) * numPixels );
	
	
	// now get rid of vertical seams, but this time use the horizonally correct
	// data to cover the vertical seams
	for( int y=0; y<inHeight; y++ ) {
		
		// overlay y coord
		int oY;
		// fractional overlay contribution
		double oYFraction;
		if( y<halfHigh ) {
			// as y approaches 0, oY moves upwards closer to the center
			oY = halfHigh + y;
			oYFraction = 1.0 - y / (double) halfHigh;
			}
		else {
			// as y approaches inHeight, oY moves downwards towards the center
			oY = y - halfHigh;
			oYFraction = oY / (double)halfHigh;
			}
        
        // skip calling pow if exponent 1
        if( mBlendingCurveExponent != 1 ) {
            oYFraction = pow( oYFraction, mBlendingCurveExponent );
            }
        
		for( int x=0; x<inWidth; x++ ) {
	
			// add in y overlay
			channelCopy[ y * inWidth + x ] =
				( 1 - oYFraction ) * inChannel[ y * inWidth + x ] +
				oYFraction * inChannel[ oY * inWidth + x ];			
			}
		}
	
	memcpy( inChannel, channelCopy, sizeof( double ) * numPixels );
	
	delete []channelCopy;
	}	
	
	
	
#endif
