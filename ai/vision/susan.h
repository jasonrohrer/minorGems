/* Jason Rohrer  */
/* susan.h */

/**
*
*	Header describing interface of SUSAN C package.
*
*	SUSAN Version 2l by Stephen Smith
*
*
*	Created 5-16-2000
*	Mods:
*/

#ifndef SUSAN_FIND_EDGES_INCLUDED
#define SUSAN_FIND_EDGES_INCLUDED

#include "susan_types.h"


/* 	Find susan edges and orientations, provide a brightness threshold for USAN
 *	in			input grayscale image
 *  x_size		width of image
 *	y_size		height of image
 *	binEdges	output binary edges (noedge = 0, edgeHere = 1)
 *				allocated by caller
 *	orient		output edge orientations
 *				values only valid for i indexes where binEdges[i] == 1
 *				allocated by caller
 *	thresh		brightness threshold for USAN area computation
 *				( larger => more edges detected )
 *
 */
void susan_find_edges_thresh( uchar *in, int x_size, int y_size, uchar *binEdges, EDGE_ORIENTATION *orient, int thresh );




/* 	Find susan edges and orientations
 *	in			input grayscale image
 *  x_size		width of image
 *	y_size		height of image
 *	binEdges	output binary edges (noedge = 0, edgeHere = 1)
 *				allocated by caller
 *	orient		output edge orientations
 *				values only valid for i indexes where binEdges[i] == 1
 *				allocated by caller
 *	defaults to a brighness threshold of 20
 *
 */
void susan_find_edges( uchar *in, int x_size, int y_size, uchar *binEdges, EDGE_ORIENTATION *orient );



#endif