/*
 * Modification History
 *
 * 2001-April-20   Jason Rohrer
 * Created.  
 */


// tests the sound player class

#include "WaveTerrain.h"
#include "CircularTerrainPathLoop.h"
#include "SoundPlayer.h"
#include <math.h>

#include <time.h>
#include <stdio.h>

int main() {

	
	SoundPlayer *player = new SoundPlayer();
	player->setUpAudio();
	
	float timeScale = 2.0 * M_PI / ( 44100 / 440 );
	
	unsigned char *sampleBuffer = new unsigned char[2];
	short sample;

	long startTime = time( NULL );
	
	for( int i=0; i<441000; i++ ) {
		unsigned char *sampleBuffer = new unsigned char[2];

		sample = (short)( 32768 * sin( ( (float)i ) * timeScale ) );

		sampleBuffer[0] = (unsigned char)( sample >> 8 & 0x00FF );
		sampleBuffer[1] = (unsigned char)( sample  & 0x00FF );

		player->playSamples( sampleBuffer, 2 );
		
		}

	printf( "Elapsed time: %d seconds\n", time( NULL ) - startTime );
	
	delete player;
	delete [] sampleBuffer;
	
	return 0;
	}
