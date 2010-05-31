/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created. 
 *
 * 2001-January-10		Jason Rohrer
 * Made class serializable.
 * Added a mMembersAllocated flag and made parameterless constructor
 * public to help with deserialization.  
 *
 * 2001-January-15		Jason Rohrer
 * Fixed a bug in the deserialize() function.
 *
 * 2001-January-16		Jason Rohrer
 * Fixed a bug in the anchor serialization code.  
 *
 * 2001-January-19		Jason Rohrer
 * Changed to support multi-texturing.
 *
 * 2001-January-24		Jason Rohrer
 * Added a copy() function. 
 * Fixed a bug in deserialize(). 
 * Made mMembersAllocated public for copy function.
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
 * 2001-April-1  Jason Rohrer
 * Made copy function virtual.  Added a getNewInstance function
 * to make derived-class-specific copying easier.
 */
 
 
#ifndef PRIMITIVE_3D_INCLUDED
#define PRIMITIVE_3D_INCLUDED 

#include <string.h>

#include "minorGems/graphics/RGBAImage.h"
#include "minorGems/math/geometry/Angle3D.h" 

#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/math/geometry/Angle3D.h" 

#include "minorGems/io/Serializable.h"
 
/**
 * 3D primitive object.
 *
 * Comprised of a triangle mesh, texture map, and anchor points.
 *
 * @author Jason Rohrer 
 */
class Primitive3D : public Serializable { 
	
	public:
		
		
		/**
		 * Constructs a Primitive.
		 *
		 * No parameters are copied, so they should not be destroyed
		 * or re-accessed by caller.  All are destroyed when the 
		 * primitive is destroyed.
		 *
		 * @param inWide width of mesh in number of vertices.
		 *   Must be even and at least 2.
		 * @param inHigh height of mesh in number of vertices.
		 * @param inVertices vertices in row-major order.
		 * @param inNumTextures number of multi-texture layers for 
		 *   this primitive.
		 * @param inTexture array of textures to map onto mesh.
		 *   Note that no RGBAImage in this array should have a 
		 *   selection in it.
		 * @param inAnchorX x texture anchors for each texture and 
		 *   each vertex (indexed as inAnchorX[textNum][vertNum]), 
		 *   in range [0,1] (outside this range, texture will wrap).
		 * @param inAnchorY y texture anchors for each texture and 
		 *   each vertex (indexed as inAnchorY[textNum][vertNum]), 
		 *   in range [0,1] (outside this range, texture will wrap).
		 */
		Primitive3D( long inWide, long inHigh, Vector3D **inVertices,
			long inNumTextures, RGBAImage **inTexture, 
			double **inAnchorX, double **inAnchorY );
		
		
		// construct without initializing any members
		// for use by subclasses and for deserialization.
		Primitive3D();
		char mMembersAllocated;
		
		
		~Primitive3D();
		
		
		/**
		 * Sets whether this primitive is transparent or not.  Default
		 * is not transparent.
		 *
		 * @param inTransparent true if this primitive is transparent.
		 */
		void setTransparent( char inTransparent );
		
		
		/**
		 * Gets whether this primitive is transparent or not.
		 *
		 * @return true iff this primitive is transparent.
		 */
		char isTransparent();
		
		
		/**
		 * Sets whether this primitive's back face is visible.  Default
		 * is not visible.
		 *
		 * @param inIsVisible true if the back face is visible.
		 */
		void setBackVisible( char inIsVisible );
		
		
		/**
		 * Gets whether this primitive's back face is visible or not.
		 *
		 * @return true iff the back face is visible.
		 */
		char isBackVisible();
		

		/**
		 * Gets a new instance of the derived class type.
		 *
		 * Should be equivalent to calling the default constructor
		 * for the derived class type.
		 *
		 * Should be overridden by all derived classes that
		 * have data members beyond those provided by Primitive3D.
		 * Note that if the extra data members require copying,
		 * then the copy() function should be overridden instead
		 * of simply overriding this function.
		 *
		 * Primitive3D::copy() will use this function to produce
		 * a class instance before doing a copy.
		 *
		 * Note that this functionality SHOULD be provided by
		 * the built-in RTTI, but it doesn't seem to be.
		 *
		 * @return an instance of the derived class.
		 */
		virtual Primitive3D *getNewInstance();
		 
		
		/**
		 * Copies this primitive.
		 *
		 * @return a copy of this primitive, which must be destroyed
		 *   by the caller.
		 */
		virtual Primitive3D *copy();

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
		 * Sets a parameter for this primative.
		 *
		 * @param inParameterIndex the index of the parameter to set.
		 *   If an index for a non-existing parameter is specified,
		 *   this call has no effect.
		 * @param inValue the value to set the parameter to, in [-1, 1].
		 *   The default value for all parameters is 0.
		 */
		virtual void setParameter( int inParameterIndex, double inValue ); 


		/**
		 * Gets a parameter for this primative.
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
		 * Steps this primitive forward in time.
		 *
		 * @param inStepSize the size of the timestep to take.
		 */
		virtual void step( double inStepSize );


		/**
		 * Starts a temporal animation of this primitive.
		 * The animation progresses as step() is called repeatedly.
		 * If called for a non-existent animation or for one that is
		 * already running, this function has no effect.
		 */
		virtual void startAnimation( int inAnimationIndex );

		/**
		 * Stops a temporal animation of this primitive.  If called
		 * for a non-existent animation or for one that is not currently
		 * running, this function has no effect.
		 */
		virtual void stopAnimation( int inAnimationIndex );

		
		long mHigh, mWide;
		long mNumVertices;
		
		Vector3D **mVertices;
		Vector3D **mNormals;
		
		long mNumTextures;
		
		// Note that no RGBAImage in this array should have a 
		// selection in it.
		RGBAImage **mTexture; 
		
		double **mAnchorX; 
		double **mAnchorY;
		
		
		// implement the Serializable interface
		virtual int serialize( OutputStream *inOutputStream );
		virtual int deserialize( InputStream *inInputStream );
		
	protected:
		
		
		/**
		 * Generates standard normals from the vertices.
		 *
		 * If subclass is not generating normals, this 
		 * must be called before primitive is drawn, but after
		 * the vertices have been initialized.
		 */
		void generateNormals();
		
		
		char mTransparent;
		char mBackVisible;
		
		
		
	};



inline Primitive3D::Primitive3D(  long inWide, long inHigh, 
	Vector3D **inVertices, long inNumTextures, RGBAImage **inTexture, 
	double **inAnchorX, double **inAnchorY ) 
	: mHigh( inHigh ), mWide( inWide ), mNumVertices( inHigh * inWide ),
	mNumTextures( inNumTextures ), mVertices( inVertices ),
	mTexture( inTexture ), mAnchorX( inAnchorX ), mAnchorY( inAnchorY ),
	mTransparent( false ), mBackVisible( false ),
	mMembersAllocated( true ) {
	
	generateNormals();	
	}



inline Primitive3D::Primitive3D()
	: mTransparent( false ), mBackVisible( false ),
	mMembersAllocated( false ) {
	
	}



inline Primitive3D::~Primitive3D() {
	if( mMembersAllocated ) {
		int i;
		for( i=0; i<mNumVertices; i++ ) {
			delete mVertices[i];
			delete mNormals[i];
			}
		for( i=0; i<mNumTextures; i++ ) {
			delete [] mAnchorX[i];
			delete [] mAnchorY[i];
			delete mTexture[i];
			}	
		delete [] mVertices;
		delete [] mNormals;
		delete [] mAnchorX;
		delete [] mAnchorY;	

		delete [] mTexture;
		}
	}



inline void Primitive3D::generateNormals() {
	
	mNormals = new Vector3D*[ mNumVertices ];
	
	// for each non-edge vertex
	for( int y=0; y<mHigh; y++ ) {
		for( int x=0; x<mWide; x++ ) {
			int index = y * mWide + x;
			
			// each point is adjacent to up to 4 other points, and thus
			// is part of up to 4 edges (or 8 edge pairs)
			
			// if we sum the normals generated by taking the cross of 
			// each edge pair, we'll have a good approximation of the
			// normal at this point.
			Vector3D *normalSum = new Vector3D( 0.0, 0.0, 0.0 );
			
			// is there a more elegant way to do this?
			
			Vector3D **edges = new Vector3D*[4];
			
			// watch for boundary cases.
			// use NULL for any non-existing edge (i.e., at edge of mesh)
			
			if( x != 0 ) {
				edges[0] = new Vector3D( mVertices[ index - 1 ] ); // x - 1
				}
			else {
				edges[0] = NULL;
				}
			
			if( y != 0 ) {	
				edges[1] = new Vector3D( mVertices[ index - mWide ] ); // y - 1
				}
			else {
				edges[1] = NULL;
				}
			
			if( x != mWide - 1 ) {	
				edges[2] = new Vector3D( mVertices[ index + 1 ] ); // x + 1
				}
			else {
				edges[2] = NULL;
				}
			
			if( y != mHigh - 1 ) {
				edges[3] = new Vector3D( mVertices[ index + mWide ] ); // y + 1
				}
			else {
				edges[3] = NULL;
				}
			
			int e;
			// subtract this vertex from each to get a directional vector
			for( e=0; e<4; e++ ) {
				if( edges[e] != NULL ) { 
					edges[e]->subtract( mVertices[ index ] );
					}
				}
			
			// now cross and add into sum
			for( e=0; e<4; e++ ) {
				if( edges[e] != NULL && edges[ (e+1) % 4 ] != NULL ) {
					// not that this order of crossing works
					// because our cross product is right handed
					Vector3D *normal = edges[e]->cross( edges[ (e+1) % 4 ] );
					normal->normalize();

					// add this normal to our sum
					normalSum->add( normal );

					delete normal;
					}
				}
			
			// now delete edges
			for( e=0; e<4; e++ ) {
				if( edges[e] != NULL ) { 
					delete edges[e];
					}
				}
			delete [] edges;
			
			// save summed normal as normal for this point
			normalSum->normalize();
			mNormals[index] = normalSum;	
			}
		}
	}



inline void Primitive3D::setTransparent( char inTransparent ) {
	mTransparent = inTransparent;
	}



inline char Primitive3D::isTransparent() {
	return mTransparent;
	}



inline void Primitive3D::setBackVisible( char inIsVisible ) {
	mBackVisible = inIsVisible;
	}



inline char Primitive3D::isBackVisible() {
	return mBackVisible;
	}	



inline Primitive3D *Primitive3D::getNewInstance() {
	return new Primitive3D();
	}



inline Primitive3D *Primitive3D::copy() {
	// get an instance of the derived class, if they've
	// overridden getNewInstance()
	Primitive3D *primCopy = getNewInstance();
	
	primCopy->mHigh = mHigh;
	primCopy->mWide = mWide;
	primCopy->mNumVertices = mNumVertices;
	
	primCopy->mVertices = new Vector3D*[mNumVertices];
	primCopy->mNormals = new Vector3D*[mNumVertices];
	
	int i;
	for( i=0; i<mNumVertices; i++ ) {
		primCopy->mVertices[i] = new Vector3D( mVertices[i] );
		primCopy->mNormals[i] = new Vector3D( mNormals[i] );
		}
	primCopy->mNumTextures = mNumTextures;
	
	primCopy->mTexture = new RGBAImage*[mNumTextures];
	primCopy->mAnchorX = new double*[mNumTextures];
	primCopy->mAnchorY = new double*[mNumTextures];
	
	for( i=0; i<mNumTextures; i++ ) {
		primCopy->mTexture[i] = mTexture[i]->copy();
		
		primCopy->mAnchorX[i] = new double[mNumVertices];
		primCopy->mAnchorY[i] = new double[mNumVertices];
		
		memcpy( primCopy->mAnchorX[i], mAnchorX[i], 
			sizeof( double ) * mNumVertices );
		memcpy( primCopy->mAnchorY[i], mAnchorY[i], 
			sizeof( double ) * mNumVertices );
		}
	primCopy->mMembersAllocated	= true;
	
	primCopy->setBackVisible( isBackVisible() );
	primCopy->setTransparent( isTransparent() );
	
	return primCopy;
	}



inline int Primitive3D::getNumParameters() {
	return 0;
	}



inline int Primitive3D::getNumAnimations() {
	return 0;
	}



// the base class versions of these functions do nothing

inline void Primitive3D::setParameter( int inParameterIndex, double inValue ) {

	}



inline double Primitive3D::getParameter( int inParameterIndex ) {

	return 0;
	}



inline void Primitive3D::step( double inStepSize ) {

	}



inline void Primitive3D::startAnimation( int inAnimationIndex ) {

	}



inline void Primitive3D::stopAnimation( int inAnimationIndex ) {

	}



inline int Primitive3D::serialize( OutputStream *inOutputStream ) {
	int numBytes = 0;
	
	numBytes += inOutputStream->writeLong( mWide );
	numBytes += inOutputStream->writeLong( mHigh );
	
	int i;
	
	// output vertices and normals
	for( i=0; i<mNumVertices; i++ ) {
		numBytes += mVertices[i]->serialize( inOutputStream );
		}
	
	for( i=0; i<mNumVertices; i++ ) {
		numBytes += mNormals[i]->serialize( inOutputStream );
		}

	numBytes += inOutputStream->writeLong( mNumTextures );
	
	// output textures	
	for( i=0; i<mNumTextures; i++ ) {
		numBytes += mTexture[i]->serialize( inOutputStream );
		}
	
	// output anchor arrays
	for( i=0; i<mNumTextures; i++ ) {
		for( int p=0; p<mNumVertices; p++ ) {
			numBytes += inOutputStream->writeDouble( mAnchorX[i][p] );
			}
		}
	
	for( i=0; i<mNumTextures; i++ ) {
		for( int p=0; p<mNumVertices; p++ ) {
			numBytes += inOutputStream->writeDouble( mAnchorY[i][p] );
			}
		}
	
	numBytes += 
		inOutputStream->write( (unsigned char *)&mTransparent, 1 );
	
	numBytes += 
		inOutputStream->write( (unsigned char *)&mBackVisible, 1 );

	return numBytes;
	}

	
	
inline int Primitive3D::deserialize( InputStream *inInputStream ) {
	int numBytes = 0;
	
	int i;
	if( mMembersAllocated ) {
		// delete the old vertices, normals, and anchors
		for( i=0; i<mNumVertices; i++ ) {
			delete mVertices[i];
			delete mNormals[i];
			}
		for( i=0; i<mNumTextures; i++ ) {
			delete [] mAnchorX[i];
			delete [] mAnchorY[i];
			delete mTexture[i];
			}	
		delete [] mVertices;
		delete [] mNormals;
		delete [] mAnchorX;
		delete [] mAnchorY;	

		delete [] mTexture;
		}
	
	
	int numBytesRead = 0;
	
	unsigned char *intByteArray = new unsigned char[4];
	
	numBytes += inInputStream->readLong( &mWide );
	numBytes += inInputStream->readLong( &mHigh );
	
	mNumVertices = mHigh * mWide;
	
	mVertices = new Vector3D*[mNumVertices];
	mNormals = new Vector3D*[mNumVertices];
	
	
	// input vertices and normals
	for( i=0; i<mNumVertices; i++ ) {
		mVertices[i] = new Vector3D( 0, 0, 0 );
		numBytes += mVertices[i]->deserialize( inInputStream );
		}
	
	for( i=0; i<mNumVertices; i++ ) {
		mNormals[i] = new Vector3D( 0, 0, 0 );
		numBytes += mNormals[i]->deserialize( inInputStream );
		}

	// input number of textures
	numBytes += inInputStream->readLong( &mNumTextures );
	
	mAnchorX = new double*[mNumTextures];
	mAnchorY = new double*[mNumTextures];
	mTexture = new RGBAImage*[mNumTextures];
	
	
	// input textures	
	for( i=0; i<mNumTextures; i++ ) {
		mTexture[i] = new RGBAImage( 0, 0 );
		numBytes += mTexture[i]->deserialize( inInputStream );
		}
	
	// input anchor arrays
	for( i=0; i<mNumTextures; i++ ) {
		mAnchorX[i] = new double[mNumVertices];
		
		for( int p=0; p<mNumVertices; p++ ) {
			numBytes += inInputStream->readDouble( &( mAnchorX[i][p] ) );
			}
		}
	
	for( i=0; i<mNumTextures; i++ ) {
		mAnchorY[i] = new double[mNumVertices];
		
		for( int p=0; p<mNumVertices; p++ ) {
			numBytes += inInputStream->readDouble( &( mAnchorY[i][p] ) );
			}
		}
	
	numBytes += 
		inInputStream->read( (unsigned char *)&mTransparent, 1 );
	
	numBytes += 
		inInputStream->read( (unsigned char *)&mBackVisible, 1 );
	
	mMembersAllocated = true;
	
	return numBytesRead;
	}
	
	
	
#endif
