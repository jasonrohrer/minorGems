/*
 * Modification History
 *
 * 2000-December-20		Jason Rohrer
 * Created. 
 */
 
 
#ifndef MULTI_LIGHTING_GL_INCLUDED
#define MULTI_LIGHTING_GL_INCLUDED
 
#include <stdio.h>
 
#include "LightingGL.h"
#include "minorGems/util/SimpleVector.h"
 
/**
 * A LightingGL implementation that contains a collection of
 * LightingGLs.
 *
 * @author Jason Rohrer 
 */
class MultiLightingGL : public LightingGL { 
	
	public:
		
		/**
		 * Constructs a MultiLighting.
		 */
		MultiLightingGL();
		
		
		~MultiLightingGL();
		
		
		/**
		 * Adds a lighting to this MultiLighting.
		 *
		 * @param inLighting lighting to add.  Is not
		 *   destroyed when the MultiLighting is destroyed.
		 */
		void addLighting( LightingGL *inLighting );
		
		
		/**
		 * Removes a lighting to this MultiLighting.
		 *
		 * @param inLighting lighting to remove.
		 */
		void removeLighting( LightingGL *inLighting );
		
		
		// implements LightingGL interface
		void getLighting( Vector3D *inPoint, Vector3D *inNormal,
			Color *outColor );
	
	
	private:
	
		SimpleVector<LightingGL*> *mLightingVector;
		
	};



inline MultiLightingGL::MultiLightingGL() 
	: mLightingVector( new SimpleVector<LightingGL*>() ) {

	}



inline MultiLightingGL::~MultiLightingGL() {
	delete mLightingVector;
	}



inline void MultiLightingGL::addLighting( LightingGL *inLighting ) {
	mLightingVector->push_back( inLighting );
	}
		
		

inline void MultiLightingGL::removeLighting( LightingGL *inLighting ) {
	mLightingVector->deleteElement( inLighting );
	}



inline void MultiLightingGL::getLighting( Vector3D *inPoint, 
	Vector3D *inNormal, Color *outColor ) {
	
	int numLightings = mLightingVector->size();
	
	outColor->r = 0;
	outColor->g = 0;
	outColor->b = 0;
	
	// sum lighting contributions from each lighting
	for( int i=0; i<numLightings; i++ ) {
		Color *tempColor = new Color( 0, 0, 0 );	
		
		LightingGL *thisLighting = *( mLightingVector->getElement( i ) );
		
		thisLighting->getLighting( inPoint, inNormal, tempColor );
		
		outColor->r += tempColor->r;
		outColor->g += tempColor->g;
		outColor->b += tempColor->b;
		
		delete tempColor;
		}
	
	
	// clip color components:
	if( outColor->r > 1.0 ) {
		outColor->r = 1.0;
		}
	if( outColor->g > 1.0 ) {
		outColor->g = 1.0;
		}
	if( outColor->b > 1.0 ) {
		outColor->b = 1.0;
		}		
	
	}


	
#endif
