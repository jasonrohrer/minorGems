// Jason Rohrer
// GraphicBufffer.h

/**
*
*	Graphic buffer, with functions for manipulation
*
*
*	Created 11-8-99
*	Mods:	
*		Jason Rohrer	12-12-99	Added support for drawing IconMap objects
*		Jason Rohrer	11-18-2000	Added a getBuffer function.
*/



#ifndef GRAPHIC_BUFFER_INCLUDED
#define GRAPHIC_BUFFER_INCLUDED


#include "Color.h"
#include <stdio.h>
#include "IconMap.h"

class GraphicBuffer {


	public:
		GraphicBuffer( unsigned long *buff, int buffW, int buffH );
		~GraphicBuffer();
				
		// draw a solid image into the buffer
		void drawImage( unsigned long *image, int *imageYOffset, int xPos, int yPos, int wide, int high );
		
		// draw a transparent image into the buffer
		void drawImageAlpha( unsigned long *image, int *imageYOffset, int xPos, int yPos, int wide, int high );
		
		// replace a rectangle in the buffer with bgColor
		void eraseImage( int xPos, int yPos, int wide, int high, Color &bgColor );

		
		// draw a solid IconMap into the buffer
		void drawIconMap( IconMap *icon, int xPos, int yPos );
		
		// draw a transparent image into the buffer
		void drawIconMapAlpha( IconMap *icon, int xPos, int yPos );
		
		// replace a rectangle (over icon) in the buffer with bgColor
		void eraseIconMap( IconMap *icon, int xPos, int yPos, Color &bgColor );

		

		// set buffer to a new buffer of the same size
		void setBuffer( unsigned long *buff );
		
		// get the pixel buffer
		unsigned long *getBuffer();
		
		int getWidth();
		int getHeight();
		
		// fill buffer with a color
		void fill( Color &fillC );

		// take a screen shot, save to disc
		void screenShot( FILE *f );

	private:
		unsigned long *buffer;
		int bufferHigh;
		int bufferWide;
		
		int *buffYOffset;
		
		float invChannelMax;
		Color utilColor;		// color for using composite functions
	
	};


inline GraphicBuffer::GraphicBuffer( unsigned long *buff, int buffW, int buffH) : utilColor(0,0,0,0) {

	invChannelMax = 1 / 255.0;
	
	bufferHigh = buffH;
	bufferWide = buffW;
	
	buffer = buff;

	// precalc'ed y contributions to linear indexing of buffer
	buffYOffset = new int[bufferHigh];
	for( int y=0; y<bufferHigh; y++) {
		buffYOffset[y] = y * bufferWide;
		} 
	}

inline GraphicBuffer::~GraphicBuffer() {
	delete [] buffYOffset;
	}


inline void GraphicBuffer::drawImage( unsigned long *image, int *imageYOffset, int xPos, int yPos, int wide, int high ) {
	
	// watch for buffer bounds 
	int minY = yPos;
	if( minY < 0 ) {
		minY = 0;
		}
	int maxY = yPos + high;
	if( maxY > bufferHigh ) {
		maxY = bufferHigh;
		}
	int minX = xPos;
	if( minX < 0 ) {
		minX = 0;
		}
	int maxX = xPos + wide;
	if( maxX > bufferWide ) {
		maxX = bufferWide;
		}
	
	for( int y=minY; y<maxY; y++ ) {
		int imageY = y-yPos;
	
		int buffYContrib = buffYOffset[y];
		int imageYContrib = imageYOffset[imageY];
	
		for( int x=minX; x<maxX; x++ ) {
			int imageX = x-xPos;
			buffer[ buffYContrib + x ] = image[ imageYContrib + imageX ];	
			}
		}
	}

inline void GraphicBuffer::eraseImage( int xPos, int yPos, int wide, int high, Color &bgColor ) { 
	
	// watch for buffer bounds 
	int minY = yPos;
	if( minY < 0 ) {
		minY = 0;
		}
	int maxY = yPos + high;
	if( maxY > bufferHigh ) {
		maxY = bufferHigh;
		}
	int minX = xPos;
	if( minX < 0 ) {
		minX = 0;
		}
	int maxX = xPos + wide;
	if( maxX > bufferWide ) {
		maxX = bufferWide;
		}

	unsigned long composite = bgColor.composite;

	for( int y=minY; y<maxY; y++ ) {
		int buffYContrib = buffYOffset[y];
		for( int x=minX; x<maxX; x++ ) {
			buffer[ buffYContrib + x ] = composite;	
			}
		}
	}

inline void GraphicBuffer::drawImageAlpha( unsigned long *image, int *imageYOffset, int xPos, int yPos, int wide, int high ) {
	
	// watch for buffer bounds 
	int minY = yPos;
	if( minY < 0 ) {
		minY = 0;
		}
	int maxY = yPos + high;
	if( maxY > bufferHigh ) {
		maxY = bufferHigh;
		}
	int minX = xPos;
	if( minX < 0 ) {
		minX = 0;
		}
	int maxX = xPos + wide;
	if( maxX > bufferWide ) {
		maxX = bufferWide;
		}

	for( int y=minY; y<maxY; y++ ) {
		int imageY = y-yPos;
		
		int buffYContrib = buffYOffset[y];
		int imageYContrib = imageYOffset[imageY];
		
		for( int x=minX; x<maxX; x++ ) {
			int imageX = x-xPos;
			// get alpha value for this icon pixel
			unsigned long argb = image[ imageYContrib + imageX ];
			float alpha = (argb >> 24) * invChannelMax;

			float oneMinusAlpha = 1-alpha;
			
			unsigned long buffARGB = buffer[ buffYContrib + x ];
			
			argb = utilColor.getWeightedComposite( argb, alpha );
			buffARGB = utilColor.getWeightedComposite( buffARGB, oneMinusAlpha );
			
			unsigned long sum = utilColor.sumComposite( argb, buffARGB );
			
			buffer[ buffYContrib + x ] = sum;
			}
		}
	}


inline void GraphicBuffer::drawIconMap( IconMap *icon, int xPos, int yPos ) {
	drawImage( icon->imageMap, icon->yOffset, xPos, yPos, icon->wide, icon->high );
	}
		
		
inline void GraphicBuffer::drawIconMapAlpha( IconMap *icon, int xPos, int yPos ) {
	drawImageAlpha( icon->imageMap, icon->yOffset, xPos, yPos, icon->wide, icon->high );
	}
	
	
inline void GraphicBuffer::eraseIconMap( IconMap *icon, int xPos, int yPos, Color &bgColor ) {
	eraseImage( xPos, yPos, icon->wide, icon->high, bgColor );
	}


inline void GraphicBuffer::fill( Color &fillC ) {
	unsigned long composite = fillC.composite;
	for( int y=0; y<bufferHigh; y++ ) {
		int buffYContrib = buffYOffset[y];
		for( int x=0; x<bufferWide; x++ ) {
			buffer[ buffYContrib + x ] = composite;
			}
		}	
	}


inline void GraphicBuffer::setBuffer( unsigned long *buff ) {
	buffer = buff;
	}

inline unsigned long *GraphicBuffer::getBuffer() {
	return buffer;
	}

inline int GraphicBuffer::getWidth() {
	return bufferWide;
	}
inline int GraphicBuffer::getHeight() {
	return bufferHigh;
	}

inline void GraphicBuffer::screenShot( FILE *f ) {
	fwrite( (void*)buffer, sizeof(long), bufferHigh*bufferWide, f );
	}


#endif
