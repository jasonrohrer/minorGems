// Jason Rohrer
// UniversalFileIO.h

/**
*
*	Object that handles universal file reading and writing
*	Writes files as big endian, even on little endian machines
*
*	Assumes that longs and floats are 32-bits
*
*	Created 1-12-99
*	Mods:
*		Jason Rohrer	1-30-2000	Fixed fwrite functions to work on little endian machines
*/



#ifndef UNIVERSAL_FILE_IO_INCLUDED
#define UNIVERSAL_FILE_IO_INCLUDED


#include <stdio.h>



class UniversalFileIO {

	public:
		
		UniversalFileIO();
		
		long freadLong( FILE *f );
		float freadFloat( FILE *f );
		
		void fwriteLong( FILE *f, long x );
		void fwriteFloat( FILE *f, float x );
		
	private:
	
		char machineBigEndian;
		
		char bytesInLong;
		char bytesInFloat;

	};
	
	
inline UniversalFileIO::UniversalFileIO()
	: machineBigEndian( true ),
	bytesInLong( 4 ), bytesInFloat( 4 )
	{
	
	// test whether machine is big endian
	long test = 1;
	char testChopped = (*(char*)&test);
	if( testChopped == 1 ) machineBigEndian = false;
	}




inline long UniversalFileIO::freadLong( FILE *f ) {
	if( machineBigEndian ) {
		long returnVal;
		fread((void *)&returnVal, sizeof(long), 1, f);
		return returnVal;
		}
	else {
		unsigned char *buffer = new unsigned char[bytesInLong];

		fread((void *)buffer, sizeof(char), bytesInLong, f);
		
		// now put the bytes into a long
		long returnVal = (long)( buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3] );
			
		delete [] buffer;
		
		return returnVal;
		}
	}
	
inline float UniversalFileIO::freadFloat( FILE *f ) {
	if( machineBigEndian ) {
		float returnVal;
		fread( (void *)&returnVal, sizeof(float), 1, f );
		return returnVal;
		}
	else {
		unsigned char *buffer = new unsigned char[bytesInFloat];

		fread( (void *)buffer, sizeof(char), bytesInFloat, f );

		// now put the bytes into a long
		long temp = (long)(buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3]);
		
		delete [] buffer;
		
		return *((float *) &temp);		// convert long into a float
		}
	}


inline void UniversalFileIO::fwriteLong( FILE *f, long x ) {
	if( machineBigEndian ) {
		fwrite( (void *)&x, sizeof(long), 1, f );
		}
	else {
		unsigned char *buffer = new unsigned char[bytesInLong];
		
		buffer[0] = (unsigned char)(x >> 24);		// put bytes from long into char buffer
		buffer[1] = (unsigned char)(x >> 16);
		buffer[2] = (unsigned char)(x >> 8);
		buffer[3] = (unsigned char)(x);
		
		fwrite( (void *)buffer, sizeof(char), bytesInLong, f );
		}
	}
	
inline void UniversalFileIO::fwriteFloat( FILE *f, float x ) {
	if( machineBigEndian ) {
		fwrite( (void *)&x, sizeof(float), 1, f );
		}
	else {
		unsigned char *buffer = new unsigned char[bytesInFloat];
		
		long temp = *((long*)&x);		// convert float into long so that bit-wise ops can be performed
		
		buffer[0] = (unsigned char)(temp >> 24);		// put bytes from float into char buffer
		buffer[1] = (unsigned char)(temp >> 16);
		buffer[2] = (unsigned char)(temp >> 8);
		buffer[3] = (unsigned char)(temp);
		
		fwrite( (void *)buffer, sizeof(char), bytesInFloat, f );
		}
	}


#endif