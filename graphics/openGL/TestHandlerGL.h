/*
 * Modification History
 *
 * 2000-December-19		Jason Rohrer
 * Created. 
 *
 * 2001-January-9		Jason Rohrer
 * Changed to use new Primitive3D implementations.  
 *
 * 2001-January-16		Jason Rohrer
 * Changed to use new Translate3D class for drawing primitives.
 *
 * 2001-January-30		Jason Rohrer
 * Updated to comply with new Primitive3D interface. 
 *
 * 2001-January-31		Jason Rohrer
 * Added definition of M_PI if not automatically defined.
 * Added a quit key handler.
 * Added multitexturing to central quad to test multitexture implementations.
 * Had to recommit because of lost log message.
 *
 * 2001-February-2		Jason Rohrer
 * Fixed a bug in the way textures were generated. 
 *
 * 2001-February-24		Jason Rohrer
 * Fixed incorrect delete usage.
 *
 * 2001-August-29		Jason Rohrer
 * Fixed to use new KeyboardHandler interface.  
 */
 
 
#ifndef TEST_HANDLER_GL_INCLUDED
#define TEST_HANDLER_GL_INCLUDED

#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#include "ScreenGL.h"

#include "MouseHandlerGL.h"
#include "KeyboardHandlerGL.h"
#include "SceneHandlerGL.h"

#include "PrimitiveGL.h"
#include "ObjectGL.h"
#include "minorGems/graphics/3d/LathePrimitive3D.h"
#include "minorGems/graphics/3d/LandscapePrimitive3D.h"
#include "TextureGL.h"
#include "LightingGL.h"
#include "NoLightingGL.h"
#include "DirectionLightingGL.h"
#include "MultiLightingGL.h"
#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/math/geometry/Angle3D.h"


#include "minorGems/graphics/Color.h"
#include "minorGems/graphics/RGBAImage.h"
#include "minorGems/graphics/filters/BoxBlurFilter.h"
#include "minorGems/graphics/filters/ThresholdFilter.h"
#include "minorGems/graphics/filters/InvertFilter.h"

#include "minorGems/util/random/RandomSource.h"
#include "minorGems/util/random/Noise.h"

#include "minorGems/graphics/Image.h"

/**
 * Test class for ScreenGL implementation.
 *
 * Draws a simple polygon.
 *
 * @author Jason Rohrer 
 */
class TestHandlerGL : 
	public MouseHandlerGL, 
	public KeyboardHandlerGL,
	public SceneHandlerGL { 


	public:
		
		TestHandlerGL( RandomSource *inRandSource, int inNumTriangles );
		
		~TestHandlerGL();
		
		
		/**
		 * Sets up primitives.  Must be called *after* OpenGL context is
		 * constructed.
		 */
		void setupPrimitives();
		
		
		// implement the MouseHandlerGL interface
		void mouseMoved( int inX, int inY );
		void mouseDragged( int inX, int inY );
		void mousePressed( int inX, int inY );
		void mouseReleased( int inX, int inY );
		
		// implement the KeyboardHandlerGL interface
		void keyPressed( unsigned char inKey, int inX, int inY );
		void specialKeyPressed( int inKey, int inX, int inY );
		void keyReleased( unsigned char inKey, int inX, int inY );
		void specialKeyReleased( int inKey, int inX, int inY );
		
		// implement the SceneHandlerGL interface
		void drawScene();
	
	private:
		RandomSource *mRandSource;
		
		int mNumTriangles;
			
		Vector3D ***mTriangles;
		Color ***mColors;
		
		PrimitiveGL *mPrimitive; 
		PrimitiveGL *mPrimitive2;
		PrimitiveGL *mPrimitive3;
		
		LightingGL *mLightingA;
		LightingGL *mLightingB;
		MultiLightingGL *mLighting;
		
		double mCurrentAngle;
		double mAngleStep;
	};

TestHandlerGL::TestHandlerGL( RandomSource *inRandSource, 
	int inNumTriangles ) 
	: mRandSource( inRandSource ), mNumTriangles( inNumTriangles ),
	mTriangles( new Vector3D**[inNumTriangles] ),
	mColors( new Color**[inNumTriangles] ) {
	
	mLightingA = new DirectionLightingGL( new Color( 1.0, 1.0, 1.0 ), 
		new Vector3D( 0, 0, 1 ) );
	
	mLightingB = new DirectionLightingGL( new Color( 0, 0, 1.0 ), 
		new Vector3D( 1, 1, 0 ) );
	
	mLighting = new MultiLightingGL();
	mLighting->addLighting( mLightingA );
	mLighting->addLighting( mLightingB );
	
	for( int i=0; i<mNumTriangles; i++ ) {
		mColors[i] = new Color*[3];
		mTriangles[i] = new Vector3D*[3];
		
		for( int j=0; j<3; j++ ) {
			mTriangles[i][j] = 
				new Vector3D( mRandSource->getRandomDouble() * 10,
					mRandSource->getRandomDouble() * 10 - 5,
					mRandSource->getRandomDouble() * 10 );
			mColors[i][j] =
				new Color( mRandSource->getRandomFloat(),
					mRandSource->getRandomFloat(),
					mRandSource->getRandomFloat(),
					mRandSource->getRandomFloat() );
			}		
		}
	
	} 



TestHandlerGL::~TestHandlerGL() {

	for( int i=0; i<mNumTriangles; i++ ) { 
		for( int j=0; j<3; j++ ) {
			delete mTriangles[i][j];
			delete mColors[i][j];
			}
		delete [] mColors[i];
		delete [] mTriangles[i];		
		}
	delete [] mColors;
	delete [] mTriangles;	
	
	delete mPrimitive;
	delete mPrimitive2;
	delete mPrimitive3;
	
	delete mLightingA;
	delete mLightingB;
	delete mLighting;
	}



void TestHandlerGL::setupPrimitives() {
	// make a flat rectangle object
	Vector3D **corners = new Vector3D*[4];
	corners[0] = new Vector3D( -1, 1, 0 );
	corners[1] = new Vector3D( 1, 1, 0 );
	corners[2] = new Vector3D( -1,-1, 0 );
	corners[3] = new Vector3D( 1, -1, 0 );
	
	double *anchorsX = new double[4];
	double *anchorsY = new double[4];
	
	anchorsX[0] = 0;
	anchorsX[1] = 1;
	anchorsX[2] = 0;
	anchorsX[3] = 1;
	
	anchorsY[0] = 0;
	anchorsY[1] = 0;
	anchorsY[2] = 1;
	anchorsY[3] = 1;
	
	int textSize = 64;
	double fPower = .75;
	
	
	RGBAImage *noiseImage = new RGBAImage( textSize, textSize );
	//double *redNoiseBuffer = new double[ textSize * textSize ];
	//double *greenNoiseBuffer = new double[ textSize * textSize ];
	//double *blueNoiseBuffer = new double[ textSize * textSize ];
	genFractalNoise2d( noiseImage->getChannel(0), 
		textSize, textSize, fPower, true, mRandSource );
	genFractalNoise2d( noiseImage->getChannel(1), 
		textSize, textSize, fPower, true, mRandSource );
	genFractalNoise2d( noiseImage->getChannel(2), 
		textSize, textSize, fPower, true, mRandSource );
	
	Image *selection = new Image( textSize, textSize, 1 );
	genFractalNoise2d( selection->getChannel(0), 
		textSize, textSize, fPower, true, mRandSource );	
	BoxBlurFilter *blur = new BoxBlurFilter( 10 );
	ThresholdFilter *threshold = new ThresholdFilter( 0.5 );
	InvertFilter *invert = new InvertFilter();
	selection->filter( threshold );
	
	noiseImage->setSelection( selection );
	//noiseImage->filter(invert);
	
	delete blur;
	delete threshold;
	delete invert;
	
	noiseImage->clearSelection();
	delete selection;
	/*for( int t=0; t<textSize * textSize; t++ ) {
		unsigned char red = (unsigned char)( redNoiseBuffer[t] * 255 );
		unsigned char green = (unsigned char)( greenNoiseBuffer[t] * 255 );
		unsigned char blue = (unsigned char)( blueNoiseBuffer[t] * 255 );
		textureCharBuff[ t*4 ] = red;
		textureCharBuff[ t*4 + 1 ] = green;
		textureCharBuff[ t*4 + 2 ] = blue;
		textureCharBuff[ t*4 + 3 ] = 255;
		}
	*/
	
	
	
	//genFractalNoise2d( textureBuff, textSize, textSize );
	
	RGBAImage **imageArray = new RGBAImage*[2];
	imageArray[0] = noiseImage;
	imageArray[1] = noiseImage->copy();
	
	//double *alpha = imageArray[1]->getChannel(3);
	//int numPixels = imageArray[1]->getWidth() * imageArray[1]->getHeight();
	//for( int p=0; p<numPixels; p++ ) {
	//	alpha[p] = 0.25;
	//	}
	
	double **anchorArrayX = new double*[2];
	anchorArrayX[0] = anchorsX;
	anchorArrayX[1] = new double[4];
	
	anchorArrayX[1][0] = 0;
	anchorArrayX[1][1] = 5;
	anchorArrayX[1][2] = 0;
	anchorArrayX[1][3] = 5;
	
	
	double **anchorArrayY = new double*[2];
	anchorArrayY[0] = anchorsY;
	
	anchorArrayY[1] = new double[4];
	
	anchorArrayY[1][0] = 0;
	anchorArrayY[1][1] = 0;
	anchorArrayY[1][2] = 5;
	anchorArrayY[1][3] = 5;
	
	//Primitive3D *simplePrimitive3D = 
	//	new Primitive3D( 2, 2, corners, 1, &noiseImage, &anchorsX, &anchorsY );
	
	Primitive3D *simplePrimitive3D = 
		new Primitive3D( 2, 2, corners, 2, imageArray, 
			anchorArrayX, anchorArrayY );
	
	
	
	mPrimitive = new PrimitiveGL( simplePrimitive3D );
	
	//mPrimitive = new LathePrimitiveGL( 4, latheCurve, 5, 2 * M_PI, texture );
	
	//mPrimitive->setTransparent( true );
	
	// all passed in args will be destroyed when primitive is destroyed
	
	
	// setup second primitive
	Vector3D **latheCurve = new Vector3D*[4];
	latheCurve[0] = new Vector3D( 0.5, 1, 0 );
	latheCurve[1] = new Vector3D( 1, 0.5, 0 );
	latheCurve[2] = new Vector3D( 1, -0.5, 0 );
	latheCurve[3] = new Vector3D( 0.5, -1, 0 );
	
	noiseImage = new RGBAImage( textSize, textSize );
	genFractalNoise2d( noiseImage->getChannel(0), 
		textSize, textSize, fPower, true, mRandSource );
	genFractalNoise2d( noiseImage->getChannel(1), 
		textSize, textSize, fPower, true, mRandSource );
	genFractalNoise2d( noiseImage->getChannel(2), 
		textSize, textSize, fPower, true, mRandSource );

	Primitive3D *lathePrimitive3D = 
		new LathePrimitive3D( 4, latheCurve, 15, 2 * M_PI, noiseImage );
	
	lathePrimitive3D->setTransparent( false );
	
	mPrimitive2 = new PrimitiveGL( lathePrimitive3D );
	//mPrimitive2->setBackVisible( true );
	
	// all passed in args will be destroyed when primitive is destroyed
	
	
	noiseImage = new RGBAImage( textSize, textSize );
	genFractalNoise2d( noiseImage->getChannel(0), 
		textSize, textSize, fPower, true, mRandSource );
	genFractalNoise2d( noiseImage->getChannel(1), 
		textSize, textSize, fPower, true, mRandSource );
	genFractalNoise2d( noiseImage->getChannel(2), 
		textSize, textSize, fPower, true, mRandSource );
	
	int landSize = 20;
	unsigned long *intHeights = new unsigned long[ landSize * landSize ];
	genFractalNoise2d( intHeights, landSize, landSize );
	double *heights = new double[ landSize * landSize ];
	for( int l=0; l<landSize*landSize; l++ ) {
		// take one color component as the height
		heights[l] = (double)( intHeights[l] & 0xFF ) / 655;
		//heights[l] = 0;
		}
	
	
	LandscapePrimitive3D *landscapePrimitive3D = 
		new LandscapePrimitive3D( landSize, landSize, heights, noiseImage );
	
	mPrimitive3 = new PrimitiveGL( landscapePrimitive3D );
	
	mCurrentAngle = 0;
	mAngleStep = 0.01;
	}



void TestHandlerGL::mouseMoved( int inX, int inY ) {
	}

void TestHandlerGL::mouseDragged( int inX, int inY ) {
	}

void TestHandlerGL::mousePressed( int inX, int inY ) {
	}

void TestHandlerGL::mouseReleased( int inX, int inY ) {
	}
		
void TestHandlerGL::keyPressed( unsigned char inKey, int inX, int inY ) {
	if( inKey == 'q' || inKey == 'Q' ) {
		exit( 0 );
		}
	}

void TestHandlerGL::specialKeyPressed( int inKey, int inX, int inY ) {
	}



void TestHandlerGL::keyReleased( unsigned char inKey, int inX, int inY ) {
	}



void TestHandlerGL::specialKeyReleased( int inKey, int inX, int inY ) {
	}


		
void TestHandlerGL::drawScene() {	
/*	glDisable( GL_TEXTURE_2D );
	
	glBegin( GL_TRIANGLES );
		for( int p=0; p<mNumTriangles; p++ ) { 

		 
			for( int v=0; v<3; v++ ) {
	    		glColor4f( mColors[p][v]->r, 
					mColors[p][v]->g, mColors[p][v]->b, mColors[p][v]->a );
				glVertex3f( mTriangles[p][v]->mX, 
					mTriangles[p][v]->mY, mTriangles[p][v]->mZ );
	    		}
		
			}
	glEnd();
*/
	Vector3D *pos = new Vector3D( 0, 0, 10 );
	Angle3D *rot = new Angle3D( mCurrentAngle, mCurrentAngle, 0 );
	//Angle3D *rot = new Angle3D( 0, 0, 0 );
	Transform3D *trans = new Transform3D();
	trans->scale( 5 );
	trans->rotate( rot );
	trans->translate( pos );
	
	
	//mPrimitive->setBackVisible( false );
	mPrimitive->draw( trans, mLighting );
	delete rot;
	delete pos;
	delete trans;
	
	pos = new Vector3D( 0, -10, 10 );
	rot = new Angle3D( 0, mCurrentAngle, 0 );
	trans = new Transform3D();
	trans->scale( 20 );
	trans->rotate( rot );
	trans->translate( pos );
	
	mPrimitive3->draw( trans, mLighting );
	delete rot;
	delete pos;
	delete trans;
	
	pos = new Vector3D( 0, 5, 0 );
	rot = new Angle3D( M_PI, mCurrentAngle, 0 );
	trans = new Transform3D();
	trans->scale( 20 );
	trans->rotate( rot );
	trans->translate( pos );
	
	//mPrimitive3->setBackVisible( false );
	//mPrimitive3->draw( trans, mLighting );
	delete rot;
	delete pos;
	delete trans;
	
	mCurrentAngle += mAngleStep;	
	}
	
#endif	
