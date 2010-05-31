/*
 * Modification History
 *
 * 2002-May-25    Jason Rohrer
 * Created.
 * Made getZero static.
 */



#ifndef BIG_INT_INCLUDED
#define BIG_INT_INCLUDED



/**
 * A multi-byte integer representation.
 *
 * Some of the ideas used in this class were gleaned
 * from studying Sun's Java 1.3 BigInteger implementation.
 *
 * @author Jason Rohrer.
 */
class BigInt {


        
    public:


        
        /**
         * Constructs an integer.
         *
         * @param inSign the sign of this integer:
         *   -1 if negative, +1 if positive, and 0 if zero.
         * @param inNumBytes the number of bytes in this integer.
         * @param inBytes the bytes for this integer.
         *   Copied internally, so must be destroyed by caller.
         */
        BigInt( int inSign, int inNumBytes, unsigned char *inBytes );


        
        /**
         * Constructs an integer from a 32-bit int.
         *
         * @param inInt the int to use.
         */
        BigInt( int inInt );



        ~BigInt();
        
        

        /**
         * Adds an integer to this integer.
         *
         * @praram inOtherInt the int to add.
         *   Must be destroyed by caller.
         *
         * @return a newly allocated integer containing the sum.
         *   Must be destroyed by caller.
         */
        BigInt *add( BigInt *inOtherInt );

        

        /**
         * Subtracts an integer from this integer.
         *
         * @praram inOtherInt the int to subtract.
         *   Must be destroyed by caller.
         *
         * @return a newly allocated integer containing the difference.
         *   Must be destroyed by caller.
         */
        BigInt *subtract( BigInt *inOtherInt );



        /**
         * Gets whether this integer is less than another integer.
         *
         * @praram inOtherInt the integer test.
         *   Must be destroyed by caller.
         *
         * @return true if this integer is less than the other.
         */        
        char isLessThan( BigInt *inOtherInt );



        /**
         * Gets whether this integer is equal to another integer.
         *
         * @praram inOtherInt the integer test.
         *   Must be destroyed by caller.
         *
         * @return true if this integer is equal to the other.
         */
        char isEqualTo( BigInt *inOtherInt );



        /**
         * Gets a copy of this integer.
         *
         * @return a newly allocated integer containing the copy.
         *   Must be destroyed by caller.
         */
        BigInt *copy();



        /**
         * Gets an integer equal to zero.
         *
         * @return a newly allocated integer containing zero.
         *   Must be destroyed by caller.
         */
        static BigInt *getZero();



        /**
         * Converts this integer to a decimal string.
         *
         * @return a \0-terminated ascii decimal string.
         *   Must be destroyed by caller.
         */
        //char *convertToDecimalString();

        

        /**
         * Converts this integer to a hex string.
         *
         * @return a \0-terminated ascii hexx string.
         *   Must be destroyed by caller.
         */
        char *convertToHexString();



        /**
         * Converts this integer to a 32-bit int.
         *
         * If this integer contains more than 32-bits, the high-order
         * bits will be discarded, though the sign will be preserved.
         */
        int convertToInt();
        
        
        
        /**
         * -1 if negative, +1 if positive, and 0 if zero.
         */
        int mSign;
        
        int mNumBytes;

        /**
         * Integer is stored in big endian byte order.
         */
        unsigned char *mBytes;


        
    protected:


        
        /**
         * Flips the byte order of this integer.
         *
         * @return a newly allocated integer containing the flipped version.
         *   Must be destroyed by caller.
         */ 
        BigInt *flipByteOrder();



        /**
         * Computes the hex representation of a four-bit int.
         *
         * @param inInt the four-bit int to convert.
         *
         * @return the int as a hex ascii character,
         *   in {0, 1, ..., A, B, ..., F}.
         */
        char fourBitIntToHex( int inInt );

        
        
    };



#endif

