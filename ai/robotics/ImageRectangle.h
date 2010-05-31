/*
 * Modification History
 *
 * 2001-February-27		Jason Rohrer
 * Created.     
 */
 
 
#ifndef IMAGE_RECTANGLE_INCLUDED
#define IMAGE_RECTANGLE_INCLUDED 


/**
 * Representation of a rectangular region in an image.
 *
 * @author Jason Rohrer
 */ 
class ImageRectangle {
	
	public:
		
		/**
		 * All values are in [0,1].
		 */
		double mXStart;
		double mXEnd;
		double mYStart;
		double mYEnd;
		
		/**
		 * Constructs an ImageRectangle.
		 *
		 * @param inXStart the left boundary of the rectangle, in [0,1].
		 * @param inXEnd the right boundary of the rectangle, in [0,1].
		 * @param inYStart the top boundary of the rectangle, in [0,1].
		 * @param inYEnd the bottom boundary of the rectangle, in [0,1].
		 */
		ImageRectangle( double inXStart, double inXEnd, 
			double inYStart, double inYEnd );
		
			
	};
	
	
	
inline ImageRectangle::ImageRectangle( double inXStart, double inXEnd, 
	double inYStart, double inYEnd ) 
	: mXStart( inXStart ), mXEnd( inXEnd ), mYStart( inYStart ),
	mYEnd( inYEnd ) {
	
	}	
		

		
#endif
