// 32 bit DDraw pluggable framework
// Jason Rohrer, 4-28-99

/**
*	Mods:	
*		Jason Rohrer	11-8-99		Changed to use GraphicBuffer object as screen buffer
*		Jason Rohrer	3-21-2000	Added support for mouse querying
*/


// based on the work of:

/*
 * water ripples effect
 * --------------------
 *  anthony greene :: emit
 *   april 1999 
 */

#define WIN32_LEAN_AND_MEAN  // make sure certain headers are included correctly


/* includes */

#include <windows.h>         // include the standard windows stuff
//#include <windowsx.h>        // include the 32 bit stuff
//#include <mmsystem.h>        // include the multi media stuff
							 // need winmm.lib also
#include <ddraw.h>           // include direct draw components

#include <stdlib.h>			 


#include "swapBuffers.h"	// interface for swapping buffers
#include "getKey.h"		// interface for handling keyboard input
#include "getMouse.h"	// interface for handling mouse input


#include "GraphicBuffer.h"

//#include "ALifeGuiRunner.h"
#include "GoGUIRunner.h"



/* defines */

#define WINDOW_CLASS_NAME "WINDOW_CLASS"	// this is the name of the window class
#define SCREEN_WIDTH     640				// the width of the viewing surface
#define SCREEN_HEIGHT    480				// the height of the viewing surface
#define SCREEN_BPP       32					// the bits per pixel
#define MAX_COLORS       256				// the maximum number of colors


/* types */ 

typedef unsigned char  BYTE;
typedef unsigned char  UCHAR;
typedef unsigned short WORD;


/* macros */

// these query the keyboard in real-time
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)


/* prototypes */

int DD_Init(HWND hwnd);
int DD_Shutdown(void);

void graphixPicker(void);		// our mother function, picks effect order, etc.


/* directdraw globals */

LPDIRECTDRAW         lpdd         = NULL;		// dd object
LPDIRECTDRAWSURFACE  lpddsprimary = NULL;		// dd primary surface
LPDIRECTDRAWSURFACE  lpddsback    = NULL;		// dd back surface
LPDIRECTDRAWPALETTE  lpddpal      = NULL;		// a pointer to the created dd palette
PALETTEENTRY         color_palette[256];		// holds the shadow palette entries
DDSURFACEDESC        ddsd;						// a direct draw surface description struct
DDSCAPS              ddscaps;					// a direct draw surface capabilities struct
HRESULT              ddrval;					// result back from dd calls
HWND                 main_window_handle = NULL; // used to store the window handle

unsigned long        *double_buffer = NULL;     // the double buffer
unsigned long		 *screen_buffer = NULL;

GraphicBuffer *graphicDoubleBuffer;


/* globals */

int Height[2][640*480];
int old;
int nu;
int running = 0;
int cycle;

/* direct x functions */

int DD_Init(HWND hwnd)
{
	// this function is responsible for initializing direct draw,
	// it creates a primary surface 

	int index;     // looping index

	// now that the windows portion is complete, start up direct draw
	if (DirectDrawCreate(NULL,&lpdd,NULL)!=DD_OK)
	{
		// shutdown any other dd objects and kill window
		DD_Shutdown();
		return(0);
	}

	// now set the coop level to exclusive and set for full screen and mode x
	if (lpdd->SetCooperativeLevel(hwnd, DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE |
		                          DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX)!=DD_OK)
	{
		// shutdown any other dd objects and kill window
		DD_Shutdown();
		return(0);
	}

	// now set the display mode
	if (lpdd->SetDisplayMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP)!=DD_OK)
	{
		// shutdown any other dd objects and kill window
		DD_Shutdown();
		return(0);
	}

	// Create the primary surface
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize            = sizeof(ddsd);
	ddsd.dwFlags           = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE;

	if (lpdd->CreateSurface(&ddsd,&lpddsprimary,NULL)!=DD_OK)
	{
		// shutdown any other dd objects and kill window
		DD_Shutdown();
		return(0);
	} // end if

	// allocate memory for the double buffer
	if ((double_buffer = (unsigned long *)malloc(SCREEN_WIDTH*SCREEN_HEIGHT*4))==NULL)
		return(0);

	// create GraphicBuffer object
	graphicDoubleBuffer = new GraphicBuffer( double_buffer, 640, 480 );


	// create the palette and attach it to the primary surface
	// clear all the palette entries to RGB 0,0,0
	memset(color_palette,0,256*sizeof(PALETTEENTRY));

	// set all of the flags to the correct value
	int c=0;
	for (index=0; index<256; index++)
	{
		// create the GRAY/RED/GREEN/BLUE palette, 64 shades of each
		if (index < 64) {
			color_palette[index].peRed   = index;
			color_palette[index].peGreen = 0;
			color_palette[index].peBlue  = 0;
		} else if (index < 128) {
			color_palette[index].peRed   = index;
			color_palette[index].peGreen = 0;
			color_palette[index].peBlue  = 0;
		} else if (index < 192) { 
			color_palette[index].peRed   = index;
			color_palette[index].peGreen = c;
			color_palette[index].peBlue  = c;
			c++;
		} else if (index < 256) { 
			color_palette[index].peRed   = index;
			color_palette[index].peGreen = c;
			color_palette[index].peBlue  = c;
			c++;
		}
		// set the no collapse flag
		color_palette[index].peFlags = PC_NOCOLLAPSE;
	}

	// now create the palette object, note that it is a member of the dd object itself
	if (lpdd->CreatePalette((DDPCAPS_8BIT | DDPCAPS_INITIALIZE),color_palette,&lpddpal,NULL)!=DD_OK)
	{
		// shutdown any other dd objects and kill window
		DD_Shutdown();
		return(0);
	}

	// now attach the palette to the primary surface
	lpddsprimary->SetPalette(lpddpal);

	// return success if we got this far
	return(1);
} 


int DD_Shutdown(void)
{
	// this function tests for dd components that have been created 
	// and releases them back to the operating system
	
	// test if the dd object exists
	if (lpdd)
	{
		// test if there is a primary surface
		if(lpddsprimary)
		{
			// release the memory and set pointer to NULL
			lpddsprimary->Release();
			lpddsprimary = NULL;
		}

		// now release the dd object itself
		lpdd->Release();
		lpdd = NULL;

		// free double buffer
		if (double_buffer!=NULL)
			free(double_buffer);

		// return success
		return(1);
	} 
	else
		return(0);
} 


/* windows callback fucntion */

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// this is the main message handler of the system

	HDC			hdc;		// handle to graphics context
	PAINTSTRUCT ps;			// used to hold the paint info

	// what is the message..
	switch(msg)
	{	
		case WM_CREATE: {
			// do windows inits here
			return(0);
		} break;

		case WM_PAINT: {
			// this message occurs when your window needs repainting
			hdc = BeginPaint(hwnd,&ps);	 
			EndPaint((struct HWND__ *)hdc,&ps);		
			return(0);
   		} break;

		case WM_DESTROY: {
			// this message is sent when your window is destroyed
			PostQuitMessage(0);
			return(0);
		} break;
		
		case WM_MOUSEMOVE: {
			// extract x,y 
			/*
			int mouse_x = (int)LOWORD(lparam);
			int mouse_y = (int)HIWORD(lparam);
			Height[old][mouse_y*640+mouse_x] = 300;
			*/
	        return(0);
        } break;

		default:break;
    } 

	// let windows process any messages that we didn't take care of 
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}


int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{
	WNDCLASSEX		winclass;	// this holds the windows class info
	HWND			hwnd;		// this holds the handle of our new window
	MSG				msg;		// this holds a generic message
	
	// first fill in the window class stucture

	winclass.cbSize			= sizeof(WNDCLASSEX);
	winclass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground	= (struct HBRUSH__ *)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL;
	winclass.lpszClassName	= WINDOW_CLASS_NAME;

	// register the window class
	if (!RegisterClassEx(&winclass))
		return(0);

	// create the window
	if (!(hwnd = CreateWindowEx(WS_EX_TOPMOST,
							WINDOW_CLASS_NAME,					// class
							"water",							// title
							WS_VISIBLE | WS_POPUP,
					 		0,0,								// x,y
							GetSystemMetrics(SM_CXSCREEN),
							GetSystemMetrics(SM_CYSCREEN),				
							NULL,								// parent
							NULL,								// menu
							hinstance,							// instance
							NULL)))								// creation parms
	return(0);

	// hide the mouse cursor
	ShowCursor(0);

	// save the window handle
	main_window_handle = hwnd;

	// initialize direct draw
	if (!DD_Init(hwnd)) {
		DestroyWindow(hwnd);
		return(0);
	}

	
	// peek at the message once just to make them happy
	PeekMessage(&msg,NULL,0,0,PM_REMOVE);
	
	// do it once, then leave
	
	graphixPicker();		// picks which effects to run.

	
	// afterwards, end!!!



	// shut down direct draw
	DD_Shutdown();

	// return to Windows
	return(msg.wParam);
	
} 






void graphixPicker() {		// our "mother function"  
							// selects which effect to run next
	
	// graphix plug-in format:
		// myGraphix( bufferPtr, bufferHigh, bufferWide, colorBits, numFrames)
		// function can cll "swapBuffer32" internally whenever it needs the back buffer
		//		sent to the screen.
		
			
	
	//	jcrTorus( double_buffer, 480, 640, 32, 100);
	//	jcrVoxels( double_buffer, 480, 640, 32, 230);
	//	jcrTorus( double_buffer, 480, 640, 32, 50);
	//	jcrBloom( double_buffer, 480, 640, 32, 100);					
	
	
	//ALifeGuiRunner( *graphicDoubleBuffer, 200 );	
	GoGUIRunner( *graphicDoubleBuffer, 0 );
	}




char getKeyDown( int vKeyCode ) { 
	return ((GetAsyncKeyState(vKeyCode) & 0x8000) ? true : false);
	}
char getKeyUp( int vKeyCode ) {
	return ((GetAsyncKeyState(vKeyCode) & 0x8000) ? false : true);
	}

void getMouse( int *x, int *y ) {
	POINT p;
	
	GetCursorPos( &p );
	
	*x = p.x;
	*y = p.y;
	}



/// CODE FOR SWAPPING BUFFERES BELOW THIS POINT.......





// swap bufferB to the screen (32 bit version)
void swapBuffers32( GraphicBuffer &bufferB ) {
	
	unsigned long *primary_buffer = NULL, // used to draw
	      *dest_ptr       = NULL, // used in line by line copy
	      *src_ptr        = NULL; // " "

		short bufferHigh = 480;
		short bufferWide = 640;
	
	
		// copy the double buffer into the primary buffer
		memset(&ddsd,0,sizeof(ddsd)); 
		ddsd.dwSize = sizeof(ddsd);

		// lock the primary surface
		lpddsprimary->Lock(NULL,&ddsd, 
              DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,NULL);

		// get video pointer to primary surfce
		primary_buffer = (unsigned long *)ddsd.lpSurface;       

		// test if memory is linear
		if (ddsd.lPitch == 640*4)
		{
			// copy memory from double buffer to primary buffer
			memcpy(primary_buffer, double_buffer, 640*480*4);
		}
		else
		{	// non-linear
			// make copy of source and destination addresses
			dest_ptr = primary_buffer;
			src_ptr = double_buffer;
	
			// memory is non-linear, copy line by line
			for (int y=0; y<bufferHigh; y++)
			{
				// copy line
				memcpy(dest_ptr, src_ptr, bufferWide*4);	

				// advance pointers to next line
				dest_ptr+=ddsd.lPitch;
				src_ptr+=bufferWide*4;

				// note: the above code code be replaced with the simpler
				// memcpy(&primary_buffer[y*ddsd.lPitch], double_buffer[y*bufferWide], bufferWide);
				// but it is much slower due to the recalculation and multiplication each cycle
	       } 
	   } 
	

	// unlock primary buffer
	lpddsprimary->Unlock(primary_buffer);

	}		// end of swapBuffers




