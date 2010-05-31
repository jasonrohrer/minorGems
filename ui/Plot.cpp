// Jason Rohrer
// Plot.cpp

/**
*
*	Scrolling Plot Gui element implementation
*
*
*	Created 11-7-99
*	Mods:	
*		Jason Rohrer	11-8-99		Changed to use GraphicBuffer object as screen buffer
*
*/


#include "Plot.h"


Plot::Plot( int x, int y, int w, int h, Color bordC, Color bC, Color lnC ) {

	startX = x;
	startY = y;
	wide = w;
	high = h;
	
	innerWide = wide - 2*borderWide;
	innerHigh = high - 2*borderWide;
	
	
	borderC = bordC;
	bgC = bC;
	lineC = lnC;
	
	
	imageMap = new unsigned long[high * wide];
	
	mapYOffset = new int[high];	
	
	// precalc y offsets into 2d image map
	for( int y=0; y<high; y++ ) {
		mapYOffset[y] = y*wide;
		}
		
	// prepare image map	
	for( int y=0; y<high; y++ ) {
		int yContrib = mapYOffset[y];
		
		for( int x=0; x<wide; x++ ) {
			
			if( y<borderWide || x<borderWide || y>high-borderWide-1 || x>wide-borderWide-1 ) {
				imageMap[ yContrib + x ] = borderC.composite;			// border
				}
			else {
				imageMap[ yContrib + x ] = bgC.composite;			// background
				}
					
			}
	
		}
	
	
	plotVals = new float[innerWide];
	
	for( int i=0; i<innerWide; i++ ) {
		plotVals[i] = 0;
		}
	
	}
	
Plot::~Plot() {
	delete [] imageMap;
	delete [] mapYOffset;
	delete [] plotVals;
	}
	
	
	
void Plot::addPoint( float p ) {

	// scroll plot vals
	//memmove( (void *)plotVals, (void *)(&(plotVals[1])), sizeof(float) * innerWide );
	for( int i=0; i<innerWide-1; i++ ) {
		plotVals[i] = plotVals[i+1];
		}


	// add new
	plotVals[innerWide-1] = p;


	float largest = 0;	
	// find largest
	for( int i=0; i<innerWide; i++ ) {
		if( largest < plotVals[i] ) {
			largest = plotVals[i];
			}
		}
	
	float invLargest = 1;
	
	if( largest > 0) {
		invLargest = 1/largest;
		}
	

	// fill plot with bg color
	for( int y=borderWide; y<high-borderWide; y++ ) {
		int yContrib = mapYOffset[y];
		for( int x=borderWide; x<wide-borderWide; x++ ) {
			imageMap[ yContrib + x ] = bgC.composite;			// background		
			}
		}
	
	// now plot line
	for( int x=borderWide; x<wide-borderWide; x++ ) {
		
		int y = (int)(plotVals[x-borderWide] * invLargest * innerHigh);
		y = innerHigh - y;
		if( y > innerHigh + borderWide ) y = innerHigh + borderWide -1;
		if( y < borderWide ) y = borderWide;
		
		imageMap[ mapYOffset[y] + x ] = lineC.composite;			// line		
		}


/*	for( int x=0; x<innerWide; x++) {
		int y = innerHigh - (int)(plotVals[x] * innerHigh);
		imageMap[ mapYOffset[y] + x ] = lineC.composite;
		}
*/	
	}