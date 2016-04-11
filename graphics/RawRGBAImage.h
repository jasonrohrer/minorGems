#ifndef RAW_RGBA_IMAGE_INCLUDED
#define RAW_RGBA_IMAGE_INCLUDED



// NOTE that it can contain 3 or 4 channels (the A is optional)
class RawRGBAImage {
    
    public:
        
        // bytes are NOT copied internally and are destroyed when
        // this object is destroyed
        RawRGBAImage( unsigned char *inRGBABytes, int inWidth, int inHeight,
                      int inNumChannels ) 
            : mRGBABytes( inRGBABytes ), 
              mWidth( inWidth ), mHeight( inHeight ),
              mNumChannels( inNumChannels ) {
            }
        
            
        
        ~RawRGBAImage() {
            if( mRGBABytes != NULL ) {
                delete [] mRGBABytes;
                }
            }
        

        unsigned char *mRGBABytes;
        
        unsigned int mWidth;
        unsigned int mHeight;

        unsigned int mNumChannels;
    };


#endif
