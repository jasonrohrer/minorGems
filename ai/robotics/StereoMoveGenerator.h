/*
 * Modification History
 *
 * 2001-February-27		Jason Rohrer
 * Created.
 *
 * 2001-April-29   Jason Rohrer
 * Changed to use the MoveProcessor interface.
 *
 * 2001-May-7   Jason Rohrer
 * Changed comments to indicate that 3-channel images must be passed in.
 */
 
 
#ifndef STEREO_MOVE_GENERATOR_INCLUDED
#define STEREO_MOVE_GENERATOR_INCLUDED 

#include "MoveProcessor.h"

#include "minorGems/graphics/Image.h"

/**
 * Interface for a class that can take in stereo data
 * and generate moves.
 *
 * @author Jason Rohrer
 */ 
class StereoMoveGenerator {
	
	public:
		
		/**
		 * Generates moves from stereo data.
		 *
		 * All parameters must be destroyed by caller.
		 *
		 * @param inLeftImage the left source image.
		 *   Must contain three channels.
		 * @param inRightImage the right source image.
		 *   Must contain three channels.
		 * @param inStereoData the computed stereo map for the image pair.
		 * @param inMoveProcessor the processor that generated moves
		 *   will be passed to.
		 */
		virtual void generateMoves( 
			Image *inLeftImage, Image *inRightImage, 
			Image *inStereoData, MoveProcessor *inMoveProcessor ) = 0;	
	
	};
		
		
		
#endif
