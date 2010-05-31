/*
 * Modification History
 *
 * 2000-December-21		Jason Rohrer
 * Created. 
 */
 
 
#ifndef THRESHOLD_FILTER_INCLUDED
#define THRESHOLD_FILTER_INCLUDED

#include "minorGems/graphics/ChannelFilter.h" 
 
/**
 * Threshold filter.
 *
 * @author Jason Rohrer 
 */
class ThresholdFilter : public ChannelFilter { 
	
	public:
		
		/**
		 * Constructs a threshold filter.
		 *
		 * @param inThreshold threshold value.  Channel values
		 *   above or equal to inThreshold are set to 1, while
		 *   all other values are set to 0.
		 */
		ThresholdFilter( double inThreshold );
		
		
		/**
		 * Sets the threshold.
		 *
		 * @param inThreshold threshold value.  Channel values
		 *   above or equal to inThreshold are set to 1, while
		 *   all other values are set to 0.
		 */
		void setThreshold( double inThreshold );
		
		
		// implements the ChannelFilter interface
		void apply( double *inChannel, int inWidth, int inHeight );

	private:
		double mThreshold;
	};
	
	
	
inline ThresholdFilter::ThresholdFilter( double inThreshold ) 
	: mThreshold( inThreshold ) {	
	
	}



inline void ThresholdFilter::setThreshold( double inThreshold ) {
	mThreshold = inThreshold;
	}
	
	
	
inline void ThresholdFilter::apply( double *inChannel, 
	int inWidth, int inHeight ) {

	int numPixels = inWidth * inHeight;
	for( int i=0; i<numPixels; i++ ) {
		if( inChannel[i] >= mThreshold ) {
			inChannel[i] = 1.0;
			}
		else {
			inChannel[i] = 0.0;
			}
		}
	}
	
#endif
