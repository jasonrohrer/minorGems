/*
 * Modification History
 *
 * 2000-December-20		Jason Rohrer
 * Created. 
 */
 
 
#ifndef NO_LIGHTING_GL_INCLUDED
#define NO_LIGHTING_GL_INCLUDED
 
#include "LightingGL.h" 
 
/**
 * LightingGL implementation that uses a constant white light everywhere.
 *
 * @author Jason Rohrer 
 */
class NoLightingGL : public LightingGL{ 
	
	public:
		
		// implements LightingGL interface
		void getLighting( Vector3D *inPoint, Vector3D *inNormal,
			Color *outColor );
		
	};



inline void NoLightingGL::getLighting( Vector3D *inPoint, 
	Vector3D *inNormal, Color *outColor ) {
	
	outColor->r = 1.0;
	outColor->g = 1.0;
	outColor->b = 1.0;	
	}


	
#endif
