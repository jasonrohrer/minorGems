/*
 * Modification History
 *
 * 2001-May-18   Jason Rohrer
 * Created.
 *
 * 2001-May-21   Jason Rohrer
 * Updated to use gray codes, and tested replacement of noisy regions
 * with pseudo-random noise.  Does not produce good results,
 * and does not seem like a viable compression method.
 */
 
 
#include "BitField.h"
#include "ImageBitFields.h"
#include "GrayCode.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ImageColorConverter.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"
#include "minorGems/io/file/FileInputStream.h"
#include "minorGems/graphics/converters/BMPImageConverter.h"

#include "minorGems/util/random/StdRandomSource.h"

#include <string.h>
#include <stdio.h>

// a test function for ImageBitFields

// prototypes:
void usage( char *inAppName );


int main( char inNumArgs, char **inArgs ) {

	if( inNumArgs != 5 ) {
		usage( inArgs[0] );
		}

	float complexityThreshold;
	if( 1 != sscanf( inArgs[3], "%f", &complexityThreshold ) ) {
		usage( inArgs[0] );
		}
	int useGray;
	if( 1 != sscanf( inArgs[4], "%d", &useGray ) ) {
		usage( inArgs[0] );
		}

	int blockSize = 8;
	
	StdRandomSource *randSource = new StdRandomSource(); 
	
	BMPImageConverter *converter = new BMPImageConverter();


	File *inFile = new File( NULL, inArgs[1], strlen( inArgs[1] ) );

	FileInputStream *inStream = new FileInputStream( inFile );

	Image *inImage = converter->deformatImage( inStream );

	printf( "image size = (%d,%d)\n",
			inImage->getWidth(), inImage->getHeight() );
	
	delete inFile;
	delete inStream;


	// 8 fields per channel
	BitField ***fields =
		ImageBitFields::imageToBitFields( inImage, 8 );


	printf( "testing gray codes\n" );
	char *bitString = new char[3];
	for( int c=0; c<8; c++ ) {
		bitString[0] = 0x1 & ( c >> 2 );
		bitString[1] = 0x1 & ( c >> 1 );
		bitString[2] = 0x1 & ( c >> 0 );

		printf( "%d%d%d ", bitString[0], bitString[1], bitString[2] );
		char *codeString = GrayCode::encode( bitString, 3 );
		printf( "%d%d%d ", codeString[0], codeString[1], codeString[2] );

		char *code2String = GrayCode::decode( codeString, 3 );
		printf( "%d%d%d\n", code2String[0], code2String[1], code2String[2] );

		delete [] codeString;
		delete [] code2String;
		}
	
	if( useGray ) {
		// gray encode the bit fields
		for( int c=0; c<3; c++ ) {
			char *bits = new char[8];
		
			int w = fields[c][0]->getWidth();
			int h = fields[c][0]->getHeight();
			int numPixels = w * h;

			for( int i=0; i<numPixels; i++ ) {
				// copy the bits into our array
				int b;
				for( b=0; b<8; b++ ) {
					char *field = fields[c][b]->getField();
					bits[b] = field[i];
					}
				char *grayBits = GrayCode::encode( bits, 8 );

				// copy the gray bits back into our bit fields
				for( b=0; b<8; b++ ) {
					char *field = fields[c][b]->getField();
					field[i] = grayBits[b];
					}
				delete [] grayBits;
				}
			delete [] bits;
			}
		}
	
	
	int numBlocksCompressed = 0;
	int numBlocksTotal = 0;

	int numBlocksHighOrLow = 0;
	
	int maxNumBlockChanges = 2 * ( blockSize - 1 ) * blockSize;
	
	// fill in high-noise blockSize x blockSize bit blocks with biased noise
	for( int c=0; c<3; c++ ) {
		for( int b=0; b<8; b++ ) {
			
			char *field = fields[c][b]->getField();
			int w = fields[c][b]->getWidth();
			int h = fields[c][b]->getHeight();
			int numPixels = w * h;

			// for each row in this block
			for( int blockY=0; blockY<h/blockSize; blockY++ ) {

				
				for( int blockX=0; blockX<w/blockSize; blockX++ ) {
					numBlocksTotal++;
					
					// sum at all 64 pixels in this blockSize x blockSize block
					char valSum = 0;
					
					int startY = blockY * blockSize;
					int startX = blockX * blockSize;

					// count the number of changes in the block
					int numBlockChanges = 0;

					// look at each row in this block
					for( int y=startY; y<startY + blockSize; y++ ) {
						int currentColor = field[ y * w + startX ];
						int numRowChanges = 0;
						
						for( int x=startX + 1; x<startX + blockSize; x++ ) {
							int newColor = field[ y * w + x ];
							if( newColor != currentColor ) {
								numRowChanges++;
								currentColor = newColor;
								}
							valSum += field[ y * w + x ];
							}
						numBlockChanges += numRowChanges;
						}
					
					// look at each column in this block
					for( int x=startX; x<startX + blockSize; x++ ) {
						int currentColor = field[ startY * w + x ];
						int numColumnChanges = 0;
						
						for( int y=startY + 1; y<startY + blockSize; y++ ) {
							int newColor = field[ y * w + x ];
							if( newColor != currentColor ) {
								numColumnChanges++;
								currentColor = newColor;
								}
							}
						numBlockChanges += numColumnChanges;
						}
					
					
					float blockWeight = (float)valSum /
						(float)( blockSize * blockSize );

					if( blockWeight == 0 || blockWeight == 1.0 ) {
						numBlocksHighOrLow++;
						}

					float complexity =
						(float)numBlockChanges /
						(float)maxNumBlockChanges;
					
					// if the block is outside the thresholds
					// replace it with biased random noise
					if( complexity >= complexityThreshold ) {
						
						// now randomly fill in pixels using
						// a bias of blockWeight
						for( int y=startY; y<startY + blockSize; y++ ) {
							for( int x=startX; x<startX + blockSize; x++ ) {
								if( randSource->getRandomFloat() <= 0.5 ) {

									field[ y * w + x ] = 1;
									}
								else {
									field[ y * w + x ] = 0;
									}
								}
							}
						numBlocksCompressed++;
						}
					// else leave the block alone					
					}
				}
			}
		}


	if( useGray ) {
		// gray decode the bit fields
		for( int c=0; c<3; c++ ) {
			char *grayBits = new char[8];
		
			int w = fields[c][0]->getWidth();
			int h = fields[c][0]->getHeight();
			int numPixels = w * h;

			for( int i=0; i<numPixels; i++ ) {
				// copy the bits into our array
				int b;
				for( b=0; b<8; b++ ) {
					char *field = fields[c][b]->getField();
					grayBits[b] = field[i];
					}
				char *bits = GrayCode::decode( grayBits, 8 );

				// copy the bits back into our bit fields
				for( b=0; b<8; b++ ) {
					char *field = fields[c][b]->getField();
					field[i] = bits[b];
					}
				delete [] bits;
				}
			delete [] grayBits;
			}
		}
	
	
	printf( "%d blocks random out of %d\n",
			numBlocksCompressed, numBlocksTotal );

	printf( "%d blocks high or low out of %d\n",
			numBlocksHighOrLow, numBlocksTotal );

	printf( "fraction of blocks compressable:  %f\n",
			( numBlocksCompressed + numBlocksHighOrLow )
			/ (float)numBlocksTotal );
	
	// for each channel and image, output the bit field
	/*for( int c=0; c<3; c++ ) {
		for( int b=0; b<8; b++ ) {
			Image *outImage =
				ImageBitFields::bitFieldsToImage( &( fields[c][b] ), 1 );
			char *outFileName = new char[99];
			sprintf( outFileName, "noiseBitField_chan%d_bit%d.bmp", c, b );

			Image *threeChannelOutImage = 
                ImageColorConverter::grayscaleToRGB( outImage );

			File *outFile = new File( NULL, outFileName,
									  strlen( outFileName ) );

			FileOutputStream *outStream =
				new FileOutputStream( outFile );

			converter->formatImage( threeChannelOutImage, outStream );
			
			delete outFile;
			delete outStream;

			delete outImage;
			delete threeChannelOutImage;
			delete [] outFileName;
			}
		}
	*/
	
	
	// convert back
	Image *outImage =
		ImageBitFields::bitFieldsToImage( fields, 3, 8 );

	File *outFile = new File( NULL, inArgs[2], strlen( inArgs[2] ) );

	FileOutputStream *outStream = new FileOutputStream( outFile );

	converter->formatImage( outImage, outStream );

	delete outFile;
	delete outStream;

	for( int i=0; i<3; i++ ) {
		for( int j=0; j<8; j++ ) {
			delete fields[i][j];
			}
		delete [] fields[i];
		}
	delete [] fields;

	delete inImage;
	delete outImage;

	delete randSource;
	
	return 0;
	}



void usage( char *inAppName ) {
	
	printf( "usage:\n" );
	printf( "%s in_file out_file complexity_hreshold use_gray\n",
			inAppName );

	printf( "examples:\n" );
	printf( "\t%s test.bmp test2.bmp 0.9 1\n", inAppName );
	printf( "\t%s test.bmp test2.bmp 0.8 0\n", inAppName );
	
	exit( 1 );
	}
