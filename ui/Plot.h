// Jason Rohrer
// Plot.h

/**
*
*	Scrolling Plot Gui element
*
*
*	Created 11-7-99
*	Mods:	
*		Jason Rohrer	11-8-99		Changed to use GraphicBuffer object as screen buffer
*
*/



#ifndef PLOT_INCLUDED
#define PLOT_INCLUDED


#include <string.h>
#include "Color.h"
#include "GraphicBuffer.h"

class Plot {

	public:
		// construct a plot in a specific location with
		//   specific border, background, and line colors
		Plot( int x, int y, int w, int h, Color bordC, Color bC, Color lnC );
		~Plot();
		
		// add a point to the right of plot, cause plot to scroll left 
		void addPoint( float p );
		
		// draw plot into a graphic buffer
		void draw( GraphicBuffer &buff );
		
		// erase plot from buffer
		void erase( GraphicBuffer &buff, Color &bgColor );
		
	private:
	
		unsigned long *imageMap;
	
		int startX;
		int startY;
		int high;
		int wide;
		
		int innerHigh;	// width and heigth of area inside borders
		int innerWide;

		int *mapYOffset;
	
		Color borderC;		// color of border
		Color bgC;		// color of progress bar max
		
		Color lineC;	// color of plot line
		
		
		
		static const int borderWide = 2;
	
		
		float *plotVals;		// values of plot line

	};




inline void Plot::draw( GraphicBuffer &buff ) {
	buff.drawImage(imageMap, mapYOffset, startX, startY, wide, high);
	}	

inline void Plot::erase( GraphicBuffer &buff, Color &bgColor ) {
	buff.eraseImage(startX, startY, wide, high, bgColor);	
	}



#endif