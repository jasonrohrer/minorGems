/*
 * Modification History
 *
 * 2001-May-20   Jason Rohrer
 * Created.  
 */
 
 
#ifndef GRAY_CODE_INCLUDED
#define GRAY_CODE_INCLUDED 


/**
 * A gray code for arbitrary length bit strings.
 *
 * Sample gray code:
 * in:   000 001 010 011 100 101 110 111
 * out:  000 001 011 010 110 111 101 100
 *
 * @author Jason Rohrer.
 */
class GrayCode {



	public:


		/**
		 * Encodes a bit string as a gray code.
		 *
		 * @param inBits an array with each bit represented
		 *   by a char.  Must be destroyed by caller.
		 * @param inNumBits the number of bits (chars) in
		 *   the array.
		 *
		 * @return a new array containing the encoded
		 *   bit string, with one bit per char.  Must be destroyed
		 *   by caller.
		 */
		static char *encode( char *inBits, int inNumBits ); 

		/**
		 * Decodes a gray code bit string.
		 *
		 * @param inBits a gray coded array with each bit represented
		 *   by a char.  Must be destroyed by caller.
		 * @param inNumBits the number of bits (chars) in
		 *   the array.
		 *
		 * @return a new array containing the decoded
		 *   bit string, with one bit per char.  Must be destroyed
		 *   by caller.
		 */
		static char *decode( char *inGrayBits, int inNumBits );
		
		
	};



inline char *GrayCode::encode( char *inBits, int inNumBits ) {
	char *outBits = new char[ inNumBits ];

	// first bits always match
	outBits[0] = inBits[0];
	
	for( int i=1; i<inNumBits; i++ ) {
		// xor bit i with bit i-1
		outBits[i] = inBits[i] ^ inBits[i-1];
		}

	return outBits;
	}



inline char *GrayCode::decode( char *inGrayBits, int inNumBits ) {
	char *outBits = new char[ inNumBits ];
	
	for( int i=0; i<inNumBits; i++ ) {
		outBits[i] = inGrayBits[i];

		// xor with all the bits on the left
		for( int j=0; j<i; j++ ) {
			outBits[i] = outBits[i] ^ inGrayBits[j];
			}
		}

	return outBits;
	}



#endif
