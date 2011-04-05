#include "minorGems/graphics/rgbaColor.h"


rgbaColor *extractJRI( unsigned char *inData, int inNumBytes,
                       int *outWidth, int *outHeight );



// returns NULL if inRGBA contains more than 256 colors
unsigned char *generateJRI( rgbaColor *inRGBA, int inWidth, int inHeight,
                            int *outNumBytes );
