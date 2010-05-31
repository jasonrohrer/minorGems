
//#include <SDL/SDL.h>

#include "minorGems/graphics/ScreenGraphics.h"
#include "minorGems/graphics/GraphicBuffer.h"
#include "minorGems/graphics/Color.h"

#include "minorGems/ui/Mouse.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int numIcons = 200;

int currentStep = 0;
int numSteps = 1000;

void catch_int(int sig_num) {
	printf( "Quiting..." );
	currentStep = numSteps;
	signal( SIGINT, catch_int);
	}

int main() {
	int i, j;
	
	// let catch_int handle interrupt (^c)
    signal( SIGINT, catch_int);
	
	ScreenGraphics *graphics = new ScreenGraphics( 640, 480 );
	Mouse *mouse = new Mouse(2);
	
	
	unsigned long *pixelBuff = new unsigned long[ 640 * 480 ];
	
	GraphicBuffer *buffer = new GraphicBuffer( pixelBuff, 640, 480 );
	
	IconMap **maps = new IconMap*[numIcons];
	
	IconMap *mouseMap;
	
	IconMap *mouseMap1 = new IconMap( 10, 10 );
	Color mouseColor1( 1.0, 0.0, 0.0, 1.0 );
	for( int y=0; y<10; y++ ) {
		for( int x=0; x<10; x++ ) {	
			mouseMap1->imageMap[ mouseMap1->yOffset[y] + x ] = 
				mouseColor1.composite;
			}
		}
	IconMap *mouseMap2 = new IconMap( 10, 10 );
	Color mouseColor2( 0.0, 1.0, 0.0, 1.0 );
	for( int y=0; y<10; y++ ) {
		for( int x=0; x<10; x++ ) {	
			mouseMap2->imageMap[ mouseMap2->yOffset[y] + x ] = 
				mouseColor2.composite;
			}
		}
	
	IconMap *mouseMap3 = new IconMap( 10, 10 );
	Color mouseColor3( 0.0, 0.0, 1.0, 1.0 );
	for( int y=0; y<10; y++ ) {
		for( int x=0; x<10; x++ ) {	
			mouseMap3->imageMap[ mouseMap3->yOffset[y] + x ] = 
				mouseColor3.composite;
			}
		}
		
	mouseMap = mouseMap1;	
		
	Color c( 0.0f, 0.0f, 0.0f, 1.0f );
	buffer->fill( c );
	
	float *xPos = new float[numIcons];
	float *yPos = new float[numIcons];
	float *xDelta = new float[numIcons];
	float *yDelta = new float[numIcons];
	
	for( i=0; i<numIcons; i++ ) {
		xPos[i] =  (float)640 * rand() / RAND_MAX;
		yPos[i] = (float)480 * rand() / RAND_MAX;
		xDelta[i] = 10.0f * rand() / RAND_MAX;
		yDelta[i] = 10.0f * rand() / RAND_MAX;
		maps[i] = new IconMap( 10, 10 );
		Color randColor( (float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1.0 );
		for( int y=0; y<10; y++ ) {
			for( int x=0; x<10; x++ ) {	
				maps[i]->imageMap[ maps[i]->yOffset[y] + x ] = 
					randColor.composite;
				}
			}
		}
	int mouseX = 0;
	int mouseY = 0;	
	char buttonDown1 = false;
	char buttonDown2 = false;
	for( currentStep=0; currentStep<numSteps; currentStep++ ) {
		buffer->eraseIconMap( mouseMap, mouseX, mouseY, c );
		
		mouse->getLocation( &mouseX, &mouseY );
		
		buffer->drawIconMap( mouseMap, mouseX, mouseY );
		
		if( !buttonDown1 && mouse->isButtonDown(0) ) {
			buttonDown1 = true;
			mouseMap = mouseMap2;
			}
		else if( buttonDown1 && !mouse->isButtonDown(0) ) {
			buttonDown1 = false;
			mouseMap = mouseMap1;
			}
		
		else if( !buttonDown2 && mouse->isButtonDown(1) ) {
			buttonDown2 = true;
			mouseMap = mouseMap3;
			}
		else if( buttonDown2 && !mouse->isButtonDown(1) ) {
			buttonDown2 = false;
			mouseMap = mouseMap1;
			}
			
		
		
		for( i=0; i<numIcons; i++ ) {
			buffer->eraseIconMap( maps[i], (int)( xPos[i] - 5 ), 
				(int)( yPos[i] - 5 ), c );
			if( xPos[i] > 640 || xPos[i] < 0 ) {
				xDelta[i] = -xDelta[i];
				}
			xPos[i] += xDelta[i];
			if( yPos[i] > 480 || yPos[i] < 0 ) {
				yDelta[i] = -yDelta[i];
				}
			yPos[i] += yDelta[i];

			buffer->drawIconMap( maps[i], (int)( xPos[i] - 5 ), 
				(int)( yPos[i] - 5 ) );
			}
		
		graphics->swapBuffers( buffer );
		}
		
	
	/*
	for( int i=0; i<100; i++ ) {
		if( i%2 == 0 ) {
			Color c( 1.0f, 0.0f, 0.0f, 1.0f );
			buffer->fill( c );
			}
		else {
			Color c( 0.0f, 1.0f, 0.0f, 1.0f );
			buffer->fill( c );
			}
		graphics->swapBuffers( buffer );
		}
	*/
	printf( "Done.\n" );
	return 0;
	/*
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "Couldn't initialize SDL: %s\n", SDL_GetError() );
		exit(1);
		}
	
	//atexit( SDL_Quit );
	
	SDL_Surface *screen = SDL_SetVideoMode( 640, 480, 
		32, SDL_HWSURFACE | SDL_DOUBLEBUF );//| SDL_FULLSCREEN );
		
	if ( screen == NULL ) {
		printf( "Couldn't set 640x480x32 video mode: %s\n",
                                       SDL_GetError() );
		exit(1);
		}
	
	for( int i=0; i< 100; i++ ) {
		if ( SDL_MUSTLOCK(screen) ) {
			if ( SDL_LockSurface(screen) < 0 ) {
				printf( "Couldn't lock screen: %s\n",
                                    	   SDL_GetError());
				exit(1);
				}
			}

		Uint32 value;
		if( i%2 == 0 ) {
			value = 0x0;
			}
		else {
			value = 0xFFFFFFFF;
			}

		Uint32 *buffer = (Uint32 *)( screen->pixels );
		for ( int y=0; y<screen->h; y++ ) {
			for ( int x=0; x<screen->w; x++ ) {

				int r = ( ( ( x * 255 ) / screen->w ) + i ) % 255;
				int g = ( ( ( y * 255 ) / screen->h ) + i ) % 255;
				int b = 0;
				int a = 255;
				
				//buffer[ y * screen->w + x ] = (a << 24) | (r << 16) | (g << 8) | b;
				buffer[ y * screen->w + x ] = value;
				}
			}

		if ( SDL_MUSTLOCK(screen) ) {
			SDL_UnlockSurface(screen);
			}

		//SDL_UpdateRect( screen, 0, 0, screen->w, screen->h );
		SDL_Flip( screen );
		}
	SDL_Quit();
	
	SDL_Delay(2000);

	exit(0);
	*/
	}
