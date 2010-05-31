/** qcgrab.exe 
  * 20010224 Jeremy Tavan
  *
  * Rev 1.0 Initial version, seems to work, but doesn't re-establish
  *			broken connections correctly.
 **/


#include "stdafx.h"
#include "resource.h"
#include "windows.h"
#include "stdio.h"

// the following two files are located in the inc directory of the QuickCam
// SDK. These two files contain interface definitions for the video portal
// control.
#include "VPortal2.h"
#include "VPortal2_i.c"

#include <LVServerDefs.H>

// minorGems utilities
#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketServer.h"
#include "minorGems/network/SocketStream.h"
#include "minorGems/io/Stream.h"
#include "minorGems/io/PipedStream.h"
#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/converters/BMPImageConverter.h"


// IVideoPortal is the interface to the video portal control
IVideoPortal* gpVideo = NULL;

// InitializeVideo is used to actually create an instance of the video
// portal control and obtain a IVideoPortal pointer to this interface.
BOOL InitializeVideo( HWND hWnd );

// UnInitializeVideo is used for clean up purposes
BOOL UnInitializeVideo( void );

CComModule _Module;
HWND gMainHwnd = NULL;
DWORD gdwAdviseCookie = 0;

//the following CDriver class implements the connection point to the video
//portal control. This connection point allows the video control to commuicate
//with the application through the PortalNotification event. The
//PortalNotification method is called by the video portal control and is
//handled here.
class CDriver :
	public IDispatchImpl<_IVideoPortalEvents, &IID__IVideoPortalEvents, 
&LIBID_VPORTAL2Lib>,
	public CComObjectRoot
{
public:
		CDriver() {}
BEGIN_COM_MAP(CDriver)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(_IVideoPortalEvents)
END_COM_MAP()

		STDMETHOD(PortalNotification)(
			long lMsg, 
			long lParam1, 
			long lParam2, 
			long lParam3)
{
	// implement any PortalNotification notification handling here.
	switch( lMsg )
	{
		case NOTIFICATIONMSG_MOTION:
		break;
		case NOTIFICATIONMSG_MOVIERECORDERROR:
		break;
		case NOTIFICATIONMSG_CAMERADETACHED:
		break;
		case NOTIFICATIONMSG_CAMERAREATTACHED:
		break;
		case NOTIFICATIONMSG_IMAGESIZECHANGE:
		break;
		case NOTIFICATIONMSG_CAMERAPRECHANGE:
		break;
		case NOTIFICATIONMSG_CAMERACHANGEFAILED:
		break;
		case NOTIFICATIONMSG_POSTCAMERACHANGED:
		break;
		case NOTIFICATIONMSG_CAMERBUTTONCLICKED:
		break;
		case NOTIFICATIONMSG_VIDEOHOOK:
		break;
		case NOTIFICATIONMSG_SETTINGDLGCLOSED:
		break;
		case NOTIFICATIONMSG_QUERYPRECAMERAMODIFICATION:
		break;
		case NOTIFICATIONMSG_MOVIESIZE:
		break;
		default:
		break;
	};
		return S_OK;
	}
};

// the following defines the connection point interface pointer
CComObject<CDriver>* gpDriver;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	//Initializes the COM library on the current apartment and identifies the
	//concurrency model as single-thread apartment (STA).
	CoInitialize(NULL);
	// Initialize the ATL module
	_Module.Init(NULL, hInstance);
	//Initialize ATL control containment code.
	AtlAxWinInit();
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_QCGRAB, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_QCGRAB);

	// Connect to camera
	long lResult;
	gpVideo->ConnectCamera(0, &lResult );
	
	// Figure out how large a buffer we need to store images.  
	// Will usually be 320x240x3 bytes.
	BYTE* pBuffer;
	long lSize;
	gpVideo->PictureToMemory(0, 24, NULL, &lSize, NULL, &lResult );
	pBuffer = new BYTE[lSize]; // allocate buffer
	

	// Start the network socket server on port 3000
	SocketServer *netServ = new SocketServer( 3000 , 50 );
	Socket *socket = netServ->acceptConnection();
	SocketStream *sockStream = new SocketStream( socket );
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		// if we've lost the connection, wait for it to be re-established
		if (sockStream->getLastError()) {
			delete socket;
			Socket *socket = netServ->acceptConnection();
			sockStream = new SocketStream( socket );
		}
		
		PipedStream *stream = new PipedStream();
		BMPImageConverter *converter = new BMPImageConverter();
		if (pBuffer) {
			// Got a buffer, capture frame into it
			gpVideo->PictureToMemory(0, 24, (long)pBuffer, &lSize, NULL,
				&lResult );		
		}

		// Convert into an Image and send it to the connected client
		stream->write( pBuffer , lSize);
		Image *image = converter->deformatImage( stream );
		image->serialize( sockStream );
		// And clean up after ourselves.
		delete stream;
		delete converter;
		delete image;
	
		// Handle routine Windows stuff like mouse events.
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
    free(pBuffer);
	UnInitializeVideo(); // clean up the video control...
	_Module.Term();
	CoUninitialize();

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_QCGRAB);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_QCGRAB;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
	gMainHwnd = hWnd;
	InitializeVideo( hWnd );

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			RECT rt;
			GetClientRect(hWnd, &rt);
			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// IntializeVideo creates the video portal control instance, obtains the
// connection point object, and connects the video portal to a camera device.
BOOL InitializeVideo( HWND hParent )
{
	long lResult;
	USES_CONVERSION;
	long numCams;
	
	
	LPOLESTR strGUIDVideoPortalActiveXControl = NULL;
	StringFromCLSID(CLSID_VideoPortal, &strGUIDVideoPortalActiveXControl);
	// first we create a window which contains the video portal control.
	// notice the class name as “AtlAxWin”, and the window name
	// is the CLSID of the video portal control itself…
	HWND hWndCtl = ::CreateWindow( "AtlAxWin",
	OLE2T(strGUIDVideoPortalActiveXControl),
	WS_CHILD | WS_VISIBLE | WS_GROUP,
	0,
	0,
	400,
	400,
	hParent,
	NULL,
	::GetModuleHandle(NULL),
	NULL );
	CoTaskMemFree(strGUIDVideoPortalActiveXControl );
	if(!hWndCtl)
	{
		return FALSE;
	}
	// the following method retrieves the IVideoPortal interface from the HWND
	// of the window we just created.
	if ( FAILED( AtlAxGetControl(hWndCtl, (IUnknown **)&gpVideo)) )
	{
		return FALSE;
	}
	// the following instantiates the CDriver connection point object
	CComObject<CDriver>::CreateInstance(&gpDriver);
	// the following assigns the CDriver connection point to the video
	// portal’s connection point.
	if ( FAILED ( AtlAdvise(gpVideo, gpDriver->GetUnknown(),
	IID__IVideoPortalEvents, &gdwAdviseCookie) ) )
	{
		gpVideo->Release();
		gpVideo = NULL;
		return FALSE;
	}
	// now we have everything we need to start communicating with the
	// video portal. The gpVideo object is the interface to the video
	// portal
	WCHAR wstrKey[] = L"HKEY_CURRENT_USER\\Software\\TestApp1";
	WCHAR wstrUnique[] = L"Aaron";
	BSTR bStrKey = ::SysAllocString(wstrKey);
	BSTR bStrUnique = ::SysAllocString(wstrUnique);
	// the following line initializes the video control to be used by
	// the application
	if ( FAILED(gpVideo->PrepareControl( bStrUnique, bStrKey, 0, &lResult)) )
	{
		::SysFreeString(bStrKey);
		::SysFreeString(bStrUnique);
		UnInitializeVideo();
		return FALSE;
	}
	::SysFreeString(bStrUnique);
	gpVideo->put_PreviewMaxWidth( 320 );
	gpVideo->put_PreviewMaxHeight( 240 );
	// next we turn on the video portal’s status bar
	if ( FAILED(gpVideo->EnableUIElements( UIELEMENT_STATUSBAR,
	0,
	TRUE,
	&lResult )) )
	{
	}

	gpVideo->GetCameraCount(&numCams,&lResult);

	// next, we connect to a camera device
	if ( FAILED(gpVideo->ConnectCamera(0, &lResult )) )
	{
		::SysFreeString(bStrKey);
		UnInitializeVideo();
		return FALSE;
	}
	::SysFreeString(bStrKey);
	// finally, we tell the video portal, to enable video preview.
	gpVideo->put_EnablePreview(TRUE);
	return FALSE;
}

// UnInitialize video performs cleanup of the video portal when
//it is no longer needed.
BOOL UnInitializeVideo( void )
{
	if ( gpDriver )
	{
		AtlUnadvise(gpVideo,IID__IVideoPortalEvents, gdwAdviseCookie);
		gpDriver = NULL;
	}
	if ( gpVideo )
	{
		gpVideo->Release();
		gpVideo = NULL;
	}
	return TRUE;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
