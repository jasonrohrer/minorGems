// Jason Rohrer
// localStereo.h

/**
*
*	Interface:  Function for computing stereo disparity using local window
*	Uses L2 distance only
*
*
*	Created 5-17-2000
*	Mods:
*/

#ifndef LOCAL_STEREO_INCLUDED
#define LOCAL_STEREO_INCLUDED

#include "P_M.h"

void localStereo( P_M *inL, P_M *inR, P_M *out, int windowSize, int maxDisparity );


#endif