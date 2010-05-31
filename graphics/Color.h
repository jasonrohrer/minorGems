// Jason Rohrer
// Color.h

/**
*
*	Color object for RadiosGL
*
*	Float components in RGB (0..1)
*	Color of emitted energy can be greater than 1
*
*
*	Created 9-5-99
*	Mods:
*		Jason Rohrer	11-6-99			Added 32-bit composite member
*		Jason Rohrer	11-13-99		Added printing functionality
*		Jason Rohrer	11-15-99		Added rebuildComposite function
*											Added weightColor function
*											Added getMax function
*		Jason Rohrer	2004-June-12	Fixed a bug in copy function.
*		Jason Rohrer	2004-June-12	Added a linear sum function.
*
*       Jason Rohrer    2004-August-12  Optimized Color constructor.
*       Jason Rohrer    2004-August-27  Added an equality test.
*       Jason Rohrer    2005-February-4 Added weighting and setValue functions.
*       Jason Rohrer    2005-February-10  Added invert and saturate functions.
*       Jason Rohrer    2005-February-11  Added HSV conversion function.
*       Jason Rohrer    2005-April-5  Disabled default building of composite.
*       Jason Rohrer    2006-August-29  Changed alpha default to 1.
*       Jason Rohrer    2006-October-3  Disabled building composite in
*                                       default Color() constuctor.
*       Jason Rohrer    2008-October-23 Added alpha to print(), and set alpha 
*                                       to 1 in default constructor.
*       Jason Rohrer    2010-April-3    Added reverse HSV function. 
*/

#ifndef COLOR_INCLUDED
#define COLOR_INCLUDED

#include <stdio.h>
#include <float.h>

class Color {
	public :
        /**
         * Constructs a color.
         *
         * @param red, green, blue the components of the color, each in [0,1].
         * @param alpha the alpha value in [0,1].  Defaults to 1.
         * @param inBuildComposite set to true to build the composite
         *   upon construction, or false to skip building composite (faster).
         *   Defaults to false.
         */
		Color(float red, float green, float blue, float alpha=1,
              char inBuildComposite=false );


        
        /**
         * Constructs a all-zero (except alpha set to 1) color with no 
         * composite built.
         */
        Color();

        

        /**
         * Constructs an rgb color from HSV components.
         *
         * @param inHue, inSaturation, inValue the HSV components of the
         *   color, each in [0,1].
         * @param alpha the alpha value in [0,1].  Defaults to 1.
         * @param inBuildComposite set to true to build the composite
         *   upon construction, or false to skip building composite (faster).
         *   Defaults to true.
         *
         * @return an RGBA color equivalent to the HSV color.
         *   Must be destroyed by caller.
         */
        static Color *makeColorFromHSV(
            float inHue, float inSaturation, float inValue,
            float inAlpha=1, char inBuildComposite=false ); 


        /**
         * Makes HSV values (each in range 0..1) from this RGB color.
         *
         * @param outH, outS, outV pointers to where values should be returned.
         */
        void makeHSV( float *outH, float *outS, float *outV );
        

        
		float r, g, b, a;

        char mCompositeBuilt;
		unsigned long composite;		// 32-bit composite color

        
		Color *copy();	// make a copy of this color


        
        /**
         * Sets the RGBA values of this color.
         *
         * @param red, green, blue, alpha the values to set.
         *   Alpha defaults to 0.
         */
        void setValues( float red, float green, float blue, float alpha=1 );

        
        
        /**
         * Sets the RGBA values of this color using the values from
         * another color.
         *
         * @param inOtherColor the color to copy values from.
         *   Must be destroyed by caller.
         */
        void setValues( Color *inOtherColor ); 

        
        
        /**
         * Tests whether this color is equal to another color.
         *
         * @param inOtherColor the other color.
         *   Must be destroyed by caller.
         *
         * @return true if they are equal, or false otherwise.
         */
        char equals( Color *inOtherColor );

        
		void print();


		/**
		 * Computes the linear weighted sum of two colors.
		 *
		 * @param inFirst the first color.
		 * @param inSecond the second color.
		 * @param inFirstWeight the weight given to the first color in the
		 *   sum.  The second color is weighted (1-inFirstWeight).
		 *
		 * @return the sum color.  Must be destroyed by caller.
		 */
		static Color *linearSum( Color *inFirst, Color *inSecond,
			float inFirstWeight );

        
		// after adjusting the r, g, b, a values exterally
		// call this to remake the composite unsigned long
		unsigned long rebuildComposite();
		
		// get largest component of R,G,B
		float getMax();
		
		// alter color data by multiplying by weight
		void weightColor( float weight );

        
        
        /**
         * Alters color data by multiplying by a weight color.
         *
         * @param inWeightColor the color to multiply this color by.
         *   Must be destroyed by caller.
         */
		void weightColor( Color *inWeightColor );

        

        /**
         * Inverts this color.
         *
         * Ignores alpha channel.
         */
        void invert();

        

        /**
         * Saturates this color, ensuring that at most 2 components are
         * non-zero.
         *
         * Ignores alpha channel.
         */
        void saturate();

        
        
		// get component by component weighted 32-bit composite
		// (returns alpha unweighted)
		unsigned long getWeightedComposite( float weight );		// from this color
		unsigned long getWeightedComposite(unsigned long c1, float weight );	// from composite
		
		unsigned long sumComposite(unsigned long c1, unsigned long c2);
		
		// access this color as a three vector
		float &operator[](int rgbIndex);
        
	};
	
	
inline Color::Color() {
	r = 0;
	g = 0;
	b = 0;
	a = 1;

    mCompositeBuilt = false;
	composite = 0;
	}



inline Color::Color(float red, float green, float blue, float alpha,
                    char inBuildComposite ) {
	r = red;
	g = green;
	b = blue;
	a = alpha;

    if( inBuildComposite ) {
        rebuildComposite();
        }
    else {
        composite = 0;
        mCompositeBuilt = false;
        }
	}



inline Color *Color::makeColorFromHSV(
    float inHue, float inSaturation, float inValue,
    float inAlpha, char inBuildComposite ) {

    // based on pseudocode from http://www.easyrgb.com/math.php
    float r, g, b;
    
    if ( inSaturation == 0 ) {
        r = inValue;                      
        g = inValue;
        b = inValue;
        }
    else {
        float var_h = inHue * 6;
        
        // H must be < 1
        if( var_h == 6 ) {
            var_h = 0;
            }
        
        float var_i = int( var_h );             // Or var_i = floor( var_h )
        float var_1 = inValue * ( 1 - inSaturation );
        float var_2 = inValue * ( 1 - inSaturation * ( var_h - var_i ) );
        float var_3 =
            inValue * ( 1 - inSaturation * ( 1 - ( var_h - var_i ) ) );
        
        if( var_i == 0 ) {
            r = inValue;
            g = var_3;
            b = var_1;
            }
        else if( var_i == 1 ) {
            r = var_2;
            g = inValue;
            b = var_1;
            }
        else if( var_i == 2 ) {
            r = var_1;
            g = inValue;
            b = var_3;
            }
        else if( var_i == 3 ) {
            r = var_1;
            g = var_2;
            b = inValue;
            }
        else if( var_i == 4 ) {
            r = var_3;
            g = var_1;
            b = inValue;
            }
        else {
            r = inValue;
            g = var_1;
            b = var_2;
            }
        }

    return new Color( r, g, b, inAlpha, inBuildComposite );
    }



inline void Color::makeHSV( float *outH, float *outS, float *outV ) {
    
    // based on pseudocode from http://www.easyrgb.com/math.php

    //Min. value of RGB
    float var_Min = r;
    if( g < var_Min ) {
        var_Min = g;
        }
    if( b < var_Min ) {
        var_Min = b;
        }
    
    //Max. value of RGB
    float var_Max = r;
    if( g > var_Max ) {
        var_Max = g;
        }
    if( b > var_Max ) {
        var_Max = b;
        }

    //Delta RGB value
    float del_Max = var_Max - var_Min;             


    //HSV results from 0 to 1
    float H;
    float S;
    float V = var_Max;
    
    if ( del_Max == 0 ) {
        //This is a gray, no chroma...
        H = 0;
        S = 0;
        }
    else {
        //Chromatic data...
        S = del_Max / var_Max;
        

        float del_R = 
            ( ( ( var_Max - r ) / 6 ) + ( del_Max / 2 ) ) / del_Max;
        
        float del_G = 
            ( ( ( var_Max - g ) / 6 ) + ( del_Max / 2 ) ) / del_Max;
        
        float del_B = 
            ( ( ( var_Max - b ) / 6 ) + ( del_Max / 2 ) ) / del_Max;
        

        if( r == var_Max )  {
            H = del_B - del_G;
            }
        else if( g == var_Max ) {
            H = ( 1.0f / 3.0f ) + del_R - del_B;
            }
        else if ( b == var_Max ) {
            H = ( 2.0f / 3.0f ) + del_G - del_R;    
            }
        

        if( H < 0 ) {
            H += 1;
            }
        
        if( H > 1 ) {
            H -= 1;
            }
        }
    
    
    *outH = H;
    *outS = S;
    *outV = V;
    }




inline Color *Color::copy() {
	Color *copyColor = new Color(r,g,b,a, mCompositeBuilt );
	return copyColor;
	}



inline void Color::setValues( float red, float green,
                              float blue, float alpha ) {
    r = red;
    g = green;
    b = blue;
    a = alpha;
    
    if( mCompositeBuilt ) {
        rebuildComposite();
        }
    }



inline void Color::setValues( Color *inOtherColor ) {
    setValues( inOtherColor->r,
               inOtherColor->g,
               inOtherColor->b,
               inOtherColor->a );
    }



inline char Color::equals( Color *inOtherColor ) {
    if( r == inOtherColor->r &&
        g == inOtherColor->g &&
        b == inOtherColor->b &&
        a == inOtherColor->a ) {
        return true;
        }
    else {
        return false;
        }
    }

inline void Color::print() {
	printf( "(%f, %f, %f, %f)", r, g, b, a );
	}



inline Color *Color::linearSum( Color *inFirst, Color *inSecond,
	float inFirstWeight ) {
	
	float secondWeight = 1 - inFirstWeight;
	float r = inFirstWeight * inFirst->r + secondWeight * inSecond->r;
	float g = inFirstWeight * inFirst->g + secondWeight * inSecond->g;
	float b = inFirstWeight * inFirst->b + secondWeight * inSecond->b;
    float a = inFirstWeight * inFirst->a + secondWeight * inSecond->a;
	
	return new Color( r, g, b, a,
                      inFirst->mCompositeBuilt || inSecond->mCompositeBuilt );
	}



inline unsigned long Color::rebuildComposite() {
	composite = ((int)(b*255)) | ((int)(g*255)) << 8 | ((int)(r*255)) << 16 |
        ((int)(a*255)) << 24;
    mCompositeBuilt = true;
	return composite;
	}

inline float Color::getMax() {
	float max = -FLT_MAX;
	if( r > max ) max = r;
	if( g > max ) max = g;
	if( b > max ) max = b;
	return max;
	}



inline void Color::weightColor( float weight ) {
	r = r * weight;
	g = g * weight;
	b = b * weight;
	// for now, don't touch alpha

    if( mCompositeBuilt ) {
        rebuildComposite();
        }
	}	



inline void Color::invert() {
    r = 1 - r;
    g = 1 - g;
    b = 1 - b;
    }



inline void Color::saturate() {
    if( r < g && r < b ) {
        r = 0;
        }
    else if( g < r && g < b ) {
        g = 0;
        }
    else if( b < r && b < g ) {
        b = 0;
        }
    else if( r != 0 ) {
        // they are all equal, but non-zero

        // default to dropping red
        r = 0;
        }
    //else
    // they are all 0
    // leave as black
    }



inline void Color::weightColor( Color *inWeightColor ) {
    r *= inWeightColor->r;
    g *= inWeightColor->g;
    b *= inWeightColor->b;
    a *= inWeightColor->a;

    if( mCompositeBuilt ) {
        rebuildComposite();
        }
    }



inline float &Color::operator[](int rgbIndex) {
	if( rgbIndex == 0) return r;
	else if( rgbIndex == 1) return g;
	else if( rgbIndex == 2) return b;
	else if( rgbIndex == 3) return a;
	return r;		// default, return r reference
	}
	
	
inline unsigned long Color::getWeightedComposite( float weight ) {
	return ((int)(b*255*weight)) | ((int)(g*255*weight)) << 8 | ((int)(r*255*weight)) << 16 | ((int)(a*255)) << 24;
	}


inline unsigned long Color::getWeightedComposite( unsigned long c1, float weight ) {
	int b = c1 & 0xFF;
	
	int g = (c1 >> 8) & 0xFF;
	
	int r = (c1 >> 16) & 0xFF;

	int a = c1 >> 24;
	
	return ((int)(b*weight)) | (((int)(g*weight)) << 8) | (((int)(r*weight)) << 16) | (((int)(a*weight)) << 24);
	}

inline unsigned long Color::sumComposite(unsigned long c1, unsigned long c2) {
	int b = (c1 & 0xFF) + (c2 & 0xFF);
	if( b > 255) b = 255;
	
	int g = ((c1 >> 8) & 0xFF) + ((c2 >> 8) & 0xFF);
	if( g > 255) g = 255;
	
	int r = ((c1 >> 16) & 0xFF) + ((c2 >> 16) & 0xFF);
	if( r > 255) r = 255;

	int a = (c1 >> 24) + (c2 >> 24);
	if( a > 255) a = 255;

	return b | (g << 8) | (r << 16) | (a << 24);
	}
#endif
