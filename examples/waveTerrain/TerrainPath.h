/*
 * Modification History
 *
 * 2001-April-20   Jason Rohrer
 * Created.  
 */
 
 
#ifndef TERRAIN_PATH_INCLUDED
#define TERRAIN_PATH_INCLUDED 


/**
 * Interface for wave terrain paths that map a time value to
 * a 2d coordinate.
 *
 * @author Jason Rohrer.
 */
class TerrainPath {
	public:


		/**
		 * Gets coordinate values along the path.
		 *
		 * @param inTime the time along the path to get values for,
		 *   in [0, DBL_MAX].
		 * @param outX pointer to where x coordinate will be returned.
		 *   Return value is in [0,1].
		 * @param outY pointer to where y coordinate will be returned.
		 *   Return value is in [0,1].
		 */
		virtual void getPosition( double inTime, double *outX,
						  double *outY ) = 0;

	};


#endif
