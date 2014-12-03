#include "bcm_host.h"

#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include <assert.h>




static EGLDisplay display;
static EGLSurface surface;
static EGLContext context;

static uint32_t screen_width;
static uint32_t screen_height;


static void raspbianCreateSurface() {

    int32_t success = 0;
    EGLBoolean result;
    EGLint num_config;

    static EGL_DISPMANX_WINDOW_T nativewindow;

    DISPMANX_ELEMENT_HANDLE_T dispman_element;
    DISPMANX_DISPLAY_HANDLE_T dispman_display;
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;

    static const EGLint attribute_list[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
        };
   
    EGLConfig config;

    bcm_host_init();

    // get an EGL display connection
    display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
    assert( display!=EGL_NO_DISPLAY );

    // initialize the EGL display connection
    result = eglInitialize( display, NULL, NULL );
    assert( EGL_FALSE != result );

    // get an appropriate EGL frame buffer configuration
    result = eglChooseConfig( display, attribute_list, 
                              &config, 1, &num_config );
    assert( EGL_FALSE != result );

    // create an EGL rendering context
    context = eglCreateContext(  display, config, EGL_NO_CONTEXT, NULL );
    assert( context!=EGL_NO_CONTEXT );

    // create an EGL window surface
    success = graphics_get_display_size( 0 /* LCD */, 
                                         &screen_width, &screen_height );
    assert(  success >= 0 );

    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = screen_width;
    dst_rect.height = screen_height;
      
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = screen_width << 16;
    src_rect.height = screen_height << 16;        

    dispman_display = vc_dispmanx_display_open( 0 /* LCD */ );
    dispman_update = vc_dispmanx_update_start( 0 );
         
    dispman_element = vc_dispmanx_element_add ( 
        dispman_update, dispman_display,
        0/*layer*/, &dst_rect, 0/*src*/,
        &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 
        0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/ );
   
    nativewindow.element = dispman_element;
    nativewindow.width = screen_width;
    nativewindow.height = screen_height;
    vc_dispmanx_update_submit_sync( dispman_update );
      
    surface = eglCreateWindowSurface( display, config, &nativewindow, NULL );
    assert( surface != EGL_NO_SURFACE );

    // connect the context to the surface
    result = eglMakeCurrent( display, surface, surface, context );
    assert( EGL_FALSE != result );
    }




static void raspbianSwapBuffers() {
    eglSwapBuffers( display, surface );
    }



static void raspbianReleaseSurface() {
    // clear screen
    glClear( GL_COLOR_BUFFER_BIT );
    eglSwapBuffers( display, surface );
    
    // Release OpenGL resources
    eglMakeCurrent( display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
    eglDestroySurface( display, surface );
    eglDestroyContext( display, context );
    eglTerminate( display );
    }
