/*
 * Modification History
 *
 * 2000-November-19		Jason Rohrer
 * Created.
 *
 * 2001-November-29		Jason Rohrer
 * Made screen size smaller.
 */


#include "SimpleInstructionSet.h"
#include "BitMemory.h"

#include "minorGems/graphics/ScreenGraphics.h"
#include "minorGems/graphics/GraphicBuffer.h"

#include "minorGems/util/random/StdRandomSource.h"

#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include <SDL.h>

int currentStep = 0;
int numSteps = 1000;

void catch_int(int sig_num) {
	printf( "Quiting..." );
	currentStep = numSteps;
	signal( SIGINT, catch_int);
	}

int main() {
	
	StdRandomSource *randSource = new StdRandomSource();
	
    // let catch_int handle interrupt (^c)
    signal( SIGINT, catch_int);
	
	SimpleInstructionSet *set = new SimpleInstructionSet();
	
	int screenW = 256;
	int screenH = 256;
	
	unsigned long memorySize = screenW * screenH * 32;
	memorySize = set->setMemorySize( memorySize );
	
	BitMemory *mem = new BitMemory( memorySize );
	
	
	
	//mem->printMemory();
	printf( "\n" );
	
	unsigned char *inputArray = new unsigned char[memorySize >> 3];
	
	unsigned long randSeed = time( NULL );
	
	printf( "randSeed = %d\n", randSeed );
	
	//srand( 974682169 );//randSeed );
	srand( randSeed );
	unsigned char counter = (unsigned char)( rand() % 255 );
	for( int b=0; b<(memorySize >> 3)-1; b++ ) {
	//for( int b=0; b<(memorySize >> 3)-1; b+=4 ) {
		/*inputArray[b] = 255;
		inputArray[b+1] = 100;
		inputArray[b+2] = 128;
		inputArray[b+3] = 255;
		*/
		inputArray[b] = counter;
		
		counter++;
		/*if( b % ( screenW * 4 ) == 0 ) {
			counter = (unsigned char)( rand() % 255 );
			}
		*/
		
		}
	mem->setMemoryRange( 0, memorySize - 1, inputArray );	
	
	delete [] inputArray;
	
	// setup screen and buffer
	ScreenGraphics *screen = new ScreenGraphics( screenW, screenH );
	
	unsigned long numChars;
	unsigned char *memBlocks = mem->getAllMemory( &numChars );
	unsigned long *screenBlocks = new unsigned long[ screenW * screenH ];
	
	memcpy( (void*)screenBlocks, (void *)memBlocks, numChars );	
		
	GraphicBuffer *screenBuffer = new GraphicBuffer( screenBlocks, 
		screenW, screenH );
	
	screen->swapBuffers( screenBuffer );
	SDL_Delay( 2000 );
	delete [] memBlocks;
	
	printf( "Starting program:\n" );
	
	//mem->printMemory();
	int numPrograms = 1;
	
	unsigned long *pc = new unsigned long[numPrograms];
	int p;
	for( p=0; p<numPrograms; p++ ) {
		//pc[p] = randSource->getRandomBoundedInt( 0, memorySize-1 );
		pc[p] = 0;
		}
	
	for( currentStep=0; currentStep<numSteps; currentStep++ ) {
		for( p=0; p<numPrograms; p++ ) {
			//printf( "%d ", pc[p] );
			pc[p] = set->stepProgram( mem, pc[p] );
			}
		memBlocks = 
			mem->getAllMemory( &numChars );
		memcpy( (void*)screenBlocks, (void *)memBlocks, numChars );	
		screen->swapBuffers( screenBuffer );
		
		delete [] memBlocks;
		//mem->printMemory();
		} 
	
	delete [] pc;
	
	
	mem->clearMemory();
	//mem->printMemory();
	
	delete screen;
	delete screenBuffer;
	delete [] screenBlocks;
	
	delete mem;
	delete set;
	printf( "Done.\n" );
	}
