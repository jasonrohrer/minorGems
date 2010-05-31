/*
 * Modification History
 *
 * 2001-May-12   Jason Rohrer
 * Created.
 * Fixed a bug with double input.
 * Changed to rotate by half view angle when looking
 * for a waypoint.
 *
 * 2001-May-13   Jason Rohrer
 * Added support for learning waypoint colors.
 * Fixed several bugs.
 *
 * 2001-May-14   Jason Rohrer
 * Changed to use HSB hue instead of RGB for waypoint
 * identification.
 * Added support for output files.
 *
 * 2001-May-15   Jason Rohrer
 * Added support for ignoring pixels based on a saturation threshold.
 * Added a blur filter to the incoming images.
 *
 * 2001-May-17   Jason Rohrer
 * Added some timing output messages.
 */
 
 
#ifndef WAYPOINT_MOVE_GENERATOR_INCLUDED
#define WAYPOINT_MOVE_GENERATOR_INCLUDED 

#include "StereoMoveGenerator.h"
#include "MoveProcessor.h"
#include "ImageUtilities.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/Color.h"
#include "minorGems/graphics/filters/MedianFilter.h"
#include "minorGems/graphics/filters/BoxBlurFilter.h"

#include "minorGems/graphics/ImageColorConverter.h"
#include "ImageStatistics.h"
#include "ImageRectangle.h"

#include "minorGems/system/Thread.h"

#include <stdio.h>
#include <sys/timeb.h>

/**
 * A move generator that travels through a series of
 * color waypoints.  Uses only 2d images, not stereo.
 * Only processes the left image.
 *
 * @author Jason Rohrer
 */ 
class WaypointMoveGenerator : public StereoMoveGenerator {
	
	public:
		
		/**
		 * Constructs a move generator.
		 *
		 * @param inConfigFile the file to read configuration
		 *   information from.  Must be closed by caller.
		 *   Can be closed immediately after the constructor
		 *   returns.
		 * @param inOutputFiles true if image files should be output
		 *   to disk.
		 */
		WaypointMoveGenerator( FILE*inConfigFile, char inOutputImageFiles );
		
		~WaypointMoveGenerator();
		
		// implements StereoMoveGenerator
		virtual void generateMoves( 
			Image *inLeftImage, Image *inRightImage, 
			Image *inStereoData, MoveProcessor *inMoveProcessor );	
	
	private:
		char mInitialized;

		char mOutputFiles;
		
		// our view angle as a fraction of a full rotation
		// a value of 0.5 represents a 180 degree view angle
		double mViewAngle;
		
		double mTolerance;
		double mSaturationThreshold;
		
		double mSeeWaypointFraction;
		double mAtWaypointFraction;
		int mLoop;

		int mLearnWaypointColors;
		int mLearningStartDelay;
		int mBetweenColorLearningDelay;
		
		char mNumWaypoints;
		// a hue value for each waypoint
		double *mWaypoints;

		int mCurrentWaypoint;


		int mLearningWaypointColors;
		int mCurrentLearningWaypoint;
		
		/**
		 * Reads our configuration values from file.
		 *
		 * @param inConfigFile the file to read values from.
		 *   Must be destroyed by caller.
		 *
		 * @return true iff all values were read succesfully.
		 */
		char readConfigFile( FILE *inConfigFile );
		
	};
	
	
	
inline WaypointMoveGenerator::WaypointMoveGenerator(
	FILE *inConfigFile,
	char inOutputImageFiles )
	: mInitialized( false ), mOutputFiles( inOutputImageFiles ) {
	
	if( !readConfigFile( inConfigFile ) ) {
		printf( "WaypointMoveGenerator failed to read config file.\n" );
		}
	else {
		mInitialized = true;

		if( mLearnWaypointColors ) {
			mLearningWaypointColors = true;
			mCurrentLearningWaypoint = 0;
			}
		}
	}



inline WaypointMoveGenerator::~WaypointMoveGenerator() {

	if( mInitialized ) {
		delete [] mWaypoints;
		}
	
	}	
	
		
		
inline void WaypointMoveGenerator::generateMoves( 
	Image *inLeftImage, Image *inRightImage, 
	Image *inStereoData, MoveProcessor *inMoveProcessor ) {

	struct timeb timeStruct;

    ftime( &timeStruct );
    printf("about to do median, time = %ld.%d\n",
		   timeStruct.time, timeStruct.millitm );
  
	MedianFilter *filter = new MedianFilter( 2 );
	//BoxBlurFilter *filter = new BoxBlurFilter( 2 );
	inLeftImage->filter( filter );

	ftime( &timeStruct );
    printf("about to do RGB->HSB, time = %ld.%d\n",
		   timeStruct.time, timeStruct.millitm );
	
	delete filter;
	
	Image *hsbImage = ImageStatistics::RGBtoHSB( inLeftImage );
	
	// handle the case where we're learning
	if( mLearningWaypointColors ) {
		// the image coming in is for the last waypoint
		int lastWaypoint = mCurrentLearningWaypoint - 1;

		if( lastWaypoint < 0 ) {
			// first step in the learning phase
			Thread::sleep( mLearningStartDelay * 1000 );

			}
		else if( lastWaypoint >= 0 ) {
			Image *ignoreMask = new Image( hsbImage->getWidth(),
										   hsbImage->getHeight(),
										   1 );
			ImageStatistics::thresholdMap( hsbImage,
										   mSaturationThreshold,
										   1,
										   ignoreMask );
			
			// look at the inLeftImage to learn this waypoint
			ImageRectangle *centerRect =
				new ImageRectangle( 0.45, 0.55, 0.45, 0.55 );

			// find the average hue of the image center
			double aveHue =
				ImageStatistics::averageValue( hsbImage, 0,
											   centerRect,
											   ignoreMask );
			delete centerRect;

			mWaypoints[lastWaypoint] = aveHue;

			// print the learned color
			printf( "Waypoint %d learned as hue:   %f\n",
					lastWaypoint, aveHue );

			if( mOutputFiles ) {
				char *filePrefix = new char[100];

				// output RGB image
				sprintf( filePrefix,
						 "waypoint%dExampleRGB__", lastWaypoint );
				ImageUtilities::imageToBMPFile( inLeftImage, filePrefix );

				// output HSB image
				sprintf( filePrefix, "waypoint%dExampleHSB__", lastWaypoint );
				ImageUtilities::imageToBMPFile( hsbImage, filePrefix );

				delete [] filePrefix;
				}

			delete ignoreMask;
			}

		mCurrentLearningWaypoint++;
		
		if( mCurrentLearningWaypoint > mNumWaypoints ) {
			// we're done learning
			mLearningWaypointColors = false;
			}
		else {
			// still learning

			if( mCurrentLearningWaypoint != 1 ) {
				// flash once to tell trainer we're done with
				// the sampling of the last waypoint
				inMoveProcessor->flashLight( 1 );
				}
			
			// delay before sending flashes
			Thread::sleep( mBetweenColorLearningDelay * 1000 );
			
			// send flashes for the next waypoint
			// send 1 + the waypoint number so that we can specify
			// waypoint 0
			inMoveProcessor->flashLight( mCurrentLearningWaypoint );
			}

		// don't go on with other move code if we're learning
		delete hsbImage;
		return;
		}

	
	// if we reached this point without returning,
	// we are out of the learning phase
	
	
	// make sure we're init'ed before accessing members
	if( !mInitialized ) {
		printf( "WaypointMoveGenerator not initialized\n" );
		}
	else {

		ftime( &timeStruct );
		printf("about to do ignore mask, time = %ld.%d\n",
			   timeStruct.time, timeStruct.millitm );
		
		Image *ignoreMask = new Image( hsbImage->getWidth(),
									   hsbImage->getHeight(),
									   1 );
		ImageStatistics::thresholdMap( hsbImage,
									   mSaturationThreshold,
									   1,
									   ignoreMask );

		ftime( &timeStruct );
		printf("about to do fraction near value, time = %ld.%d\n",
			   timeStruct.time, timeStruct.millitm );
		
		double fraction =
			ImageStatistics::fractionNearValue(
				hsbImage,
				0,
				mWaypoints[ mCurrentWaypoint ],
				mTolerance, ignoreMask );

		printf( "fraction = %f\n", fraction );

		if( mOutputFiles ) {
			char *filePrefix = new char[100];

			// output RGB image
			sprintf( filePrefix,
					 "waypoint%dSearchRGB__", mCurrentWaypoint );
			ImageUtilities::imageToBMPFile( inLeftImage, filePrefix );

			// output HSB image
			sprintf( filePrefix,
					 "waypoint%dSearchHSB__", mCurrentWaypoint );
			ImageUtilities::imageToBMPFile( hsbImage, filePrefix );
			
			delete [] filePrefix;
			}

		ftime( &timeStruct );
		printf("about to do send a move, time = %ld.%d\n",
			   timeStruct.time, timeStruct.millitm );
		
		if( fraction > mAtWaypointFraction ) {
			// we've made it to a waypoint
			// flash a light to say that we're here
			inMoveProcessor->flashLight( mCurrentWaypoint + 1 );
				
			printf( "Reached waypoint %d\n", mCurrentWaypoint );
			
			// move onto the next waypoint
			if( mCurrentWaypoint < mNumWaypoints - 1 ) {
				
				mCurrentWaypoint++;
				}
			else {
				// out of waypoints
				if( mLoop ) {
					printf( "Looping back to waypoint 0\n" );
					// start over
					mCurrentWaypoint = 0;
					}
				else {
					// were done, so do nothing
					printf( "Reached final waypoint, so stopping.\n" );
					}
				}					
			}
		else if( fraction > mSeeWaypointFraction ) {
			// we can see the waypoint, but we're not there yet

			double centerX;
			double centerY;

			// find the center of the visible waypoint color
			ImageStatistics::centerOfValue(
				hsbImage,
				0,
				mWaypoints[ mCurrentWaypoint ],
				mTolerance,
				&centerX, &centerY, ignoreMask );

			double viewLocationOfCenter = ( centerX - 0.5 );

			double angleToRotate =
				mViewAngle * viewLocationOfCenter;

			// if we'd be rotating more than 1/100th of a rotation
			if( angleToRotate > 0.01 || angleToRotate < -0.01 ) {
				printf( "Rotating by %f to center waypoint\n",
						angleToRotate );
				inMoveProcessor->rotateClockwise( angleToRotate );
				}
			else {
				// our view is basically centered on our waypoint,
				// so roll towards it
				printf(
					"Moving forward 1/10 meter towards waypoint\n" );
				inMoveProcessor->translateForward( 1.0 );
				}
			}
		else {
			// we can't even see the waypoint.

			printf( "Rotating %f to look for waypoint\n",mViewAngle / 2.0 );
		
			// rotate by 90 to look for it
			inMoveProcessor->rotateClockwise( mViewAngle / 2.0 );
			}

		ftime( &timeStruct );
		printf("done sending a move, time = %ld.%d\n",
			   timeStruct.time, timeStruct.millitm );
		
		delete ignoreMask;
		}
	
	delete hsbImage;
	}	



inline char WaypointMoveGenerator::readConfigFile(
	FILE *inConfigFile ) {

	int numRead = fscanf( inConfigFile, "%lf", &mViewAngle );
	if( numRead != 1 ) {
		return 0;
		}
	mViewAngle = mViewAngle / 360.0;

	
	numRead = fscanf( inConfigFile, "%lf", &mTolerance );
	if( numRead != 1 ) {
		return 0;
		}

	
	numRead = fscanf( inConfigFile, "%lf", &mSaturationThreshold );
	if( numRead != 1 ) {
		return 0;
		}

	
	numRead = fscanf( inConfigFile, "%lf", &mSeeWaypointFraction );
	if( numRead != 1 ) {
		return 0;
		}

	
	numRead = fscanf( inConfigFile, "%lf", &mAtWaypointFraction );
	if( numRead != 1 ) {
		return 0;
		}
	

	numRead = fscanf( inConfigFile, "%d", &mLoop );
	if( numRead != 1 ) {
		return 0;
		}


	numRead = fscanf( inConfigFile, "%d", &mLearnWaypointColors );
	if( numRead != 1 ) {
		return 0;
		}

	
	numRead = fscanf( inConfigFile, "%d", &mLearningStartDelay );
	if( numRead != 1 ) {
		return 0;
		}

	
	numRead = fscanf( inConfigFile, "%d", &mBetweenColorLearningDelay );
	if( numRead != 1 ) {
		return 0;
		}
	
	
	numRead = fscanf( inConfigFile, "%d", &mNumWaypoints );
	if( numRead != 1 ) {
		return 0;
		}

	
	mWaypoints = new double[ mNumWaypoints ];

	for( int i=0; i<mNumWaypoints; i++ ) {
		double hue;

		numRead = fscanf( inConfigFile, "%lf", &hue );
		if( numRead != 1 ) {
			return 0;
			}
		
		mWaypoints[i] = hue;
		}
	
	// success
	return 1;
	}

 
		
#endif
