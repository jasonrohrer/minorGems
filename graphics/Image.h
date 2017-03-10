/*
 * Modification History
 *
 * 2000-December-21		Jason Rohrer
 * Created. 
 *
 * 2001-January-6		Jason Rohrer
 * Fixed a bug in filter( ChannelFilter * ).
 * Set mSelection to NULL by default.
 *
 * 2001-January-10		Jason Rohrer
 * Made class serializable. 
 *
 * 2001-January-15		Jason Rohrer
 * Made copy() not-virtual, so it can be overridden by subclasses 
 * while allowing pointer type to determine which function 
 * implementation is invoked. 
 *
 * 2001-January-31		Jason Rohrer
 * Fixed a bug in copy().
 *
 * 2001-February-3		Jason Rohrer
 * Updated serialization code to use new interfaces. 
 *
 * 2001-February-4		Jason Rohrer
 * Rewrote the serialization code to send the image across as a byte
 * array with one byte per channel.  This will reduce the transfer size by
 * a factor of 8.  Keeping images in double format is convennient for image
 * creation, but the added quality never affects the end user anyway, so
 * there's no point in sending the extra data to a stream.
 * Removed an unused array allocation.
 *
 * 2005-February-21		Jason Rohrer
 * Made destructor virtual to avoid compiler warnings.
 *
 * 2006-August-25		Jason Rohrer
 * Made zero init of pixels optional (for speed).
 *
 * 2008-September-25		Jason Rohrer
 * Added a sub-image function and setting/getting color functions.
 *
 * 2011-January-16     Jason Rohrer
 * Opt:  Changed to use memset to init large double arrays to 0.
 *
 * 2011-April-5     Jason Rohrer
 * Fixed float-to-int conversion.  
 *
 * 2015-June-29     Jason Rohrer
 * Added image expansion function.  
 */
 
 
#ifndef IMAGE_INCLUDED
#define IMAGE_INCLUDED

#include <stdio.h> 
#include <string.h>
#include <math.h>
 
#include "ChannelFilter.h"
#include "Color.h"

#include "minorGems/io/Serializable.h"

 
/**
 * A multi-channel, double-valued image.
 *
 * Is Serializable.  Note that a serialized image doesn't have a selection.
 *
 * @author Jason Rohrer 
 */
class Image : public Serializable { 
	
	public:
		
		/**
		 * Constructs an image.
		 *
		 * @param inWidth width of image in pixels.
		 * @param inHeight height of image in pixels.
		 * @param inNumChannels number of channels in image.
         * @param inStartPixelsAtZero true to initialize all pixels
         *   to zero, or false to leave default memory values (garbage)
         *   in place (pixels must be initialized by caller in this case).
         *   Defaults to true.
		 */
		Image( int inWidth, int inHeight, int inNumChannels,
               char inStartPixelsAtZero = true );
		
		
		virtual ~Image();
		
		
		// gets the dimensions of this image.
		virtual long getWidth();
		virtual long getHeight();
		virtual long getNumChannels();
		
		/**
		 * Gets the values of a particular channel.
		 *
		 * Values are not copied.
		 *
		 * @param inChannel the channel to get.
		 * 
		 * @return the values of the specified channel in row-major order.
		 */
		virtual double *getChannel( int inChannel );
        


        /**
         * Gets the 3- or 4-channel color value at a given location in the 
         * image.
         *
         * @param inIndex the image index.
         *
         * @return a color object.
         */
        virtual Color getColor( int inIndex );
        

        
        /**
         * Sets the 3- or 4-channel color value at a given location in the 
         * image.
         *
         * @param inIndex the image index.
         * @param inColor the new color to set.
         */
        virtual void setColor( int inIndex, Color inColor );
        

		
		/**
		 * Selects a region of the image.  Default is a clear selection,
		 * which means all regions of image are affected by an applied
		 * filter.
		 *
		 * @param inSelection the image to use as the selection mask.
		 *   Values of 0 indicate pixels that are not selection, and 1
		 *   indicate pixels that are selected, with selection amount
		 *   varying linearly between 0 and 1.
		 *   If inSelection is a single channel, then that channel is 
		 *   used as a selection mask for all channels in this image.
		 *   If inSelection contains the same number of channels as this 
		 *   image, then the corresponding channels of inSelection are
		 *   are used to mask each channel of this image.
		 *   If inSelection contains a number of channels different
		 *   from the number in this image, the first channel of inSelection
		 *   is used to mask each channel in this image.
		 *
		 * Note that inSelection is not copied or destroyed by this class.
		 * Thus, modifying inSelection after calling setSelection will
		 * modify the selection in this image.
		 */
		virtual void setSelection( Image *inSelection );
		
		
		
		/**
		 * Gets the selection for this image.
		 *
		 * @return the selection for this image.  Returns NULL
		 *   if there is no selection.  Must not be destroyed
		 *   by caller before calling clearSelection.
		 */
		virtual Image *getSelection();
		
		
		
		/**
		 * Clears the selection.  Effectively selects the entire image.
		 */
		virtual void clearSelection();
		
		
		/**
		 * Applies a filter to the selected region of 
		 * a specified channel of this image.
		 *
		 * @param inFilter the filter to apply.
		 * @param inChannel the channel to filter.
		 */
		virtual void filter( ChannelFilter *inFilter, int inChannel );
		
		 
		/**
		 * Applies a filter to the selected region of 
		 * all channels of this image.
		 *
		 * @param inFilter the filter to apply.
		 */
		virtual void filter( ChannelFilter *inFilter );
		
		
		/**
		 * Copies the selected region of this image.  Not virtual,
		 * so can be overridden by subclasses while allowing pointer
		 * type to determine which function implementation is invoked.
		 *
		 * @return a new image with the same number of channels
		 *   as this image, each containing the selected region
		 *   from each corresponding channel of this image.  Unselected
		 *   regions are set to black.  Returned image has no selection.
		 */
		Image *copy();
		
		
		/**
		 * Pastes the selected region from another image into
		 * the selected region of this image.
		 *
		 * @param inImage the image to paste.  Let c be the number
		 *   of channels in this image, and cp be the number
		 *   of channels in the image being pasted.
		 *   If c<cp, then only the first c channels of inImage
		 *   are pasted.  If c>cp, then only the first cp channels
		 *   of this image are pasted into.
		 */
		virtual void paste( Image *inImage );
		
		
		/**
		 * Copies the data from the selected region of a channel.
		 *
		 * @param inChannel the channel to copy.
		 *
		 * @return a copy of the channel data.  Must be destroyed
		 *   by the caller.
		 */
		virtual double *copyChannel( int inChannel );
		
		
		
		/**
		 * Pastes channel data into the selected region of a specified channel.
		 *
		 * @param inChannelData an array containing the channel 
		 *   data to be pasted.  Must be destroyed by caller.
		 * @param inChannel the channel to paste into.
		 */
		virtual void pasteChannel( double *inChannelData, int inChannel );
		
	
		/**
		 * Gets the mask for a specified channel.
		 *
		 * @param inChannel the channel to get a mask for.
		 *
		 * @return the mask data for the specified channel.
		 *   If selection has the same number of channels as this image
		 *   then a different mask is returned for each channel.  Otherwise,
		 *   the first channel from the selection is returned as the
		 *   mask for every channel.  Returns NULL if there is no selection.
		 */
		virtual double *getChannelSelection( int inChannel );

		

        /**
         * Extracts a smaller sub-image from this image.
         *
         * Ignores current selection.
         *
         * @param inStartX, inStartY, inWidth, inHeight
         *   coordinates for the top left corner pixel of the sub-image
         *   and the width and height of the sub-image.
         *
         * @return the sub-image as a new image.  Must be destoryed by caller.
         */ 
        Image *getSubImage( int inStartX, int inStartY, 
                            int inWidth, int inHeight );
        
        
        // copies region from source into this image, placing
        // upper left corner of source at inStartX, inStartY in this image
        // and copying inWidth by inHeight pixels
        void setSubImage( int inStartX, int inStartY, 
                          int inWidth, int inHeight,
                          Image *inSourceImage );
        


        // centers this image in a new, larger image with a black or white
        // border
        Image *expandImage( int inExpandedWidth, int inExpandedHeight,
                            char inWhiteBorder = false );

        


        /**
         * Generated a 4-channel (alpha) image from a 3-channel image
         * by using the color of the lower left corner pixel as a transparent
         * background color.
         *
         * Note that if a 4-channel image is passed in, the existing alpha
         * channel will be replaced in the returned image.
         *
         * @return a new 4-channel image.  Must be destoryed by caller.
         */
        Image *generateAlphaChannel();
        
        
        // inSaturation in range [-1,1], where 0 is no adjustment
        // does nothing if image has fewer than 3 channels (needs RGB)
        virtual void adjustSaturation( double inSaturation );
     
		
		// implement the Serializable interface
		virtual int serialize( OutputStream *inOutputStream );
		virtual int deserialize( InputStream *inInputStream );
	
	protected:
		long mWide, mHigh, mNumPixels, mNumChannels;
		
		double **mChannels;
		
		// NULL if nothing selected.
		Image *mSelection;
		
		
		
		
		
		/**
		 * Pastes masked channel data into the selected region of a 
		 * specified channel.
		 *
		 * @param inChannelData an array containing the channel 
		 *   data to be pasted.  Must be destroyed by caller.
		 * @param inMask the selection mask to use for passed-in channel.
		 *   Set to NULL for no mask.
		 * @param inChannel the channel to paste into.
		 */
		virtual void pasteChannel( double *inChannelData, double *inMask,
			int inChannel );
		
	};



inline Image::Image( int inWidth, int inHeight, int inNumChannels,
                     char inStartPixelsAtZero ) 
	: mWide( inWidth ), mHigh( inHeight ), mNumPixels( inWidth * inHeight ),
	mNumChannels( inNumChannels ), mChannels( new double*[inNumChannels] ),
	mSelection( NULL ) {
	
	// initialize all channels
	for( int i=0; i<mNumChannels; i++ ) {
		mChannels[i] = new double[ mNumPixels ];
        if( inStartPixelsAtZero ) {
            memset( mChannels[i], 0, mNumPixels * sizeof( double ) );
            }
		}
	}
		
		
		
inline Image::~Image() {
	for( int i=0; i<mNumChannels; i++ ) {
		delete [] mChannels[i];
		}
	delete [] mChannels;	
	}
		


inline long Image::getWidth() {
	return mWide;
	}
	
	
	
inline long Image::getHeight() {
	return mHigh;
	}



inline long Image::getNumChannels() {
	return mNumChannels;
	}
	
	
		
inline double *Image::getChannel( int inChannel ) {
	return mChannels[ inChannel ];
	}
	


inline Color Image::getColor( int inIndex ) {
    Color c;
    
    for( int i=0; i<mNumChannels && i < 4; i++ ) {
        c[i] = (float)( mChannels[i][inIndex] );
        }
    
    return c;
    }



inline void Image::setColor( int inIndex, Color inColor ) {
    for( int i=0; i<mNumChannels && i < 4; i++ ) {
        mChannels[i][inIndex] = (double)( inColor[i] );
        }
    }

		 
		
inline void Image::setSelection( Image *inSelection ) {
	mSelection = inSelection;
	}



inline Image *Image::getSelection() {
	return mSelection;
	}
		
		
		
inline void Image::clearSelection() {
	mSelection = NULL;
	}
	
	
		
inline void Image::filter( ChannelFilter *inFilter, int inChannel ) {

	if( mSelection == NULL ) {
		inFilter->apply( mChannels[ inChannel ], mWide, mHigh );
		}
	else { // part of image selected
		// turn selection off and filter channel entirely
		Image *tempSelection = mSelection;
		mSelection = NULL;
		// filter a copy of the channel
		double *filteredChannel = copyChannel( inChannel );
		inFilter->apply( filteredChannel, mWide, mHigh );
		
		// now paste filtered channel back into selected region
		mSelection = tempSelection;
		pasteChannel( filteredChannel, inChannel );
		}
	}
		
	
		
inline void Image::filter( ChannelFilter *inFilter ) {
	for( int i=0; i<mNumChannels; i++ ) {
		filter( inFilter, i );
		}	
	}



inline Image *Image::copy() {
	Image *copiedImage = new Image( mWide, mHigh, mNumChannels );
	copiedImage->paste( this );
	
	return copiedImage;
	}
		
		
		
inline void Image::paste( Image *inImage ) {
	// copy paste in the min number of channels only
	int numChannelsToPaste = mNumChannels;
	if( numChannelsToPaste > inImage->getNumChannels() ) {
		numChannelsToPaste = inImage->getNumChannels();
		}
	
	for( int i=0; i<numChannelsToPaste; i++ ) {
		pasteChannel( inImage->getChannel(i), 
			inImage->getChannelSelection(i), i );
		}
	}



inline double *Image::copyChannel( int inChannel ) {
	
	// first, copy the channel
	double *copiedChannel = new double[mNumPixels];
	memcpy( copiedChannel, 
		mChannels[inChannel], sizeof( double ) * mNumPixels );
	
	
	if( mSelection != NULL ) {
		// apply selection to copied channel
		double *selection = getChannelSelection( inChannel );	
		
		// scale copied channel with selection
		for( int i=0; i<mNumPixels; i++ ) {
			copiedChannel[i] = copiedChannel[i] * selection[i];
			}
		}
	
	return copiedChannel;	
	}



inline double *Image::getChannelSelection( int inChannel ) {
	if( mSelection == NULL ) {
		return NULL;
		}
	else {
		if( mSelection->getNumChannels() == mNumChannels ) {
			// use separate selection for each channel
			return mSelection->getChannel( inChannel );
			}
		else {
			// use first channel of selection for all channels
			return mSelection->getChannel( 0 );
			}
		}
	}


		
inline void Image::pasteChannel( double *inChannelData, int inChannel ) {
	pasteChannel( inChannelData, NULL, inChannel );
	}



// We've abstracted away the complexity in the other fuctions,
// but it all seemed to filter down into this function, which
// is very messy.
inline void Image::pasteChannel( double *inChannelData, double *inMask,
	int inChannel ) {
	
	double *thisChannel = mChannels[inChannel];
	if( mSelection != NULL ) {
		// scale incoming data with this selection
		double *selection = getChannelSelection(inChannel);
		
		if( inMask != NULL ) {
			// scale incoming data with both masks
			for( int i=0; i<mNumPixels; i++ ) {
				thisChannel[i] = ( thisChannel[i] * (1-selection[i])
					+ 
					( thisChannel[i] * (1-inMask[i]) 
						+ inChannelData[i] * inMask[i] ) * selection[i] );
				}
			}
		else {	
			// scale incomming data with this selecition only
			for( int i=0; i<mNumPixels; i++ ) {
				thisChannel[i] = ( thisChannel[i] * (1-selection[i])
					+ inChannelData[i] * selection[i] );
				}
			}
		}
	else {	// no selection in this image
		if( inMask != NULL ) {
			// scale incoming data with its masks
			for( int i=0; i<mNumPixels; i++ ) {
				thisChannel[i] = thisChannel[i] * (1-inMask[i]) 
					+ inChannelData[i] * inMask[i];
				}
			}
		else {
			// copy channel directly, with no mask
			memcpy( thisChannel, inChannelData, sizeof(double) * mNumPixels );
			}
		}	
	}



inline Image *Image::getSubImage( int inStartX, int inStartY, 
                                  int inWidth, int inHeight ) {
    
    int endY = inStartY + inHeight;
    
    Image *destImage = new Image( inWidth, inHeight, mNumChannels, false );

    for( int c=0; c<mNumChannels; c++ ) {
        double *destChannel = destImage->getChannel( c );
        double *sourceChannel = mChannels[c];
        
        int destY=0;
        for( int y=inStartY; y<endY; y++ ) {
            
            // copy row
            memcpy( &( destChannel[ destY * inWidth ] ),
                    &( sourceChannel[ y * mWide + inStartX ] ),
                    sizeof( double ) * inWidth );
            
            destY ++;
            }
        }
    
    return destImage;
    }



inline void Image::setSubImage( int inStartX, int inStartY, 
                         int inWidth, int inHeight,
                         Image *inSourceImage ) {

    int sourceWidth = inSourceImage->getWidth();
    
    if( inWidth > inSourceImage->getWidth() ) {
        inWidth = inSourceImage->getWidth();
        }
    if( inHeight > inSourceImage->getHeight() ) {
        inHeight = inSourceImage->getHeight();
        }

    if( inWidth + inStartX > mWide ) {
        inWidth = mWide - inStartX;
        }
    if( inHeight + inStartY > mHigh ) {
        inHeight = mHigh - inStartY;
        }
    
    
    int endY = inStartY + inHeight;
   
    for( int c=0; c<mNumChannels; c++ ) {
        double *destChannel = mChannels[c];
        double *sourceChannel = inSourceImage->getChannel(c);
        
        int sourceY=0;
        for( int y=inStartY; y<endY; y++ ) {
            
            // copy row
            memcpy( &( destChannel[ y * mWide + inStartX ] ),
                    &( sourceChannel[ sourceY * sourceWidth ] ),
                    sizeof( double ) * inWidth );
            
            sourceY ++;
            }
        }

    }



inline Image *Image::expandImage( int inExpandedWidth, int inExpandedHeight,
                                  char inWhiteBorder ) {
    Image *destImage = new Image( inExpandedWidth, inExpandedHeight,
                                  mNumChannels, ! inWhiteBorder );

    if( inWhiteBorder ) {
        int numPixels = inExpandedWidth * inExpandedHeight;
        for( int c=0; c<mNumChannels; c++ ) {
            double *chan = destImage->getChannel( c );
            for( int i=0; i<numPixels; i++ ) {
                chan[i] = 1.0;
                }
            }
        }
    

    int xOffset = ( inExpandedWidth - mWide ) / 2;
    int yOffset = ( inExpandedHeight - mHigh ) / 2;
    

    for( int c=0; c<mNumChannels; c++ ) {
        double *destChannel = destImage->getChannel( c );
        double *sourceChannel = mChannels[c];
        
        int destY = yOffset;
        for( int y=0; y<mHigh; y++ ) {
            
            // copy row
            memcpy( &( destChannel[ destY * inExpandedWidth + xOffset ] ),
                    &( sourceChannel[ y * mWide ] ),
                    sizeof( double ) * mWide );
            
            destY ++;
            }
        }
    
    return destImage;
    }





inline Image *Image::generateAlphaChannel() {
    Image *fourChannelImage = new Image( getWidth(),
                                         getHeight(),
                                         4, false );
        
    // copy first three
    for( int c=0; c<3; c++ ) {
        fourChannelImage->pasteChannel( getChannel( c ),
                                        c );
        }

    double *r = fourChannelImage->getChannel( 0 );
    double *g = fourChannelImage->getChannel( 1 );
    double *b = fourChannelImage->getChannel( 2 );
        
    // index of transparency
    int tI = 
        fourChannelImage->getWidth() * 
        ( fourChannelImage->getHeight() - 1 );
        
    // color of transparency
    double tR = r[tI];
    double tG = g[tI];
    double tB = b[tI];
    
    double *alpha = fourChannelImage->getChannel( 3 );
    
    int numPixels = 
        fourChannelImage->getWidth() * 
        fourChannelImage->getHeight();
        
    for( int i=0; i<numPixels; i++ ) {
            
        if( r[i] == tR && 
            g[i] == tG && 
            b[i] == tB ) {
            alpha[i] = 0;
            }
        else {
            alpha[i] = 1;
            }
        }
    
    return fourChannelImage;
    }

    

inline int Image::serialize( OutputStream *inOutputStream ) {
	// first output width and height
	
	int numBytes = 0;
	
	numBytes += inOutputStream->writeLong( mWide );
	numBytes += inOutputStream->writeLong( mHigh );
	
	// then output number of channels
	numBytes += inOutputStream->writeLong( mNumChannels );
	
	// now output each channel
	for( int i=0; i<mNumChannels; i++ ) {
		unsigned char *byteArray = new unsigned char[mNumPixels];
		
		// convert each 8-bit double pixel to one byte
		for( int p=0; p<mNumPixels; p++ ) {
			//numBytes += inOutputStream->writeDouble( mChannels[i][p] );
			byteArray[p] = (unsigned char)( lrint( mChannels[i][p] * 255 ) );
			}
		
		numBytes += inOutputStream->write( byteArray, mNumPixels );
			
		delete [] byteArray;	
		}
	
	return numBytes;
	}
	
	
	
inline int Image::deserialize( InputStream *inInputStream ) {
	int i;
	// first delete old image channels
	for( i=0; i<mNumChannels; i++ ) {
		delete [] mChannels[i];
		}
	delete [] mChannels;
	
	
	// input width and height
	
	int numBytes = 0;
	
	numBytes += inInputStream->readLong( &mWide );
	numBytes += inInputStream->readLong( &mHigh );
	
	mNumPixels = mWide * mHigh;
	
	// then input number of channels
	numBytes += inInputStream->readLong( &mNumChannels );
	
	mChannels = new double*[mNumChannels];
	
	// now input each channel
	for( i=0; i<mNumChannels; i++ ) {
		mChannels[i] = new double[mNumPixels];
		
		unsigned char *byteArray = new unsigned char[mNumPixels];
		numBytes += inInputStream->read( byteArray, mNumPixels );
		
		// convert each byte to an 8-bit double pixel
		for( int p=0; p<mNumPixels; p++ ) {
			//numBytes += inInputStream->readDouble( &( mChannels[i][p] ) );
			mChannels[i][p] = (double)( byteArray[p] ) / 255.0;
			}
		
		delete [] byteArray;	
		}
	
	return numBytes;
	}





static double max( double inA, double inB, double inC ) {
    if( inA >= inB &&
        inA >= inC ) {
        return inA;
        }
    if( inB >= inA &&
        inB >= inC ) {
        return inB;
        }
    return inC;
    }

static double min( double inA, double inB, double inC ) {
    if( inA <= inB &&
        inA <= inC ) {
        return inA;
        }
    if( inB <= inA &&
        inB <= inC ) {
        return inB;
        }
    return inC;
    }


// conversion algorithms found here:
// http://stackoverflow.com/questions/2353211/hsl-to-rgb-color-conversion

// Modified somewhat to match GIMP code



static void rgbToHSL( double inR, double inG, double inB,
                      double *outH, double *outS, double *outL ) {

    double maxC = max( inR, inG, inB );
    double minC = min( inR, inG, inB );

    double h, s, l;
    
    l = ( maxC + minC ) / 2.0;

    if( maxC == minC ){
        h = s = 0; // achromatic
        }
    else{
        double d = maxC - minC;
        
        // divide by zero impossible here unless maxC = minC = 1,
        // and then we're in the above achromatic case
        s = l > 0.5 ? 
            d / (2 - ( maxC + minC ) ) : 
            d / ( maxC + minC );
        
        if( maxC == inR )
            h = ( inG - inB ) / d;
        else if( maxC == inG ) 
            h = ( inB - inR ) / d + 2.0; 
        else if( maxC == inB ) 
            h = ( inR - inG ) / d + 4.0;
        
        h /= 6.0;

        if( h < 0 ) {
            h += 1.0;
            }
        }
    
    *outH = h;
    *outS = s;
    *outL = l;
    }



static double hueToRGB( double p, double q, double t ) {
    if( t < 0 ) t += 6.0;
    if( t > 6.0 ) t -= 6.0;
    if( t < 1.0 ) return p + ( q - p ) * t;
    if( t <  3.0 ) return q;
    if( t < 4.0 ) return p + ( q - p ) * ( 4.0 - t );
    return p;
    }



static void hslToRGB( double inH, double inS, double inL,
                      double *outR, double *outG, double *outB ) {
    double r, g, b;

    if( inS == 0) {
        r = g = b = inL; // achromatic
        }
    else {
        double q = inL < 0.5 ? 
            inL * ( 1.0 + inS ) : 
            inL + inS - inL * inS;
        double p = 2.0 * inL - q;
        
        double sixHue = inH * 6.0;

        r = hueToRGB( p, q, sixHue + 2.0 );
        g = hueToRGB( p, q, sixHue );
        b = hueToRGB( p, q, sixHue - 2.0 );
        }

    *outR = r;
    *outG = g;
    *outB = b;
    }


    
inline void Image::adjustSaturation( double inSaturation ) {    
    if( mNumChannels < 3 ) {
        return;
        }    

    for( int p=0; p<mNumPixels; p++ ) {
        double r = mChannels[0][p];
        double g = mChannels[1][p];
        double b = mChannels[2][p];
        
        double h,s,l;
        
        rgbToHSL( r, g, b, &h, &s, &l );
        
        /*
        // algorithm found here:
        // http://www.pocketmagic.net/
        //       enhance-saturation-in-images-programatically/

        if( inSaturation >= 0 ) {
            // we don't want to saturate unsaturated colors -> 
            // we get only defects 
            // for unsaturared colors this tends to 0
            double grayFactor = s; 
            // how far can we go?
            // if we increase saturation, we have "255-s" space left
            double varInterval = 1.0 - s; 
            // compute the new saturation
            s = s + inSaturation * varInterval * grayFactor;
			} 
        else {
            // how far can we go?
            // for decrease we have "s" left
            double varInterval = s; 
            s = s + inSaturation * varInterval;
			}
        */
        
        // however, this does NOT match the output from the Gimp
        
        // this is what the GIMP does, and it has many of the nice
        // properties of the above code, even though it's simpler
        
        // Ignoring rounding errors (the GIMP does the whole conversion
        // in 0-255 int space, and rounds HSL values to 0-255 integers)
        // this matches the output of the GIMP.
        s *= inSaturation + 1.0;

        if( s > 1 ) {
            s = 1;
            }
        else if( s < 0 ) {
            s = 0;
            }

        hslToRGB( h, s, l, &r, &g, &b );

        mChannels[0][p] = r;
        mChannels[1][p] = g;
        mChannels[2][p] = b;
        }
    }


	
#endif
