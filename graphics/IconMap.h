// Jason Rohrer
// IconMap.h

/**
*
*	Simple 2d icon map
*
*
*	Created 12-12-99
*	Mods:
*		Jason Rohrer	12-13-99	Added numPixels member	
*		Jason Rohrer	3-22-2000	Added support for drawing icon maps into an icon map
*		Jason Rohrer	12-17-2000	Added include of Color.h
*/



#ifndef ICON_MAP_INCLUDED
#define ICON_MAP_INCLUDED

#include <string.h>
#include "Color.h"

class IconMap {

	
	public:
		
		IconMap( int w, int h);		// construct a map of a certain width and height
	
		IconMap( int w, int h, int *offset );
									// pass in precomputed y offsets into image map
									
		~IconMap();
		
		int wide;
		int high;
		int numPixels;
		
		int *yOffset;
		
		unsigned long *imageMap;
		
		
		// draw a solid IconMap into this IconMap
		void drawIconMap( IconMap *icon, int xPos, int yPos );
		
		// draw a transparent IconMap into this IconMap
		void drawIconMapAlpha( IconMap *icon, int xPos, int yPos );
		
		void copy( IconMap *icon );		// copy contents of another icon map into this one
										// does nothing if icon maps aren't the same size
		
	private:
		char yOffsetExternal;	// is the yOffset ptr external?
		
		float invChannelMax;
		Color utilColor;
		
	};
	
	
inline IconMap::IconMap( int w, int h ) {
	invChannelMax = 1 / 255.0;
	
	wide = w;
	high = h;
	numPixels = wide * high;
	
	imageMap = new unsigned long[ wide * high ];
	
	yOffset = new int[high];
	for( int y=0; y<high; y++ ) {		// precalc y offsets
		yOffset[y] = y * wide;
		}
	yOffsetExternal = false;
	}
	
inline IconMap::IconMap( int w, int h, int *offset ) {
	invChannelMax = 1 / 255.0;
	
	wide = w;
	high = h;
	numPixels = wide * high;
		
	imageMap = new unsigned long[ wide * high ];
	
	yOffset = offset;
	yOffsetExternal = true;
	}	

inline IconMap::~IconMap() {
	delete [] imageMap;
	
	if( !yOffsetExternal ) {
		delete [] yOffset;
		}
	}	



inline void IconMap::drawIconMap( IconMap *icon, int xPos, int yPos ) {
	
	unsigned long *image = icon->imageMap;
	int *imageYOffset = icon->yOffset;
	int imageWide = icon->wide;
	int imageHigh = icon->high;
	
	// watch for buffer bounds 
	int minY = yPos;
	if( minY < 0 ) {
		minY = 0;
		}
	int maxY = yPos + imageHigh;
	if( maxY > high ) {
		maxY = high;
		}
	int minX = xPos;
	if( minX < 0 ) {
		minX = 0;
		}
	int maxX = xPos + imageWide;
	if( maxX > wide ) {
		maxX = wide;
		}
	
	for( int y=minY; y<maxY; y++ ) {
		int imageY = y-yPos;
	
		int buffYContrib = yOffset[y];
		int imageYContrib = imageYOffset[imageY];
	
		for( int x=minX; x<maxX; x++ ) {
			int imageX = x-xPos;
			imageMap[ buffYContrib + x ] = image[ imageYContrib + imageX ];	
			}
		}
	}


inline void IconMap::drawIconMapAlpha( IconMap *icon, int xPos, int yPos ) {
	
	unsigned long *image = icon->imageMap;
	int *imageYOffset = icon->yOffset;
	int imageWide = icon->wide;
	int imageHigh = icon->high;
	
	// watch for buffer bounds 
	int minY = yPos;
	if( minY < 0 ) {
		minY = 0;
		}
	int maxY = yPos + imageHigh;
	if( maxY > high ) {
		maxY = high;
		}
	int minX = xPos;
	if( minX < 0 ) {
		minX = 0;
		}
	int maxX = xPos + imageWide;
	if( maxX > wide ) {
		maxX = wide;
		}

	for( int y=minY; y<maxY; y++ ) {
		int imageY = y-yPos;
		
		int buffYContrib = yOffset[y];
		int imageYContrib = imageYOffset[imageY];
		
		for( int x=minX; x<maxX; x++ ) {
			int imageX = x-xPos;
			// get alpha value for this icon pixel
			unsigned long argb = image[ imageYContrib + imageX ];
			float alpha = (argb >> 24) * invChannelMax;

			float oneMinusAlpha = 1-alpha;
			
			unsigned long buffARGB = imageMap[ buffYContrib + x ];
			
			argb = utilColor.getWeightedComposite( argb, alpha );
			buffARGB = utilColor.getWeightedComposite( buffARGB, oneMinusAlpha );
			
			unsigned long sum = utilColor.sumComposite( argb, buffARGB );
			
			imageMap[ buffYContrib + x ] = sum;
			}
		}
	}


inline void IconMap::copy( IconMap *icon ) {
	// make sure they are the same size
	if( numPixels != icon->numPixels ) {
		return;
		}
	
	// void * memcpy (void * dst, const void * src, size_t len);
	
	// each pixel is 4 bytes, so shift numPixels by 2
	memcpy( (void *)(imageMap), (void *)(icon->imageMap), numPixels << 2 );
	}

#endif
	
