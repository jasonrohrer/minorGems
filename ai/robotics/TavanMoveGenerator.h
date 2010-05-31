/*
 * Modification History
 *
 * 2001-May-2		Jeremy Tavan	
 * Created. 
 *
 * 2001-May-7           Jeremy Tavan
 * Changed to operate on greyscale conversions of 
 * original images.
 *
 * 2001-May-8   Jason Rohrer
 * Changed to move forward by 1 decimeter instead of 10.
 * 
 * 2001-May-10  Jeremy Tavan
 * Added a minimum threshold of 10% moving pixels before the robot
 * decides to move.
 *
 * 2001-May-10   Jason Rohrer
 * Changed to compute a difference between images slices to determine
 * when nothing in the visual field is moving.
 *
 * 2001-May-12   Jason Rohrer
 * Changed so that no move is generated at all when
 * waiting for the next image (no longer generates a nop).
 */
 
#ifndef TAVAN_MOVE_GENERATOR_INCLUDED
#define TAVAN_MOVE_GENERATOR_INCLUDED 

#include "StereoMoveGenerator.h"
#include "MoveProcessor.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ImageColorConverter.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"
#include "minorGems/graphics/converters/BMPImageConverter.h"

#include "ImageStatistics.h"


/**
 * Tavan's implementation of the move generator.
 *
 * Detects motion using a similar algorithm to that used by the MIT Cog vision
 * system.  Converts to greyscale, calculates threshold value as the mean grey level 
 * difference between two consecutive images.  Flags squares as moving if the 
 * delta exceeds the threshold value.  Further divides into three regions: 
 * "ahead", "left", and "right", and aims the robot at whichever has the largest
 * number of moving squares.
 *
 * @author Jeremy Tavan
 */
class TavanMoveGenerator : public StereoMoveGenerator {
	
	public:
		
		/**
		 * Constructs a move generator.
		 *
		 */
		TavanMoveGenerator( );
		
		~TavanMoveGenerator();
		
		// implements StereoMoveGenerator
		virtual void generateMoves( 
			Image *inLeftImage, Image *inRightImage, 
			Image *inStereoData, MoveProcessor *inMoveProcessor );	
	
	private:
		double mMoveThreshold;
		
		ImageRectangle *mLeftRect;
		ImageRectangle *mRightRect;
		ImageRectangle *mCenterRect;

		Image *mImageBuffer;

		int mHaveTwoImages;
	};
	
	
	
inline TavanMoveGenerator::TavanMoveGenerator() 
      : mLeftRect( new ImageRectangle( 0, 0.333, 0, 1 ) ),
	mRightRect( new ImageRectangle( 0.667, 1, 0, 1 ) ),
	mCenterRect( new ImageRectangle( 0.333, 0.666, 0, 1 ) )
{
  mHaveTwoImages = 0;
}



inline TavanMoveGenerator::~TavanMoveGenerator() {

	delete mLeftRect;
	delete mRightRect;
	delete mCenterRect;
        }	
	
		
		
inline void TavanMoveGenerator::generateMoves( 
	Image *inLeftImage, Image *inRightImage, 
	Image *inStereoData, MoveProcessor *inMoveProcessor ) {
  if (!mHaveTwoImages) {
    mImageBuffer = ImageColorConverter::RGBToGrayscale( inLeftImage );
    mHaveTwoImages = 1;
	
	// note that we don't actually have to send the move
	// processor a move before returning.
	// this should speed things up a bit.
	// inMoveProcessor->nop();
	
  } else {
    double leftFraction, rightFraction, centerFraction;

	// Prepare to write difference map to file
	BMPImageConverter *converter = new BMPImageConverter();

        char *fileName = new char[100];

        long timeStamp = time( NULL );
        sprintf( fileName, "diff%d.bmp", timeStamp );
        int nameLength = strlen( fileName );

        File *diffOutFile = new File( NULL, fileName, nameLength );
        FileOutputStream *diffOutStream = new FileOutputStream( diffOutFile );


    Image *LeftGrey = ImageColorConverter::RGBToGrayscale( inLeftImage );

    Image *diffMap = new Image(LeftGrey->getWidth(), LeftGrey->getHeight(), 1);

    double meanDiff = ImageStatistics::differenceMap( mImageBuffer, LeftGrey, diffMap );
	 Image *threeChannelImage =
                ImageColorConverter::grayscaleToRGB( diffMap );

        converter->formatImage( threeChannelImage, diffOutStream );

	delete converter;
	delete [] fileName;
	delete diffOutFile;
	delete diffOutStream;
	delete threeChannelImage;

    printf("mean difference is %f", meanDiff);
    leftFraction = ImageStatistics::fractionOverThreshold( diffMap, 0, mLeftRect, meanDiff );
    printf( "Left side fraction of moving pixels = %f\n", leftFraction );
    centerFraction = ImageStatistics::fractionOverThreshold( diffMap, 0, mCenterRect, meanDiff );
    printf( "Center fraction of moving pixels = %f\n", centerFraction );
    rightFraction = ImageStatistics::fractionOverThreshold( diffMap, 0, mRightRect, meanDiff );
    printf( "Right side fraction of moving pixels = %f\n", rightFraction );


	// decide whether there is motion in our visual field

	// compute the difference between the motion fractions in
	// the 3 screen slices
	double leftCenterDiff = leftFraction - centerFraction;
	double centerRightDiff = centerFraction - rightFraction;
	double rightLeftDiff = rightFraction - leftFraction;

	double totalDiff =
		fabs( leftCenterDiff ) +
		fabs( centerRightDiff ) +
		fabs( rightLeftDiff );
	
	
    if ( totalDiff < 0.3 ) {
		printf("Nothing seems to be moving enough.  I'm rotating 120 degrees left.\n");
		inMoveProcessor->rotateClockwise( -0.3333 );
    } else if ((leftFraction > centerFraction) && (leftFraction > rightFraction)) {
      printf( "Rotating left\n" );
      inMoveProcessor->rotateClockwise( -0.125 );
    } else if ((rightFraction > centerFraction) && (rightFraction > leftFraction)) {
      printf( "Rotating right\n" );
      inMoveProcessor->rotateClockwise( 0.125 );
    } else {
      printf( "Moving forward 1/10 meter\n" );
      inMoveProcessor->translateForward( 1.0 );
    }
    mHaveTwoImages = 0;
    delete diffMap;
    delete LeftGrey;
    delete mImageBuffer;
    
  }	
}
	
		
#endif
