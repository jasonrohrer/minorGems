// Jason Rohrer
// P_M.h

/**
*
*	Object for reading, holding, and writing PBM and PGM images and files
*
*	Assumes that image data is in binary format (not ASCII text format).
*	I.e., 	for PBM, only reads P4 magic number files
*			for PGM, only reads P5 magic number files
*
*
*	Created 2-27-99
*	Mods:
*/




#ifndef P_M_OBJECT_INCLUDED
#define P_M_OBJECT_INCLUDED

#include <stdio.h>


enum P_M_format{ PBM=4, PGM=5 };


class P_M {


	public:
		P_M( char *fileName, int *status );		// construct by passing in a filename
												// status returned in pointer location
												// 0 ok, -1 file open failed
												
		// construct by passing data in, making a P_M of a specific format
		// data COPIED from buffer
		P_M( P_M_format f, int w, int h, unsigned char *buff, int maxGry=255 );
	
		~P_M();		// destructor
		
		
		
		P_M_format getFormat();	
		
		int getWidth();
		int getHeight();
		
		unsigned char *getBuffer();		// get image data buffer
		
		int *getRowOffsets();	// get precomputed row offsets
		
		
		int write( char *fileName );		// write P_M to file
		
											// return 0 ok, -1 failure
	private:
	
		P_M_format format;
		
		int width;
		int height;
		
		int maxGray;
		
		unsigned char *buffer;
		
		int *rowOffsets;	
		
	};



inline P_M::~P_M() {
	delete [] buffer;
	delete [] rowOffsets;
	}


inline P_M_format P_M::getFormat() {
	return format;
	}
	
inline int P_M::getWidth() {
	return width;
	}
	
inline int P_M::getHeight() {
	return height;
	}

inline unsigned char *P_M::getBuffer() {
	return buffer;
	}

inline int *P_M::getRowOffsets() {
	return rowOffsets;
	}




#endif