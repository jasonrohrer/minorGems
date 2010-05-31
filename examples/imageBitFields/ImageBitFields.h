/*
 * Modification History
 *
 * 2001-May-18   Jason Rohrer
 * Created.  
 */
 
 
#ifndef IMAGE_BIT_FIELDS_INCLUDED
#define IMAGE_BIT_FIELDS_INCLUDED 

#include "minorGems/graphics/Image.h"

#include "BitField.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

/**
 * Class containing static methods to manipulate image bit fields.
 *
 * @author Jason Rohrer.
 */
class ImageBitFields {



	public:

		/**
		 * Generates a set of bit fields from an image channel.
		 *
		 * @param inImage the image to extract bit fields from.
		 *   Must be destroyed by caller.
		 * @param inChannel the channel number to analyze.
		 * @param inNumFields the number of fields to extract.
		 *   Equivalent to the number of bits per pixel used in
		 *   the analysis.  Must be in [1,32].
		 *
		 * @return an array of bit fields for the image, or
		 *   NULL in the case of an error.  The fields are
		 *   stored in big endian order in the array.
		 *   Must be destroyed by caller.
		 */
		static BitField **imageToBitFields(
			Image *inImage, int inChannel, int inNumFields );



		/**
		 * Generates a set of bit fields from an image.
		 *
		 * @param inImage the image to extract bit fields from.
		 *   Must be destroyed by caller.
		 * @param inNumFieldsPerChannel the number of fields to
		 *   extract per image channel.
		 *   Equivalent to the number of bits per pixel used in
		 *   the analysis.  Must be in [1,32].
		 *
		 * @return an array of bit fields for the image, or
		 *   NULL in the case of an error.  The array is
		 *   indexed as [channel_number][field_number].
		 *   Must be destroyed by caller.
		 */
		static BitField ***imageToBitFields(
			Image *inImage, int inNumFieldsPerChannel );
		

		/**
		 * Generates a 1-channel image from a set of bit fields.
		 *
		 * @param inFields the fields to create the image from.
		 *   The fields must be stored in big endian order in
		 *   the array.
		 * @param inNumFields the number of fields being passed in.
		 *   Must be in [1,32].
		 *
		 * @return a one-channel image extracted from the bit fields, or
		 *   NULL in the case of an error.
		 *   Must be destroyed by caller.
		 */
		static Image *bitFieldsToImage(
			BitField **inFields, int inNumFields );


		/**
		 * Generates a mulit-channel image from a set of bit fields.
		 *
		 * @param inFields the fields to create the image from.
		 *   The fields must be stored in big endian order in
		 *   the array.  The array must be indexed as
		 *   [channel_number][field_number].
		 * @param inNumChannels the number of channels in
		 *   the output image.
		 * @param inNumFieldsPerChannel the number of fields
		 *   (per channel) being passed in.
		 *   Must be in [1,32].
		 *
		 * @return a multi-channel image extracted from the bit fields, or
		 *   NULL in the case of an error.
		 *   Must be destroyed by caller.
		 */
		static Image *bitFieldsToImage(
			BitField ***inFields, int inNumChannels,
			int inNumFieldsPerChannel );

	};



inline BitField **ImageBitFields::imageToBitFields(
	Image *inImage, int inChannel, int inNumFields ) {

	if( inNumFields > 32 || inNumFields < 0 ) {
		printf(
			"ImageBitFields::imageToBitFields, inNumFields " );
		printf( "must be in [0,32]\n" );

		return NULL;
		}
	
	int i, p;
	int w = inImage->getWidth();
	int h = inImage->getHeight();
	int numPixels = w * h;
	double *pixels = inImage->getChannel( inChannel );
	
	BitField **returnFields = new BitField*[ inNumFields ];

	// the field for each BitField
	char **fields = new char*[ inNumFields ];
	
	for( i=0; i<inNumFields; i++ ) {
		returnFields[i] = new BitField( w, h );
		
		fields[i] = returnFields[i]->getField();
		}

	int maxIntPixelValue = (int)( pow( 2, inNumFields ) - 1 );
	
	// extract bits from each pixel
	for( p=0; p<numPixels; p++ ) {
		// convert each pixel to an integer with the appropriate
		// number of bits.
		unsigned int pixelValue =
			(unsigned int)( maxIntPixelValue * pixels[ p ] );

		// extract each bit
		for( i=0; i<inNumFields; i++ ) {
			// big endian

			// shift and AND to extract
			fields[i][p] = (char)(
				pixelValue >> ( inNumFields - 1 - i ) & 0x1 );
			}
		
		}

	// delete the array, but not the contents
	delete [] fields;

	return returnFields;
	}



inline BitField ***ImageBitFields::imageToBitFields(
	Image *inImage, int inNumFieldsPerChannel ) {

	if( inNumFieldsPerChannel > 32 || inNumFieldsPerChannel < 0 ) {
		printf(
			"ImageBitFields::imageToBitFields, inNumFieldsPerChannel " );
		printf( "must be in [0,32]\n" );

		return NULL;
		}
	
	int numChannels = inImage->getNumChannels();

	BitField ***returnFields = new BitField**[ numChannels ];

	// call the single-channel function on each channel
	for( int i=0; i<numChannels; i++ ) {
		returnFields[i] = imageToBitFields(
			inImage, i, inNumFieldsPerChannel );
		}

	return returnFields;
	}



inline Image *ImageBitFields::bitFieldsToImage(
	BitField **inFields, int inNumFields ) {

	if( inNumFields > 32 || inNumFields < 0 ) {
		printf(
			"ImageBitFields::bitFieldsToImage, inNumFields " );
		printf( "must be in [0,32]\n" );

		return NULL;
		}

	int i, p;
	
	int w = inFields[0]->getWidth();
	int h = inFields[0]->getHeight();
	int numPixels = w * h;

	// the field for each BitField
	char **fields = new char*[ inNumFields ];
	
	for( i=0; i<inNumFields; i++ ) {
		
		fields[i] = inFields[i]->getField();
		}

	Image *returnImage = new Image( w, h, 1 );
	double *pixels = returnImage->getChannel( 0 );


	double maxIntPixelValue = pow( 2, inNumFields ) - 1;
	
	// extract bits from each pixel
	for( p=0; p<numPixels; p++ ) {
		// convert each pixel to an integer with the appropriate
		// number of bits.
		unsigned int pixelValue = 0;

		// add each bit into the pixel value
		for( i=0; i<inNumFields; i++ ) {
			// big endian

			// shift and OR to insert
			pixelValue = pixelValue |
				( fields[i][p] << ( inNumFields - 1 - i ) );
			}

		// divide to bring into range [0,1]
		pixels[p] = pixelValue / maxIntPixelValue;		
		}

	
	
	// delete the array, but not the contents
	delete [] fields;
	
	return returnImage;
	}



inline Image *ImageBitFields::bitFieldsToImage(
	BitField ***inFields, int inNumChannels,
	int inNumFieldsPerChannel ) {

   
	if( inNumFieldsPerChannel > 32 || inNumFieldsPerChannel < 0 ) {
		printf(
			"ImageBitFields::bitFieldsToImage, inNumFieldsPerChannel " );
		printf( "must be in [0,32]\n" );

		return NULL;
		}

	int i;
	
	// one image for each channel
	Image **images = new Image*[ inNumChannels ];

	// call the single-channel function on each channel
	for( i=0; i<inNumChannels; i++ ) {
		images[i] = bitFieldsToImage(
			inFields[i], inNumFieldsPerChannel );
		}

	int w = images[0]->getWidth();
	int h = images[0]->getHeight();
	int numPixels = w * h;
	
	Image *returnImage = new Image( w, h, inNumChannels );

	// copy each of the temp images into one of our return channels
	for( i=0; i<inNumChannels; i++ ) {
		double *srcChannel = images[i]->getChannel( 0 );
		double *destChannel = returnImage->getChannel( i );

		memcpy( destChannel, srcChannel, sizeof( double ) * numPixels );

		delete images[i];
		}

	delete [] images;

	
	return returnImage;
	}



#endif
