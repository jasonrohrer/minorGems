/*
 * Modification History
 *
 * 2001-April-29   Jason Rohrer
 * Created.
 *
 * 2001-May-2   Jason Rohrer
 * Added a nop move to the interface.
 *
 * 2001-May-8   Jason Rohrer
 * Changed to use decimeters as translation units.
 *
 * 2001-May-13   Jason Rohrer
 * Added a command for flashing a light.
 */

#ifndef MOVE_PROCESSOR_INCLUDED
#define MOVE_PROCESSOR_INCLUDED


/**
 * Abstract interface for a class that can execute robot moves.
 *
 * @author Jason Rohrer
 */ 
class MoveProcessor {
	
	public:

		/**
		 * Executes a nop move.
		 */
		virtual void nop() = 0;

		
		/**
		 * Executes a translation move.
		 *
		 * Note that certain implementations may perform obstacle
		 * avoidance to complete a translation of the entire distance.
		 * Thus, the end position may be to the "left" or "right" of
		 * the position that might have been obtained in an obstacle-free
		 * situation.
		 *
		 * @param inDistanceInDecimeters the distance to translate
		 *   in decimeters.  If this value is positive, the translation
		 *   is forward.  If this value is negative, the translation
		 *   is backward.
		 */
		virtual void translateForward( double inDistanceInCentimeters ) = 0;

		
		/**
		 * Executes a rotation move.
		 *
		 * @param inRotationFracton the fraction of a rotation to complete,
		 *   in [-1.0, +1.0].  If the fraction is positive, then the rotation
		 *   is clockwise, and the rotation is counter-clockwise otherwise.
		 */   
		virtual void rotateClockwise( double inRotationFraction ) = 0;

		
		/**
		 * Executes a light flashing sequence.
		 *
		 * @param inNumFlashes the number of light flashes to produce.
		 *   Note that the length of the flashes and the maximum
		 *   number of supported flashes is implementation
		 *   dependent.
		 */   
		virtual void flashLight( int inNumFlashes ) = 0;

		
	};


	
#endif
