#include "RaspbianGLSurface.cpp"


#include <math.h>


static void drawStuff() {
    glClear( GL_COLOR_BUFFER_BIT );
    glViewport( 0,
                0,
                720,
                480 );

        
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof( 0, 100, 
              0, 100, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    
    
    glColor4f( 1, 0, 0, 1 );

    GLfloat verts[3*2] = 
        { 30, 30,
          80, 80,
          30, 80 };
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_FLOAT, 0, verts );
    
    //glColorPointer( 4, GL_FLOAT, 0, squareColors );
    //glEnableClientState( GL_COLOR_ARRAY );
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    }


int main() {
    raspbianCreateSurface();
    
    while( true ) {
        drawStuff();
        raspbianSwapBuffers();
        }

    return 1;
    }
