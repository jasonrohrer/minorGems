/*
 * Modification History
 *
 * 2001-February-13		Jason Rohrer
 * Created.
 *
 * 2001-February-16		Jason Rohrer
 * Figured out how to compile so that it will link to susan.c.
 * Still haven't tested it. 
 * Implemented the copy() function that was added to the EdgeDetector interface.  
 */
 
 
#ifndef SUSAN_EDGE_DETECTOR_INCLUDED
#define SUSAN_EDGE_DETECTOR_INCLUDED 


#include "susan_types.h"

#include "EdgeDetector.h"


// the following prototypes must be defined here
// to import the C susan functions
extern "C" void susan_find_edges_thresh( uchar *in, 
	int x_size, int y_size, uchar *binEdges, EDGE_ORIENTATION *orient, 
	int thresh );

extern "C" void susan_find_edges( uchar *in, 
	int x_size, int y_size, uchar *binEdges, EDGE_ORIENTATION *orient );



/**
 * Edge detector implementation that uses the SUSAN framework.
 *
 *
 * Note that this class depends on functions implemented in the
 * file "susan.c". 
 *
 * To compile: first compile the susan.c file with the following command
 *
 * gcc -c -o susan.o susan.c
 *
 * which will compile susan.c without linking it to anything, and create 
 * an object file called susan.o.
 *
 * Then, to compile your project file (myfile.cpp) that includes
 * SusanEdgeDetector.h, use the following command:
 *
 * g++ -o myProject myfile.cpp susan.o 
 *
 * The susan.o object file will be linked into your project.
 *
 * @author Jason Rohrer
 */
class SusanEdgeDetector : public EdgeDetector {
	
	public:
		
		/**
		 * Constructs a Susan egde detector.
		 *
		 * @param inThreshold the edge detection threshold.  Higher values
		 *   mean fewer edges are detected.
		 */
		SusanEdgeDetector( int inThreshold );
		
		/**
		 * Sets the edge-detection threshold.
		 *
		 * @param inThreshold the edge detection threshold.  Higher values
		 *   mean fewer edges are detected.
		 */
		void setThreshold( int inThreshold );
		
		
		/**
		 * Gets the edge-detection threshold.
		 *
		 * @return the edge detection threshold.  Higher values
		 *   mean fewer edges are detected.
		 */
		int getThreshold();
		
		
		
		// implements the EdgeDetector interface
		virtual Image *findEdges( Image *inImage );
		virtual EdgeDetector *copy();
		
	private:
		int mThreshold;
	
	
	};



inline SusanEdgeDetector::SusanEdgeDetector( int inThreshold )
	: mThreshold( inThreshold ) {
	
	}



inline void SusanEdgeDetector::setThreshold( int inThreshold ) {
	mThreshold = inThreshold;
	}



inline int SusanEdgeDetector::getThreshold() {
	return mThreshold;
	}
	
	
	
inline Image *SusanEdgeDetector::findEdges( Image *inImage ) {
	int channelNumber;
	if( mChannelNumber >= inImage->getNumChannels() ) {
		// channel out of range.
		channelNumber = 0;
		}
	else {
		channelNumber = mChannelNumber;
		}
	
	
	long wide = inImage->getWidth();
	long high = inImage->getHeight();
	
	long numPixels = wide * high;
	
	double *doubleChannel = inImage->getChannel( channelNumber );
	
	uchar *charChannel = new uchar[ numPixels ];
	uchar *charEdges = new uchar[ numPixels ];
	
	EDGE_ORIENTATION *orientations = new EDGE_ORIENTATION[ numPixels ];
	
	int i;
	// copy double channel into chars
	for( i=0; i<numPixels; i++ ) {
		charChannel[i] = (uchar)( 255 * doubleChannel[i] );
		}
	
	susan_find_edges_thresh( charChannel, 
		wide, high, charEdges, orientations, 
		mThreshold );
	
	Image *edges = new Image( wide, high, 1 );
	double *doubleEdges = edges->getChannel( channelNumber );	
	
	for( i=0; i<numPixels; i++ ) {
		// edges produced are in binary, so we can convert directly 
		// to doubles
		doubleEdges[i] = (double)( charEdges[i] );
		}
	
	delete [] charChannel;
	delete [] charEdges;
	delete [] orientations;
	
	
	return edges;	
	}
	


inline EdgeDetector *SusanEdgeDetector::copy() {
	return new SusanEdgeDetector( mThreshold );
	}
	

	
#endif
