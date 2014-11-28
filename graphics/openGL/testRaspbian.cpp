#include "RaspbianGLSurface.cpp"


int main() {
    raspbianCreateSurface();
    
    while( true ) {
        raspbianSwapBuffers();
        }

    return 1;
    }
