/*
 * Modification History
 *
 * 2000-December-20		Jason Rohrer
 * Created.
 *
 * 2001-January-6		Jason Rohrer
 * Added some missing includes.
 *
 * 2001-January-16		Jason Rohrer
 * Changed to use a Transform3D instead of Vectors, Angles, and scales for
 * each primitive in the object.  Changed draw() function to take a Transform3D. 
 * Fixed a memory leak in the draw() function.
 *
 * 2001-March-14   Jason Rohrer
 * Added support for paramatization and temporal animations.
 */
 
 
#ifndef OBJECT_GL_INCLUDED
#define OBJECT_GL_INCLUDED

#include <GL/gl.h>

#include "PrimitiveGL.h"
#include "LightingGL.h"

#include "minorGems/math/geometry/Transform3D.h"  

#include "minorGems/graphics/3d/Object3D.h"
 
/**
 * OpenGL object.
 *
 * Comprised of a collection of primitives.
 *
 * @author Jason Rohrer 
 */
class ObjectGL { 
	
	public:
		
		
		/**
		 * Constructs an Object.
		 *
		 * No parameters are copied, so they should not be destroyed
		 * or re-accessed by caller.  All are destroyed when the 
		 * object is destroyed.
		 *
		 * @param inObject the Object3D to base this GL version on.
		 */
		ObjectGL( Object3D *inObject );
		
		
		~ObjectGL();
		
		
		/**
		 * Draws the object.
		 *
		 * @param inTransform the transform to apply to the object before
		 *   drawing it.
		 * @param inLighting the lighting to use when drawing the object. 
		 */
		void draw( Transform3D *inTransform, LightingGL *inLighting ); 

		
		/**
		 * Gets the number of parameters associated with this object.
		 *
		 * @return the number of parameters.
		 */
		virtual int getNumParameters();

		
		/**
		 * Gets the number of animations associated with this object.
		 *
		 * @return the number of animations.
		 */
		virtual int getNumAnimations();

		
		/**
		 * Sets a parameter for this object.
		 *
		 * @param inParameterIndex the index of the parameter to set.
		 *   If an index for a non-existing parameter is specified,
		 *   this call has no effect.
		 * @param inValue the value to set the parameter to, in [-1, 1].
		 *   The default value for all parameters is 0.
		 */
		virtual void setParameter( int inParameterIndex, double inValue ); 


		/**
		 * Gets a parameter for this object.
		 *
		 * @param inParameterIndex the index of the parameter to get.
		 *   If an index for a non-existing parameter is specified,
		 *   0 is returned.
		 *
		 * @return the value of the parameter, in [-1, 1].
		 *   The default value for all parameters is 0.
		 */
		virtual double getParameter( int inParameterIndex );
		
		
		/**
		 * Steps this object forward in time.
		 *
		 * @param inStepSize the size of the timestep to take.
		 */
		virtual void step( double inStepSize );


		/**
		 * Starts a temporal animation of this object.
		 * The animation progresses as step() is called repeatedly.
		 * If called for a non-existent animation or for one that is
		 * already running, this function has no effect.
		 */
		virtual void startAnimation( int inAnimationIndex );

		
		/**
		 * Stops a temporal animation of this object.  If called
		 * for a non-existent animation or for one that is not currently
		 * running, this function has no effect.
		 */
		virtual void stopAnimation( int inAnimationIndex );
		
	private:
		
		Object3D *mObject;
		
		// GL versions of the primitives
		PrimitiveGL **mPrimitives;
		
	};



inline ObjectGL::ObjectGL( Object3D *inObject )
	: mObject( inObject ) {
	
	mPrimitives = new PrimitiveGL*[ mObject->mNumPrimitives ];
	
	for( int i=0; i<mObject->mNumPrimitives; i++ ) {
		mPrimitives[i] = new PrimitiveGL( mObject->mPrimitives[i] );
		}	
	}



inline ObjectGL::~ObjectGL() {
	for( int i=0; i<mObject->mNumPrimitives; i++ ) {
		delete mPrimitives[i];
		}
	delete [] mPrimitives;

	delete mObject;
	}



inline void ObjectGL::draw( Transform3D *inTransform, LightingGL *inLighting  ) {
		
	for( int i=0; i<mObject->mNumPrimitives; i++ ) { 
		
		// need to transform the position of primitive within object
		// copy the transform for this primitive
		Transform3D *primTransform = 
			new Transform3D( mObject->mTransform[i] );
		
		// add the object transform to the end of it
		primTransform->transform( inTransform );
		
		mPrimitives[i]->draw( primTransform, inLighting );
		
		delete primTransform;
		}
			
	}


// these just pass the function call on to the underlying Object3D

inline int ObjectGL::getNumParameters() {
	return mObject->getNumParameters();
	}



inline int ObjectGL::getNumAnimations() {
	return mObject->getNumAnimations();
	}



inline void ObjectGL::setParameter( int inParameterIndex, double inValue ) {
	mObject->setParameter( inParameterIndex, inValue ); 
	}



inline double ObjectGL::getParameter( int inParameterIndex ) {

	return mObject->getParameter( inParameterIndex ); 
	}



inline void ObjectGL::step( double inStepSize ) {
	mObject->step( inStepSize );
	}



inline void ObjectGL::startAnimation( int inAnimationIndex ) {
	mObject->startAnimation( inAnimationIndex );
	}



inline void ObjectGL::stopAnimation( int inAnimationIndex ) {
	mObject->stopAnimation( inAnimationIndex );
	}



#endif
