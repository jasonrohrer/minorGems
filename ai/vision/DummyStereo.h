/*
 * Modification History
 *
 * 2001-May-2   Jason Rohrer
 * Created.     
 */
 
 
#ifndef DUMMY_STEREO_INCLUDED
#define DUMMY_STEREO_INCLUDED 

#include "Stereo.h"



/**
 * Stereo implementation that does no real processing.
 *
 * @author Jason Rohrer
 */
class DummyStereo : public Stereo {
	
	public:

		/**
		 * Constructs a new DummyStereo.
		 */
		DummyStereo();
		
		// implements the stereo interface
		virtual Image *computeDepthMap( Image *inLeft, Image *inRight );
		
		// implementation provided, but actually invoking copy()
		// does not make sense for this class.
		// Currently returns a new DummyStereo instance.
		virtual Stereo *copy();
	
	};



// pass a dummy valu into the superclass constructor
inline DummyStereo::DummyStereo()
	: Stereo( 5 ) {

	}



inline Stereo *DummyStereo::copy() {

	return new DummyStereo();
	}



inline Image *DummyStereo::computeDepthMap( 
	Image *inLeft, Image *inRight ) {
	
	return new Image( inLeft->getWidth(), inLeft->getHeight(), 1 );
	}
	
		
		
#endif


