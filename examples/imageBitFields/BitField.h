/*
 * Modification History
 *
 * 2001-May-18   Jason Rohrer
 * Created.  
 */
 
 
#ifndef BIT_FIELD_INCLUDED
#define BIT_FIELD_INCLUDED 


/**
 * A two dimensional array of bits.
 *
 * @author Jason Rohrer.
 */
class BitField {



	public:

		/**
		 * Constructs a bit field.
		 *
		 * @param inWidth the width of the field in bits.
		 * @param inHeight the height of the field in bits.
		 */
		BitField( int inWidth, int inHeight );

		~BitField();

		/**
		 * Gets the width of this field.
		 *
		 * @return the width of this field in bits.
		 */
		int getWidth();

		/**
		 * Gets the height of this field.
		 *
		 * @return the height of this field in bits.
		 */
		int getHeight();

		/**
		 * Gets an array representing this field.
		 *
		 * @return an array represeting this field, with
		 *   one char per bit, stored in row-major order.
		 *   The returned array is not a copy, so it
		 *   must not be destroyed by the caller.
		 */
		char *getField();

		
	private:
		int mWidth;
		int mHeight;
		char *mField;

	};



inline BitField::BitField( int inWidth, int inHeight )
	: mWidth( inWidth ), mHeight( inHeight ),
	  mField( new char[ inWidth * inHeight ] ) {

	}



inline BitField::~BitField() {
	delete [] mField;
	}



inline int BitField::getWidth() {
	return mWidth;
	}



inline int BitField::getHeight() {
	return mHeight;
	}



char *BitField::getField() {
	return mField;
	}



#endif
