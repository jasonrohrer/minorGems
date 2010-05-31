// Jason Rohrer
// ProgressBar.cpp

/**
*
*	ProgressBar Gui element implementation
*
*
*	Created 11-6-99
*	Mods:	
*		Jason Rohrer	11-8-99		Changed to use GraphicBuffer object as screen buffer
*
*/



#include "ProgressBar.h"


ProgressBar::ProgressBar(int x, int y, int w, int h, Color bordC, Color hC, Color tipC) {

	startX = x;
	startY = y;
	wide = w;
	high = h;
	
	barWide = wide - 2*borderWide;
	barHigh = high - 2*borderWide;
	
	
	borderC = bordC;
	highC = hC;
	barTipC = tipC;
	
	
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
				imageMap[ yContrib + x ] = 0xFF000000;			// black inside
				}
					
			}
	
		}
	
	lastProgress = 0;
	}
	
	
ProgressBar::~ProgressBar() {
	delete [] imageMap;
	delete [] mapYOffset;
	}
	
	
void ProgressBar::setProgress(float fractionFull) {
	
	if( fractionFull < 0) fractionFull = 0;
	if( fractionFull > 1) fractionFull = 1;
	
	if( fractionFull < lastProgress ) {		// decreasing proress, erase part of bar 

		
		int deleteXStart = (int)((fractionFull) * barWide + borderWide);
		
		int deleteXEnd = (int)((lastProgress) * barWide + borderWide);
		
		
		for( int y=borderWide; y<high-borderWide; y++ ) {
		
			int yContrib = mapYOffset[y];
			
			for( int x=deleteXStart; x<=deleteXEnd; x++ ) {
				imageMap[ yContrib + x ] = 0xFF000000;			// erase to black
				}
			// color bar tip
			imageMap[yContrib + deleteXStart] = barTipC.composite;
			}
		}
	else if( fractionFull > lastProgress) {		//progress has increased
	
		int addXStart = (int)((lastProgress) * barWide + borderWide);
		int addXEnd = (int)((fractionFull) * barWide + borderWide);
		
		float weight = lastProgress;
		float deltaWeight = (fractionFull - lastProgress) / (addXEnd - addXStart);

		for( int x=addXStart; x<addXEnd; x++) {
			// weighted color
			unsigned long thisColor = highC.getWeightedComposite(weight);
			weight += deltaWeight;
			for( int y=borderWide; y<high-borderWide; y++) {
				imageMap[mapYOffset[y] + x] = thisColor;
				}
			}
		// color bar tip
		for( int y=borderWide; y<high-borderWide; y++) {
			imageMap[mapYOffset[y] + addXEnd] = barTipC.composite;
			}
		}
	
	lastProgress = fractionFull;
	}	
	

		
