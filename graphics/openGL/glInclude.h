#ifdef __mac__

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#else

#include <GL/gl.h>
#include <GL/glu.h>



#ifdef WIN_32
// on Windows, some stuff that's normally in gl.h (1.2 and 1.3 stuff) is
// in glext
#include <GL/glext.h>
#endif


#endif
