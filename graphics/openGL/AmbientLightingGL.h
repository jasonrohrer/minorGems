/*
 * Modification History
 *
 * 2001-January-17		Jason Rohrer
 * Created. 
 */
 
 
#ifndef AMBIENT_LIGHTING_GL_INCLUDED
#define AMBIENT_LIGHTING_GL_INCLUDED
 
#include "LightingGL.h" 
 
/**
 * LightingGL implementation that uses constant, specifiable 
 * light values everywhere.  Most useful when combined with
 * directional or point lightings using MultiLighting.
 *
 * Setting color to full white makes this lighting equivalent
 * to a NoLightingGL.
 *
 * @author Jason Rohrer 
 */
class AmbientLightingGL : public LightingGL{ 
	
	public:
		
		/**
		 * Constructs an AmbientLighting.
		 *
		 * @param inColor color and intensity of lighting.
		 *   Is not copied, so cannot be accessed again by caller.
		 */
		AmbientLightingGL( Color *inColor );
		

		~AmbientLightingGL();

		
		// implements LightingGL interface
		void getLighting( Vector3D *inPoint, Vector3D *inNormal,
			Color *outColor );
	
	private:
		Color *mColor;
		
	};


inline AmbientLightingGL::AmbientLightingGL( Color *inColor )
	: mColor( inColor ) {

	}



inline AmbientLightingGL::~AmbientLightingGL() {

	delete mColor;
	}



inline void AmbientLightingGL::getLighting( Vector3D *inPoint, 
	Vector3D *inNormal, Color *outColor ) {
	
	outColor->r = mColor->r;
	outColor->g = mColor->g;
	outColor->b = mColor->b;	
	}


	
#endif
