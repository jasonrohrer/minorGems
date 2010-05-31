/*
 * Modification History
 *
 * 2000-December-20		Jason Rohrer
 * Created. 
 */
 
 
#ifndef LIGHTING_GL_INCLUDED
#define LIGHTING_GL_INCLUDED
 
#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/graphics/Color.h" 
 
/**
 * Interface for lighting in an OpenGL scene.
 *
 * @author Jason Rohrer 
 */
class LightingGL { 
	
	public:
		
		/**
		 * Gets the lighting color for a particular point on a surface
		 * with a particular orientation.
		 *
		 * @param inPoint the surface point.
		 * @param inNormal direction vector for normal at surface point.
		 *   Must be normalized.
		 * @param outColor preallocated Color where the lighting color
		 *   of this surface point will be returned.
		 */
		virtual void getLighting( Vector3D *inPoint, Vector3D *inNormal,
			Color *outColor ) = 0;
		
	};
	
#endif
