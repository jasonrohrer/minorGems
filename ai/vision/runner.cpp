// Jason Rohrer
// runner.cpp

/**
*
*	Runner for edge stereo
*	Handle command line args and calls stereo function
*
*
*	Created 5-16-2000
*	Mods:
*/

#include <stdlib.h>
#include <time.h>

#include "edgeStereo.h"
#include "localStereo.h"


// prototypes
void usage();
void unknownOption(char *s);
void unknownArgument(char *s);

// where to read params from if no args passed in
char *paramsFileName = "edgeStereo.ini";


int main( int argc, char *argv[] ) {

	printf(".oO()   edgeStereo   ()Oo.\n\n");
	printf("Jason Rohrer, cs664\n\n");
	
	char *leftFileName = NULL;
	char *rightFileName = NULL;
	
	char *outFileName = NULL;
	
	//int boxSize = 0;

	int maxDisparity = 16;
	int edgeThreshold = 0;
	//int hdDilationRadius = 0;
	
	int numStereoIterations = 0;
	
	

	char clearArgV = false;

	if( argc == 0 ) {	// read params from a file
		printf( "No input arguments specified, trying read them from file:  %s\n\n", paramsFileName );
		// try opening the file
		FILE *paramsFile = fopen( paramsFileName, "r" );	// a text file
		
		if( paramsFile == NULL ) {	// file open failed
			printf("Could not open file for reading:  %s\n\n", paramsFileName);
			usage();
			}
		
		
		// read params from file, stick them into arg array
		
		delete [] argv;
		
		argv = new char *[99];		// make it big enough to hold args
		
		int retVal = 1;
		int v = 0;
		
		// note:  skip first argument slot, since this is where name of executable is kept
		
		while( retVal != 0 && retVal != EOF ) {
			v++;
			argv[v] = new char[99];
			retVal = fscanf( paramsFile, "%s", argv[v] );
			}
		// last v-index in argv is not valid arg, so v = number of args in argv
		argc = v;
		
		clearArgV = true;		// set flag to deallocate argv memory
		}
		
		
	if( argc < 11 ) {		// must at least specify 3 files, one edge thresh, one numIter, each with switch before
		usage();
		}
		
		
		
	// argc >= 11
	int i=1;
	
	int p;
	
	while( i<argc ) {
		switch( argv[i][0] ) {
			case '-':			// read any switches
				switch( argv[i][1] ) {
					case '?':
						usage();
						break;
					case 'l':
						i++;
						leftFileName = argv[i];
						i++;				
						break;
					case 'r':
						i++;
						rightFileName = argv[i];
						i++;
						break;
					case 'o':
						i++;
						outFileName = argv[i];
						i++;				
						break;
					case 't':
						i++;
						p = sscanf(argv[i], "%d", &edgeThreshold);
						if( p != 1 ) {		// arg read failed
							printf( "-t must be followed by an integer\n\n");
							usage();
							}
						i++;
						break;
					case 's':
						i++;
						p = sscanf(argv[i], "%d", &numStereoIterations);
						if( p != 1 ) {		// arg read failed
							printf( "-s must be followed by an integer\n\n");
							usage();
							}
						i++;
						break;	
					default:		// handle bad switches
						unknownOption(argv[i]);
						break;
					}
				break;
			default:			// handle bad arguments
				unknownArgument(argv[i]);
				i++;
				break;
			}
		}


/*	if( boxSize <= 1 ) {
		printf("box_size must be greater than 1\n\n");
		usage();
		}
*/
	if( edgeThreshold <= 0 ) {
		printf("edge_threshold must be greater than 0\n\n");
		usage();
		}
	
	if( numStereoIterations <= 0 ) {
		printf("num_iterations must be greater than 0\n\n");
		usage();
		}
	
	if( leftFileName == NULL ) {
		printf( "input_file_left required\n\n");
		usage();
		}
		
	if( rightFileName == NULL ) {
		printf( "input_file_right required\n\n");
		usage();
		}	
	
	if( outFileName == NULL ) {
		printf( "output_file required\n\n");
		usage();
		}
		
		
	// params read correctly
	
	srand( time( NULL ) );
	
	
	// create P_M files
	int status;
	
	P_M *inLeftImage = new P_M( leftFileName, &status );
	if( status != 0 ) {
		usage();
		} 
	
	P_M *inRightImage = new P_M( rightFileName, &status );
	if( status != 0 ) {
		usage();
		} 
	
	int high = inRightImage->getHeight();
	int wide = inRightImage->getWidth();
	int numPixels = wide * high;
	
	unsigned char *outBuffer = new unsigned char[ wide * high ];
	//unsigned char *outBuffer2 = new unsigned char[ wide * high ];
	//unsigned char *outBuffer3 = new unsigned char[ wide * high ]; 
	
	unsigned char *outBufferFin = new unsigned char[ wide * high ]; 
	
	if( outBuffer == NULL || outBufferFin == NULL ) {
		printf( "Memory allocation failed" );
		return 1;
		}
	
	
	
	// for now, use max gray of 16 for disparity 
	P_M *outImage = new P_M( PGM, wide, high, outBuffer, maxDisparity );
	//P_M *outImage2  = new P_M( PGM, wide, high, outBuffer2, maxDisparity );
	//P_M *outImage3 = new P_M( PGM, wide, high, outBuffer3, maxDisparity );
	
	// these are copied by the P_M constructors, so delete them
	delete [] outBuffer;
	//delete [] outBuffer2;
	//delete [] outBuffer3;
	
	// start timing
	unsigned long startTime = (unsigned)time( NULL );
	unsigned long netTime;
	
	
	printf( "Setting up voting table\n" );
	
	int **votes = new int*[numPixels];
	if( votes == NULL ) {		
		printf( "Memory allocation failed" );
		return 1;
		}
	
	// for each pixel, there should be an array of maxDisparity+1 ints
	int numVoteSlots = maxDisparity + 1;
	
	for( i=0; i<numPixels; i++ ) {
		votes[i] = new int[ numVoteSlots ];
		if( votes[i] == NULL ) {		
			printf( "Memory allocation failed" );
			return 1;
			}
		for( int j=0; j<numVoteSlots; j++ ) {
			votes[i][j] = 0;
			}
	/*	if( i%wide == 0 ) {
			printf( "Row done\n" );
			}	
	*/	
		}
	
	printf( "Running stereo iterations\n" );
	
	for( i=0; i<numStereoIterations; i++ ) {
		printf( "iteration %d\n", i );
	
		char retVal = edgeStereoWindow( inLeftImage, inRightImage, outImage, maxDisparity, edgeThreshold ); 
		
		if( ! retVal ) {
			return 1;
			}
		
		
		printf( "iteration %d, tallying vote\n", i );
		
		outBuffer = outImage->getBuffer();
		
		// cast vote
		for( int j=0; j<numPixels; j++ ) {
			// add one to bin for this pixel that corresponds to disparity found in outBuffer
			votes[j][ outBuffer[j] ] += 1;	
			}
		}
		
	
	// call stereo
	
	//edgeStereoHausdorff( inLeftImage, inRightImage, outImage, boxSize, maxDisparity, edgeThreshold, hdDilationRadius );	
	//localStereo( inLeftImage, inRightImage, outImage, boxSize, maxDisparity );
	
	
	// take vote over all out images
	printf( "Finding vote winners\n" );
	
	for( i=0; i<numPixels; i++ ) {
		
		int maxVote = 0;
		int maxVotedDisp = 0;
		
		for( int j=0; j<numVoteSlots; j++ ) {
			if( votes[i][j] > maxVote ) {
				maxVote = votes[i][j];
				maxVotedDisp = j;
				}
			}
		outBufferFin[i] = maxVotedDisp;
		}
	
	
	
	// end timing
	netTime = (unsigned)time( NULL ) - startTime;
	printf("\n%d seconds elapsed.\n", netTime);
	
	
	// write output to file
	P_M *outImageFin = new P_M( PGM, wide, high, outBufferFin, maxDisparity );
	delete [] outBufferFin;
	
	// output last iteration for debugging
	outImage->write( "out1.pgm" );
	
	// user-requested output file
	outImageFin->write( outFileName );
	
	
	
	delete outImage;

	delete outImageFin;
		
	
	printf ("Done.\n");
	
	return 0;
	}		// end of main	
		
		
		
		
		
		
		
		
void usage() {				// print usage help message and exit

	printf("usage:\n");
	printf("edgeStereo\n");
	printf("\t[-?]\n\t");
	printf(" -t edge_threshold -s num_iterations\n\t");
	printf("-l input_file_left -r input_file_rigth\n\t");
	printf("-o output_file\n\n");
	
	
	printf("example:\n\t");
	printf("edgeStereo -t 10 -r 5 -l left.pgm -r right.pgm -o out.pgm\n\n\t");
	printf("edgeStereo\n\n\t");
	
	printf("(if no parameters specified, parameters are read from %s file)\n\n", paramsFileName);
	
	exit(0);
	}
	
void unknownOption(char *s) {
	printf("unknown option:  %s\n\n", s);
	usage();
	}
	
void unknownArgument(char *s) {
	printf("unknown argument:  %s\n\n", s);
	usage();
	}