// Jason Rohrer
// edgesStereo.h

/**
*
*	Interface:  functions for computing stereo disparity using SUSAN edges
*
*
*	Created 5-16-2000
*	Mods:
*/

#ifndef EDGE_STEREO_INCLUDED
#define EDGE_STEREO_INCLUDED

#include "P_M.h"

/*
*	Computes optimal disparity for windows of edges using Hausdorff fraction.
*
*	Takes two input P_Ms (left and right),
*	one output P_M (disparity map),
*	an integer describing window size (diameter),
*	an integer describing max disparity,
*	an integer describing threshold for edge detection (higher => fewer edges detected)
*	and an integer describing dilation radius for computing Hausdorff fraction
*
*/
void edgeStereoHausdorff( P_M *inL, P_M *inR, P_M *out, int windowSize, int maxDisparity, int edgeThreshold, int hdDilate ); 


/*
*	Computes optimal disparity for pixels using variably sized windows.
*	Windows are rectangular and bounded by edges.
*	Edges in left image are a static cue for sizing the window.
*
*	Takes two input P_Ms (left and right),
*	one output P_M (disparity map),
*	an integer describing max disparity,
*	an integer describing threshold for edge detection (higher => fewer edges detected)
*
*/
char edgeStereoWindow( P_M *inL, P_M *inR, P_M *out, int maxDisparity, int edgeThreshold );


#endif