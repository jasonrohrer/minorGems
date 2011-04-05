#include "minorGems/graphics/rgbaColor.h"


rgbaColor *extractJRI( unsigned char *inData, int inNumBytes,
                       int *outWidth, int *outHeight );


unsigned char *generateJRI( rgbaColor *inRGBA, int inWidth, int inHeight,
                            int *outNumBytes );
