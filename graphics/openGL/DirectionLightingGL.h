/*
 * Modification History
 *
 * 2000-December-20		Jason Rohrer
 * Created. 
 */
 
 
#ifndef DIRECTION_LIGHTING_GL_INCLUDED
#define DIRECTION_LIGHTING_GL_INCLUDED
 
#include <stdio.h>
 
#include "LightingGL.h"
 
/**
 * A LightingGL implementation with a single point light source
 * at distance infinity.
 *
 * @author Jason Rohrer 
 */
class DirectionLightingGL : public LightingGL { 
	
	public:
		
		/**
		 * Constructs a DirectionLighting.
		 *
		 * @param inColor color of lighting for a surface
		 *   that is facing directly into the light source.
		 *   Is not copied, so cannot be accessed again by caller.
		 * @param inDirection incoming direction of light source.
		 *   Is not copied, so cannot be accessed again by caller.
		 */
		DirectionLightingGL( Color *inColor, Vector3D *inDirection );
		
		
		~DirectionLightingGL();
		
		
		// implements LightingGL interface
		void getLighting( Vector3D *inPoint, Vector3D *inNormal,
			Color *outColor );
	
	
	private:
		Color *mColor;
		// direction pointing *towards* light source
		Vector3D *mDirection;
		
	};



inline DirectionLightingGL::DirectionLightingGL( Color *inColor,
	Vector3D *inDirection )
	: mColor( inColor ), mDirection( inDirection ) {

	// reverse the passed-in direction
	mDirection->scale( -1 );
	}



inline DirectionLightingGL::~DirectionLightingGL() {

	delete mColor;
	delete mDirection;
	}



inline void DirectionLightingGL::getLighting( Vector3D *inPoint, 
	Vector3D *inNormal, Color *outColor ) {
	
	double dot = inNormal->dot( mDirection );
	
	if( dot > 0 ) {
		outColor->r = mColor->r * dot;
		outColor->g = mColor->g * dot;
		outColor->b = mColor->b * dot;
		}
	else {
		outColor->r = 0;
		outColor->g = 0;
		outColor->b = 0;
		}
	}


	
#endif
