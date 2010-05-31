/*
 * Modification History
 *
 * 2000-December-21		Jason Rohrer
 * Created. 
 */
 
 
#ifndef INVERT_FILTER_INCLUDED
#define INVERT_FILTER_INCLUDED

#include "minorGems/graphics/ChannelFilter.h" 
 
/**
 * Filter that inverts the values in a channel.
 *
 * @author Jason Rohrer 
 */
class InvertFilter : public ChannelFilter { 
	
	public:
		
		// implements the ChannelFilter interface
		void apply( double *inChannel, int inWidth, int inHeight );
	};
	
	
	
inline void InvertFilter::apply( double *inChannel, 
	int inWidth, int inHeight ) {

	int numPixels = inWidth * inHeight;
	for( int i=0; i<numPixels; i++ ) {
		inChannel[i] = 1.0 - inChannel[i];
		}
	}
	
#endif
