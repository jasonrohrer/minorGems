/*
 * Modification History
 *
 * 2001-April-20   Jason Rohrer
 * Created.
 *
 * 2001-April-22   Jason Rohrer
 * Updated to use a new path class, and tweaked to produce some interesting
 * sounds.
 */


// tests the wave terrain classes

#include "WaveTerrain.h"
#include "CircularTerrainPath.h"
#include "EpicycleTerrainPath.h"
#include "SoundPlayer.h"
#include "WaveTerrainDisplay.h"


#include "minorGems/util/random/StdRandomSource.h"
#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/filters/BoxBlurFilter.h"
#include "subreal/common/filters/FractalNoiseFilter.h"

#include <math.h>

#include <time.h>
#include <stdio.h>
#include <signal.h>

/**
 * Computes a periodic square function.
 *
 * Wavelength = 2*PI.
 *
 * @param inValue the time value to compute the square function for.
 *
 * @return the square function applied to inValue.
 */
double squareFunction( double inValue );


void catch_int(int sig_num) {
	printf( "Quiting...\n" );
	//currentStep = numSteps;
	exit( 0 );
	signal( SIGINT, catch_int );
	}


// sound time in seconds
double soundTime = 50;
// sound buffer size in samples
int soundBufferSize = 500;

double radiusRatio = 1.0;

int main() {

	// let catch_int handle interrupt (^c)
    signal( SIGINT, catch_int );
	
	// setup the player
	SoundPlayer *player = new SoundPlayer();
	player->setUpAudio();

	StdRandomSource *randSource = new StdRandomSource();

	int x, y;

	// construct a terrain
	int wide = 500;
	int high = 500;

	double invWide = 1.0 / wide;
	double invHigh = 1.0 / high;

	double **map = new double*[high];
	
	/*Image *mapImage = new Image( wide, high, 1 );
	
	FractalNoiseFilter *noiseFilter =
		new FractalNoiseFilter( randSource );
	noiseFilter->setFPower( 0.5 );
	//noiseFilter->setMaxFrequency( 0.01 );
	mapImage->filter( noiseFilter );

	delete noiseFilter;

	
	BoxBlurFilter *blurFilter = new BoxBlurFilter( 5 );
	mapImage->filter( blurFilter );
	delete blurFilter;
	
	double *imagePixels = mapImage->getChannel( 0 );
	*/
	
	/*double *imagePixels = mapImage->getChannel( 0 );
	
	

	for( y=0; y<high; y++ ) {
		for( x=0; x<wide; x++ ) {
			imagePixels[ y * wide + x ] =
				squareFunction(
					( x / ( (double)wide - 0.75 * x ) ) * 2 * M_PI )
				* squareFunction( ( y / ( (double)high * 0.5 ) ) * 2 * M_PI );
			// bring into [0, 1 range]
			imagePixels[ y * wide + x ] =
				0.5 * ( imagePixels[ y * wide + x ] + 1.0 );
			}
		}

	BoxBlurFilter *blurFilter = new BoxBlurFilter( 5 );
	mapImage->filter( blurFilter );
	delete blurFilter;
	*/
	for( y=0; y<high; y++ ) {
		map[y] = new double[wide];
		for( x=0; x<wide; x++ ) {
			//map[y][x] = imagePixels[ y * wide + x ];
			//map[y][x] = randSource->getRandomDouble();
			//map[y][x] = ( 1.0 + sin( x / 10.0 ) * sin( y / 10.0 ) ) * 0.5;
			/*map[y][x] =
				squareFunction(
					( x / ( (double)wide - 0.75 * x ) ) * 2 * M_PI )
				* squareFunction( ( y / ( (double)high * 0.5 ) ) * 2 * M_PI );

			//map[y][x] = squareFunction( ( y / 500.0 ) * 2 * M_PI );
			
			// bring into [0, 1 range]
			map[y][x] = 0.5 * ( map[y][x] + 1.0 );

			if( map[y][x] > 1.0 || map[y][x] < 0  ) {
				printf( "map out of range: %f\n", map[y][x] );
				}
			*/
			//map[y][x] = sin( x / 1000.0 );

			map[y][x] = sin( 30 * sqrt(
				x / ( (double)wide ) * x / ( (double)wide )
				+  y / ( (double)high ) * y / ( (double)high ) ) );
			map[y][x] *= sin( 30 * sqrt(
				( x - wide ) / ( (double)wide ) *
				( x - wide ) / ( (double)wide )
				+
				( y ) / ( (double)high ) *
				( y ) / ( (double)high ) ) );
				
			// bring into [0, 1 range]
			map[y][x] = 0.5 * ( map[y][x] + 1.0 );
			
			}
		}

	//delete mapImage;
	
	WaveTerrain *terrain = new WaveTerrain( map, wide, high );

	//CircularTerrainPath *path =
	//	new CircularTerrainPath( 0.5 );
    EpicycleTerrainPath *path =
		new EpicycleTerrainPath( 0.5, 0, 0, 0 );
	
	WaveTerrainDisplay *display =
		new WaveTerrainDisplay( terrain, path, 100, 100 );
	
	int sampleRate = 44100;
	double invSampleRate = 1.0 / sampleRate;
	
	float timeScale = 2.0 * M_PI / ( 44100 / 30 );
	
	unsigned char *sampleBuffer = new unsigned char[2];
	short shortSample;
	double doubleSample;
	
	long startTime = time( NULL );

	unsigned char *soundBuffer = new unsigned char[ soundBufferSize * 2 ];

	int numBuffers = (int)( soundTime * sampleRate / soundBufferSize );
	
	printf( "generating\n" );

	double radius;

	int j=0;
	//for( int j=0; j<numBuffers; j++ ) {
	while( true ) {
		
		for( int k=0; k<soundBufferSize; k++ ) {
			int i = j * soundBufferSize + k;

			//radius =
			//	( sin( i * invSampleRate * 30 ) + 1 ) * 0.0625
			//	+ ( sin( i * invSampleRate * 3 ) + 1 ) * 0.1875;
			radius = ( sin( i * invSampleRate ) + 1 ) * 0.125 + .25;
			path->setRadius( radius );

			path->setRotationOffset( i * 0.000000534 );

			//path->setRotationRatio( 4 );
			path->setRotationRatio(
				0.1 * sin( i * 0.00001 + (3/2) * M_PI ) + 4 );

			//path->setRadiusRatio( 1.0 );
			path->setRadiusRatio(
				10 * sin( i * 0.00002345 + (3/2) * M_PI ) + 11 );
			
			//double t = i * ( 0.001 + 0.0001 * sin( i * 0.000003243 ) );
			double t = i * 0.0005;
				
			double x, y;

			path->getPosition( t, &x, &y );

			path->getPosition( t, &x, &y );

			//printf( "t = %f, x = %f, y= %f\n", t, x, y );

			doubleSample = terrain->getSample( x, y );
			//printf( "sample = %f\n", doubleSample );

			//doubleSample = sin( i * timeScale );
		
			// this part writes the sample to speakers
			shortSample = (short)( 65535 * doubleSample - 32767 );

			//printf( "sample = %d\n", shortSample );
		
			//sampleBuffer[0] = (unsigned char)( shortSample >> 8 & 0x00FF );
			//sampleBuffer[1] = (unsigned char)( shortSample  & 0x00FF );

			soundBuffer[ k * 2] =
				(unsigned char)( shortSample >> 8 & 0x00FF );
			soundBuffer[ k * 2 + 1] =
				(unsigned char)( shortSample  & 0x00FF );
			}
		// dump the buffer to speakers
		player->playSamples( soundBuffer, 2 * soundBufferSize );

		j++;
		}
	/*for( int i=0; i<441000; i++ ) {

		radius = ( sin( i * invSampleRate * 3 ) + 1 ) * 0.25;

		path->setRadius( radius );
		
		double t = ( i % 100 ) * 0.01;

		double x, y;

		path->getPosition( t, &x, &y );

		path->getPosition( t, &x, &y );

		//printf( "t = %f, x = %f, y= %f\n", t, x, y );

		doubleSample = terrain->getSample( x, y );
		//printf( "sample = %f\n", doubleSample );

		//doubleSample = sin( i * timeScale );
		
		// this part writes the sample to speakers
		shortSample = (short)( 65535 * doubleSample - 32767 );

		//printf( "sample = %d\n", shortSample );
		
		sampleBuffer[0] = (unsigned char)( shortSample >> 8 & 0x00FF );
		sampleBuffer[1] = (unsigned char)( shortSample  & 0x00FF );

		//soundBuffer[ i * 2] = (unsigned char)( shortSample >> 8 & 0x00FF );
		//soundBuffer[ i*2 +1] = (unsigned char)( shortSample  & 0x00FF );
		player->playSamples( sampleBuffer, 2 );
		
		}
	*/
	//printf( "playing\n" );
	//player->playSamples( soundBuffer, 441000 * 2 );
	
	printf( "Elapsed time: %d seconds\n", time( NULL ) - startTime );
	
	delete player;
	delete randSource;

	//Thread::sleep( 2000 );
	
	display->stop();

	delete display;
	
	// deleting this will delete our map
	delete terrain;

	delete path;
		
	delete [] sampleBuffer;
	
	return 0;
	}



double squareFunction( double inValue ) {
	double value = sin( inValue );
	if( value >= 0 ) {
		value = 1.0;
		}
	else {
		value = -1.0;
		}
	return value;
	}
