/*
 * Modification History
 *
 * 2001-February-27		Jason Rohrer
 * Created. 
 * Raised region to avoid the floor.
 *
 * 2001-April-29   Jason Rohrer
 * Changed to implement modified StereoMoveGenerator interface.
 *
 * 2001-May-7   Jason Rohrer
 * Changed to convert incoming images to grayscale before processing.
 * Removed the color conversion code, since the images are not used anyway.
 */
 
 
#ifndef ROHRER_MOVE_GENERATOR_INCLUDED
#define ROHRER_MOVE_GENERATOR_INCLUDED 

#include "StereoMoveGenerator.h"
#include "MoveProcessor.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ImageColorConverter.h"
#include "ImageStatistics.h"


/**
 * Rohrer's implementation of the move generator.
 *
 * Turns until it sees a deep opening ahead, then goes forward
 * until the opening is no longer available.
 *
 * @author Jason Rohrer
 */ 
class RohrerMoveGenerator : public StereoMoveGenerator {
	
	public:
		
		/**
		 * Constructs a move generator.
		 *
		 * @param inDepthThreshold the threshold to use for
		 *   a "deep enough to move" decision.  Lower value
		 *   makes the move generator more selective (in other words,
		 *   it will only decide to move forward if the visual array
		 *   in front of it is very deep [far away]).  In [0,1]
		 * @param inMinDepthFraction the fraction of the pixels
		 *   in the center of the image that must be below inDepthThreshold
		 *   before the move generator will move forward.  Higher
		 *   values make the move generator more selective.  In [0,1].
		 */
		RohrerMoveGenerator( double inDepthThreshold, double 
			inMinDepthFraction );
		
		~RohrerMoveGenerator();
		
		// implements StereoMoveGenerator
		virtual void generateMoves( 
			Image *inLeftImage, Image *inRightImage, 
			Image *inStereoData, MoveProcessor *inMoveProcessor );	
	
	private:
		double mDepthThreshold;
		double mMinDepthFraction;
		
		ImageRectangle *mRectangle;
	};
	
	
	
inline RohrerMoveGenerator::RohrerMoveGenerator( double inDepthThreshold, 
	double inMinDepthFraction ) 
	: mDepthThreshold( inDepthThreshold ), 
	mMinDepthFraction(	inMinDepthFraction ),
	mRectangle( new ImageRectangle( 0.333, 0.666, 0.15, 0.5 ) ) {

	// rectangle takes the middle third of the image
	
	}



inline RohrerMoveGenerator::~RohrerMoveGenerator() {

	delete mRectangle;
	}	
	
		
		
inline void RohrerMoveGenerator::generateMoves( 
	Image *inLeftImage, Image *inRightImage, 
	Image *inStereoData, MoveProcessor *inMoveProcessor ) {

	
	double fraction = ImageStatistics::fractionUnderThreshold( 
		inStereoData, 0, mRectangle, mDepthThreshold );
	
	printf( "Fraction under threshold = %f, min fraction to go forward = %f\n",
		fraction, mMinDepthFraction );
	
	if( fraction >= mMinDepthFraction ) {
		printf( "moving forward\n" );
		// go forward by 1 meter
		inMoveProcessor->translateForward( 100.0 );
		}
	else {
		printf( "turning left to search for better direction\n" );
		// go rotate left by 45 degrees
		inMoveProcessor->rotateClockwise( -0.125 );
		}

	}	
	
	
		
#endif
