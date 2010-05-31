#include "GraphicBuffer.h"

// function that swaps bufferB to the screen  (32 bit version)
// returns new back buffer
//unsigned long *swapBuffers32( unsigned long *bufferPtrB, short bufferHigh, short bufferWide);

// now replaces "buffer" in bufferB with the new double buffer after swap
// no need to pass back the new buffer
void swapBuffers32( GraphicBuffer &bufferB );




// swap bufferB to the screen  (8 bit version)
// returns new back buffer
unsigned char *swapBuffers8( unsigned char *bufferPtrB, short bufferHigh, short bufferWide );





// NOTE:  the bodies of these functions must occur in the body of the main DDraw code file
//			i.e., in the file where all the directDraw objects are instantiated.
//			these functions assume the ddraw ojects are declared globally