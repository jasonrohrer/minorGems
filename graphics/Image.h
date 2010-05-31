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
 */
 
 
#ifndef IMAGE_INCLUDED
#define IMAGE_INCLUDED

#include <stdio.h> 
#include <string.h>
 
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
            for( int j=0; j<mNumPixels; j++ ) {
                mChannels[i][j] = 0.0;
                }
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
			byteArray[p] = (unsigned char)( mChannels[i][p] * 255 );
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


	
#endif
