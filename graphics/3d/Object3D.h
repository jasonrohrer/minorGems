/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created.  Copied from ObjectGL.
 *
 * 2001-January-10		Jason Rohrer
 * Made class serializable.  Added a parameterless constructor
 * to facilitate deserialization.  
 *
 * 2001-January-15		Jason Rohrer
 * Fixed several bugs in the deserialize() function, as well as in the
 * destructor.  
 *
 * 2001-January-16		Jason Rohrer
 * Changed to use a Transform3D instead of Vectors, Angles, and scales for
 * each primitive in the object.
 *
 * 2001-January-24		Jason Rohrer
 * Added a copy() function. 
 * Fixed a bug in deserialize(). 
 * Made mMembersAllocated public for copy function.  
 *
 * 2001-January-26		Jason Rohrer
 * Fixed a bug in copy().
 *
 * 2001-January-31		Jason Rohrer
 * Fixed bugs in serialize() and deserialize().
 *
 * 2001-February-3		Jason Rohrer
 * Updated serialization code to use new interfaces.
 *
 * 2001-March-11   Jason Rohrer
 * Added support for paramatization and temporal animations.
 *
 * 2001-March-13   Jason Rohrer
 * Added interface for getting the number of parameters and animations.
 *
 * 2001-March-14   Jason Rohrer
 * Added use of Primitive3DFactory for typed subclass primitive
 * de/serialization.
 */
 
 
#ifndef OBJECT_3D_INCLUDED
#define OBJECT_3D_INCLUDED

#include "Primitive3D.h" 
#include "minorGems/math/geometry/Transform3D.h"  

#include "minorGems/io/Serializable.h"

#include "Primitive3DFactory.h"

/**
 * 3D object.
 *
 * Comprised of a collection of primitives.
 *
 * @author Jason Rohrer 
 */
class Object3D : public Serializable { 
	
	public:
		
		
		/**
		 * Constructs an Object.
		 *
		 * No parameters are copied, so they should not be destroyed
		 * or re-accessed by caller.  All are destroyed when the 
		 * object is destroyed.
		 *
		 * @param inNumPrimitives the number of primitives in this object.
		 * @param inPrimitives the primitives comprising this object.
		 * @param inTransform a transform for each object.
		 */
		Object3D( long inNumPrimitives, Primitive3D **inPrimitives,
			Transform3D **inTransform );
		
		Object3D();
		
		~Object3D();
		
		
		/**
		 * Copies this object.
		 *
		 * @return a copy of this object, which must be destroyed
		 *   by the caller.
		 */
		Object3D *copy();


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

		
		/*
		 * Note that the default implementations for all the parameter
		 * and temporal animation functions do nothing.
		 */
		
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

		
		long mNumPrimitives;
		
		Primitive3D **mPrimitives;
		
		Transform3D **mTransform;
		
		// implement the Serializable interface
		virtual int serialize( OutputStream *inOutputStream );
		virtual int deserialize( InputStream *inInputStream );
	
		char mMembersAllocated;
		
	};



inline Object3D::Object3D( long inNumPrimitives, Primitive3D **inPrimitives,
	Transform3D **inTransform )
	: mNumPrimitives( inNumPrimitives ), mPrimitives( inPrimitives ),
	mTransform( inTransform ), mMembersAllocated( true ) {
		
	}



inline Object3D::Object3D()
	: mMembersAllocated( false ) {
		
	}



inline Object3D::~Object3D() {
	if( mMembersAllocated ) {
		for( int i=0; i<mNumPrimitives; i++ ) {
			delete mPrimitives[i];
			delete mTransform[i];
			}
		delete [] mTransform;
		}
	}



inline Object3D *Object3D::copy() {
	Object3D *objCopy = new Object3D();
	
	objCopy->mNumPrimitives = mNumPrimitives;
	
	objCopy->mPrimitives = new Primitive3D*[ mNumPrimitives ];
	objCopy->mTransform = new Transform3D*[ mNumPrimitives ];
	
	int i;
	for( i=0; i<mNumPrimitives; i++ ) {
		objCopy->mPrimitives[i] = mPrimitives[i]->copy();
		objCopy->mTransform[i] = new Transform3D( mTransform[i] );
		}
	
	objCopy->mMembersAllocated = true;
	
	return objCopy;	
	}



inline int Object3D::getNumParameters() {
	return 0;
	}



inline int Object3D::getNumAnimations() {
	return 0;
	}



// the base class versions of these functions do nothing

inline void Object3D::setParameter( int inParameterIndex, double inValue ) {

	}



inline double Object3D::getParameter( int inParameterIndex ) {

	return 0;
	}



inline void Object3D::step( double inStepSize ) {

	}



inline void Object3D::startAnimation( int inAnimationIndex ) {

	}



inline void Object3D::stopAnimation( int inAnimationIndex ) {

	}


	
inline int Object3D::serialize( OutputStream *inOutputStream ) {
	int i;
	int numBytes = 0;
	
	numBytes += inOutputStream->writeLong( mNumPrimitives );

	
	// write each primitive
	for( i=0; i<mNumPrimitives; i++ ) {
		// write a type flag for each primitive
		long typeFlag =
			Primitive3DFactory::primitive3DToInt( mPrimitives[i] ); 
		numBytes += inOutputStream->writeLong( typeFlag );
		
		// write the primitive
		numBytes += mPrimitives[i]->serialize( inOutputStream );
		}

	// write each primitive's transform
	for( i=0; i<mNumPrimitives; i++ ) {
		numBytes += mTransform[i]->serialize( inOutputStream );
		}
	
	return numBytes;
	}
	
	
	
inline int Object3D::deserialize( InputStream *inInputStream ) {
	
	if( mMembersAllocated ) {
		// first, delete current contents of object
		for( int i=0; i<mNumPrimitives; i++ ) {
			delete mPrimitives[i];
			delete mTransform[i];
			}
		delete [] mPrimitives;
		delete [] mTransform;
		}
	
	int i;
	int numBytes = 0;
	
	numBytes += inInputStream->readLong( &mNumPrimitives );
	
	printf( "receiving %d primitives\n", mNumPrimitives );
	mPrimitives = new Primitive3D*[mNumPrimitives];
	
	for( i=0; i<mNumPrimitives; i++ ) {
		// read the type flag for this primitive
		long typeFlag;
		numBytes += inInputStream->readLong( &typeFlag );
		
		// construct a new object based on the type flag
		mPrimitives[i] =
			Primitive3DFactory::intToPrimitive3D( typeFlag );

		// deserialize it
		numBytes += mPrimitives[i]->deserialize( inInputStream );
		}
	
	mTransform = new Transform3D*[mNumPrimitives];
	for( i=0; i<mNumPrimitives; i++ ) {
		mTransform[i] = new Transform3D();
		numBytes += mTransform[i]->deserialize( inInputStream );
		}
	
	mMembersAllocated = true;
	
	return numBytes;
	}


#endif
