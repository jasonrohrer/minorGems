// Jason Rohrer
// ProgressBar.h

/**
*
*	ProgressBar Gui element
*
*
*	Created 11-6-99
*	Mods:	
*		Jason Rohrer	11-8-99		Changed to use GraphicBuffer object as screen buffer
*
*/



#ifndef PROGRESS_BAR_INCLUDED
#define PROGRESS_BAR_INCLUDED

#include "Color.h"
#include "GraphicBuffer.h"

class ProgressBar {
	
	public:
		// construct a progress bar in a specific location with
		//   specific border and maximum colors
		ProgressBar( int x, int y, int w, int h, Color bordC, Color hC, Color tipC );
		~ProgressBar();
	
		void setProgress( float fractionFull );
	
		// draw progress bar into a graphic buffer
		void draw( GraphicBuffer &buff );
		
		// erase progress bar from buffer
		void erase( GraphicBuffer &buff, Color &bgColor );
	
	private:

		unsigned long *imageMap;
	
		int startX;
		int startY;
		int high;
		int wide;
		
		int barHigh;	// width and heigth of area inside borders
		int barWide;

		int *mapYOffset;
	
		Color borderC;		// color of border
		Color highC;		// color of progress bar max
		
		Color barTipC;
		
		static const int borderWide = 2;
	
		float lastProgress;		// fraction of last progress
	
	};



inline void ProgressBar::draw( GraphicBuffer &buff ) {
	buff.drawImage(imageMap, mapYOffset, startX, startY, wide, high);
	}	

inline void ProgressBar::erase( GraphicBuffer &buff, Color &bgColor ) {
	buff.eraseImage(startX, startY, wide, high, bgColor);	
	}


#endif