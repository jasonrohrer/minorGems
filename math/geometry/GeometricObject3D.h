/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2000-December-13		Jason Rohrer
 * Added scale() and copy() functions. 
 */
 
 
#ifndef GEOMETRIC_OBJECT_3D_INCLUDED
#define GEOMETRIC_OBJECT_3D_INCLUDED 

#include "Vector3D.h"
 
/**
 * Interface for any geometric objects in 3-space. 
 *
 * @author Jason Rohrer 
 */
class GeometricObject3D { 
	
	public:
		/**
		 * Moves the object.
		 *
		 * @param inVector a non-normalized vector describing the motion
		 *   of the object.
		 */
		virtual void move( Vector3D *inVector ) = 0;


		/**
		 * Rotates the object about the origin.
		 *
		 * @param inAngle the angle to rotate the object by.
		 */
		virtual void rotate( Angle3D *inAngle ) = 0;


		/**
		 * Rotates the object about the origin in reverse direction.
		 *
		 * @param inAngle the angle to rotate the object by 
		 *   in reverse direction.
		 */
		virtual void reverseRotate( Angle3D *inAngle ) = 0;


		/**
		 * Scales the object about the origin.
		 *
		 * @param inScalar value by which to scale.
		 */
		virtual void scale( double inScalar ) = 0;


		/**
		 * Makes an identical copy of this geometric object.
		 */
		virtual GeometricObject3D *copy() = 0;
	
	};
	
#endif
