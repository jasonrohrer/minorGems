/*
 * Modification History
 *
 * 2001-February-27		Jason Rohrer
 * Created.
 *
 * 2001-April-29   Jason Rohrer
 * Changed to implement modified StereoMoveGenerator interface. 
 */
 
 
#ifndef DEFAULT_MOVE_GENERATOR_INCLUDED
#define DEFAULT_MOVE_GENERATOR_INCLUDED 

#include "StereoMoveGenerator.h"
#include "MoveProcessor.h"


#include "minorGems/graphics/Image.h"

/**
 * Default implementation, turns left and right
 *
 * @author Jason Rohrer
 */ 
class DefaultMoveGenerator : public StereoMoveGenerator {
	
	public:
		
		// implements StereoMoveGenerator
		virtual void generateMoves( 
			Image *inLeftImage, Image *inRightImage, 
			Image *inStereoData, MoveProcessor *inMoveProcessor );	
	};
	
	
		
inline void DefaultMoveGenerator::generateMoves( 
	Image *inLeftImage, Image *inRightImage, 
	Image *inStereoData, MoveProcessor *inMoveProcessor ) {
	
	inMoveProcessor->rotateClockwise( -0.125 );
	inMoveProcessor->rotateClockwise( 0.125 );
	}	
	
	
		
#endif

