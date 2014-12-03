#ifdef __mac__

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>


#elif defined(RASPBIAN)

// GL ES
#include <GLES/gl.h>

// some things that aren't defined in GLES or have different names
#define GLdouble     GLfloat
#define GL_CLAMP     GL_CLAMP_TO_EDGE
#define glClearDepth glClearDepthf
#define glOrtho      glOrthof
#define glFrustum    glFrustumf
#define glGetDoublev glGetFloatv
#define GL_SOURCE0_RGB GL_SRC0_RGB
#define GL_SOURCE0_ALPHA GL_SRC0_ALPHA

// regular mesa-supplied GLU should work
#include <GL/glu.h>




#else

#include <GL/gl.h>
#include <GL/glu.h>



#ifdef WIN_32
// on Windows, some stuff that's normally in gl.h (1.2 and 1.3 stuff) is
// in glext
#include <GL/glext.h>
#endif


#endif
