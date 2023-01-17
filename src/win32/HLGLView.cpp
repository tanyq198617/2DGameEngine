#include "HLGLView.h"
#include "HLApplication.h"
#include "HLGL.h"
#include "HLMouseDispatcher.h"
#include "HLDirector2D.h"
#include <wincon.h>

#ifdef _DEBUG
#include   <io.h>
#include   <fcntl.h>
void   OpenConsole()
{    
	AllocConsole();    
	//SetConsoleOutputCP(CP_UTF8);
	//SetConsoleCP(CP_UTF8);
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);    
	int hCrt = _open_osfhandle((long)handle,_O_TEXT);
	FILE * hf = _fdopen(hCrt, "w");    
	*stdout = *hf;    
}
#endif

namespace hoolai
{
	void _setMultiTouchEnabled(bool enable)
	{

	}
}

NS_HL_BEGIN

struct timeval mousetimestamp;

static void SetupPixelFormat(HDC hDC)
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size
		1,                          // version
		PFD_SUPPORT_OPENGL |        // OpenGL window
		PFD_DRAW_TO_WINDOW |        // render to window
		PFD_DOUBLEBUFFER,           // support double-buffering
		PFD_TYPE_RGBA,              // color type
		32,                         // preferred color depth
		0, 0, 0, 0, 0, 0,           // color bits (ignored)
		0,                          // no alpha buffer
		0,                          // alpha bits (ignored)
		0,                          // no accumulation buffer
		0, 0, 0, 0,                 // accum bits (ignored)
		24,                         // depth buffer
		8,                          // no stencil buffer
		0,                          // no auxiliary buffers
		PFD_MAIN_PLANE,             // main layer
		0,                          // reserved
		0, 0, 0,                    // no layer, visible, damage masks
	};

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);
}

static bool glew_dynamic_binding()
{
	const char *gl_extensions = (const char*)glGetString(GL_EXTENSIONS);

	// If the current opengl driver doesn't have framebuffers methods, check if an extension exists
	if (glGenFramebuffers == NULL)
	{
		HLLOG("OpenGL: glGenFramebuffers is NULL, try to detect an extension\n");
		if (strstr(gl_extensions, "ARB_framebuffer_object"))
		{
			HLLOG("OpenGL: ARB_framebuffer_object is supported\n");

			glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglGetProcAddress("glIsRenderbuffer");
			glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbuffer");
			glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglGetProcAddress("glDeleteRenderbuffers");
			glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffers");
			glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorage");
			glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetRenderbufferParameteriv");
			glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglGetProcAddress("glIsFramebuffer");
			glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebuffer");
			glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffers");
			glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffers");
			glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatus");
			glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) wglGetProcAddress("glFramebufferTexture1D");
			glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2D");
			glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) wglGetProcAddress("glFramebufferTexture3D");
			glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbuffer");
			glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
			glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmap");
		}
		else
			if (strstr(gl_extensions, "EXT_framebuffer_object"))
			{
				HLLOG("OpenGL: EXT_framebuffer_object is supported\n");
				glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglGetProcAddress("glIsRenderbufferEXT");
				glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbufferEXT");
				glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglGetProcAddress("glDeleteRenderbuffersEXT");
				glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffersEXT");
				glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorageEXT");
				glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetRenderbufferParameterivEXT");
				glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglGetProcAddress("glIsFramebufferEXT");
				glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebufferEXT");
				glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffersEXT");
				glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffersEXT");
				glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");
				glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) wglGetProcAddress("glFramebufferTexture1DEXT");
				glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2DEXT");
				glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) wglGetProcAddress("glFramebufferTexture3DEXT");
				glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbufferEXT");
				glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
				glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmapEXT");
			}
			else
			{
				HLLOG("OpenGL: No framebuffers extension is supported\n");
				HLLOG("OpenGL: Any call to Fbo will crash!\n");
				return false;
			}
	}
	return true;
}

static HLGLView* s_pMainWindow = NULL;
static const WCHAR* kWindowClassName = L"HoolaiEngine";

static LRESULT CALLBACK _WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (s_pMainWindow && s_pMainWindow->getHWnd() == hWnd)
	{
		return s_pMainWindow->WindowProc(uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

HWND HLGLView::getHWnd()
{
	return m_hWnd;
}

HLGLView::HLGLView()
	: m_bCaptured(false)
	, m_hWnd(NULL)
	, m_hDC(NULL)
	, m_hRC(NULL)
	, m_lpfnAccelerometerKeyHook(NULL)
	, m_menu(NULL)
	, m_wndproc(NULL)
{
	gettimeofday(&mousetimestamp, 0);
}

bool HLGLView::initGL()
{
	m_hDC = GetDC(m_hWnd);
	SetupPixelFormat(m_hDC);
	//SetupPalette();
	m_hRC = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, m_hRC);

	// check OpenGL version at first
	const GLubyte* glVersion = glGetString(GL_VERSION);
	HLLOG("OpenGL version = %s", glVersion);

	if ( atof((const char*)glVersion) < 1.5 )
	{
		char strComplain[256] = {0};
		sprintf(strComplain,
			"OpenGL 1.5 or higher is required (your version is %s). Please upgrade the driver of your video card.",
			glVersion);
		HLLOG(strComplain);
		return false;
	}

	GLenum GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
		HLLOG("OpenGL error %s", (char *)glewGetErrorString(GlewInitResult));
		return false;
	}

	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
	{
		HLLOG("Ready for GLSL");
	}
	else
	{
		HLLOG("Not totally ready :(");
	}

	if (glewIsSupported("GL_VERSION_2_0"))
	{
		HLLOG("Ready for OpenGL 2.0");
	}
	else
	{
		HLLOG("OpenGL 2.0 not supported");
	}

	if(glew_dynamic_binding() == false)
	{
		HLLOG("No OpenGL framebuffer support. Please upgrade the driver of your video card.", "OpenGL error");
		return false;
	}

	// Enable point size by default on windows.
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	return true;
}

void HLGLView::destroyGL()
{
	if (m_hDC != NULL && m_hRC != NULL)
	{
		// deselect rendering context and delete it
		wglMakeCurrent(m_hDC, NULL);
		wglDeleteContext(m_hRC);
	}
}

bool HLGLView::Create()
{
	bool bRet = false;
	do
	{
		if(m_hWnd)
			break;

		HINSTANCE hInstance = GetModuleHandle( NULL );
		WNDCLASS  wc;        // Windows Class Structure

		// Redraw On Size, And Own DC For Window.
		wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc    = _WindowProc;                    // WndProc Handles Messages
		wc.cbClsExtra     = 0;                              // No Extra Window Data
		wc.cbWndExtra     = 0;                                // No Extra Window Data
		wc.hInstance      = hInstance;                        // Set The Instance
		wc.hIcon          = LoadIcon( NULL, IDI_WINLOGO );    // Load The Default Icon
		wc.hCursor        = LoadCursor( NULL, IDC_ARROW );    // Load The Arrow Pointer
		wc.hbrBackground  = NULL;                           // No Background Required For GL
		wc.lpszMenuName   = m_menu;                         //
		wc.lpszClassName  = kWindowClassName;               // Set The Class Name

		if(! RegisterClass(&wc) && 1410 != GetLastError())
			break;

		// center window position
		RECT rcDesktop;
		GetWindowRect(GetDesktopWindow(), &rcDesktop);

		WCHAR wszBuf[50] = {0};
		MultiByteToWideChar(CP_UTF8, 0, m_szViewName, -1, wszBuf, sizeof(wszBuf));

		// create window
		m_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,    // Extended Style For The Window
			kWindowClassName,                                    // Class Name
			wszBuf,                                                // Window Title
			WS_CAPTION | WS_POPUPWINDOW | WS_MINIMIZEBOX,        // Defined Window Style
			0, 0,                                                // Window Position
			//TODO: Initializing width with a large value to avoid getting a wrong client area by 'GetClientRect' function.
			1000,                                               // Window Width
			1000,                                               // Window Height
			NULL,                                                // No Parent Window
			NULL,                                                // No Menu
			hInstance,                                            // Instance
			NULL );

		if(! m_hWnd)
			break;

		bRet = initGL();
		if(!bRet) destroyGL();
		if(!bRet) break;

		s_pMainWindow = this;
		bRet = true;
	} while (0);
#ifdef _DEBUG
	OpenConsole();
#endif
	return bRet;
}

LRESULT HLGLView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bProcessed = FALSE;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		{
			POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
			HLPoint pt(point.x, point.y);
			pt = HLDirector2D::getSingleton()->convertToGLPoint(pt);
/*			struct timeval now;
			gettimeofday(&now, 0);
			float delta = (now.tv_sec - mousetimestamp.tv_sec) + (now.tv_usec - mousetimestamp.tv_usec) / 1000000.0f;
			if (delta > 0.5f)
			{
				HLMouseDispatcher::sharedMouseDispatcher()->hitTest(pt);
			}
			gettimeofday(&mousetimestamp, 0);*/
			HLMouseEvent evt;
			evt.clickCount = 1;
			evt.location = pt;
			evt.type = kHLMouseDown;
			HLMouseDispatcher::sharedMouseDispatcher()->onMouseEvent(evt);
		}
		break;
	case WM_LBUTTONUP:
		{
			POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
			HLPoint pt(point.x, point.y);
			pt = HLDirector2D::getSingleton()->convertToGLPoint(pt);
			HLMouseEvent evt;
			evt.clickCount = 1;
			evt.location = pt;
			evt.type = kHLMouseUp;
			HLMouseDispatcher::sharedMouseDispatcher()->onMouseEvent(evt);
		}
		break;
	case WM_MOUSEMOVE:
		{
			POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
			HLPoint pt(point.x, point.y);
			pt = HLDirector2D::getSingleton()->convertToGLPoint(pt);
			HLMouseEvent evt;
			evt.clickCount = 1;
			evt.location = pt;
			if (MK_LBUTTON == wParam)
			{
				evt.type = kHLMouseDrag;
			}
			else if (MK_RBUTTON == wParam)
			{
				evt.type = kHLRightMouseDrag;
			}
			else
			{
				evt.type = kHLMouseMove;
			}
			HLMouseDispatcher::sharedMouseDispatcher()->onMouseEvent(evt);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
			HLPoint pt(point.x, point.y);
			pt = HLDirector2D::getSingleton()->convertToGLPoint(pt);
			struct timeval now;
			gettimeofday(&now, 0);
			float delta = (now.tv_sec - mousetimestamp.tv_sec) + (now.tv_usec - mousetimestamp.tv_usec) / 1000000.0f;
			if (delta > 0.5f)
			{
				HLMouseDispatcher::sharedMouseDispatcher()->hitTest(pt);
			}
			gettimeofday(&mousetimestamp, 0);
			HLMouseEvent evt;
			evt.clickCount = 1;
			evt.location = pt;
			evt.type = kHLRightMouseDown;
			HLMouseDispatcher::sharedMouseDispatcher()->onMouseEvent(evt);
		}
		break;
	case WM_RBUTTONUP:
		{
			POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
			HLPoint pt(point.x, point.y);
			pt = HLDirector2D::getSingleton()->convertToGLPoint(pt);
			HLMouseEvent evt;
			evt.clickCount = 1;
			evt.location = pt;
			evt.type = kHLRightMouseUp;
			HLMouseDispatcher::sharedMouseDispatcher()->onMouseEvent(evt);
		}
		break;
	case WM_SIZE:
		{

		}
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(m_hWnd, &ps);
		EndPaint(m_hWnd, &ps);
		break;

	case WM_CLOSE:
		HLApplication::sharedApplication()->applicationWillTerminate();
		exit(0);
		break;

	case WM_DESTROY:
		destroyGL();
		PostQuitMessage(0);
		break;

	default:
		if (m_wndproc)
		{

			m_wndproc(message, wParam, lParam, &bProcessed);
			if (bProcessed) break;
		}
		return DefWindowProc(m_hWnd, message, wParam, lParam);
	}
	
	if (m_wndproc && !bProcessed)
	{
		m_wndproc(message, wParam, lParam, &bProcessed);
	}
	return 0;
}

void HLGLView::swapBuffers()
{
	if (m_hDC != NULL)
	{
		::SwapBuffers(m_hDC);
	}
}

void HLGLView::centerWindow()
{
	if (! m_hWnd)
	{
		return;
	}

	RECT rcDesktop, rcWindow;
	GetWindowRect(GetDesktopWindow(), &rcDesktop);

	// substract the task bar
	HWND hTaskBar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
	if (hTaskBar != NULL)
	{
		APPBARDATA abd;

		abd.cbSize = sizeof(APPBARDATA);
		abd.hWnd = hTaskBar;

		SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
		SubtractRect(&rcDesktop, &rcDesktop, &abd.rc);
	}
	GetWindowRect(m_hWnd, &rcWindow);

	int offsetX = (rcDesktop.right - rcDesktop.left - (rcWindow.right - rcWindow.left)) / 2;
	offsetX = (offsetX > 0) ? offsetX : rcDesktop.left;
	int offsetY = (rcDesktop.bottom - rcDesktop.top - (rcWindow.bottom - rcWindow.top)) / 2;
	offsetY = (offsetY > 0) ? offsetY : rcDesktop.top;

	SetWindowPos(m_hWnd, 0, offsetX, offsetY, 0, 0, SWP_NOCOPYBITS | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

void HLGLView::setWinSize(int width, int height)
{
	if (! m_hWnd)
	{
		return;
	}

	RECT rcWindow;
	GetWindowRect(m_hWnd, &rcWindow);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	// calculate new window width and height
	POINT ptDiff;
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	rcClient.right = rcClient.left + width;
	rcClient.bottom = rcClient.top + height;

	if (width > 0)
	{
		WCHAR wszBuf[MAX_PATH] = {0};
#ifdef _DEBUG
		char szBuf[MAX_PATH + 1];
		memset(szBuf, 0, sizeof(szBuf));
		_snprintf(szBuf, MAX_PATH, "%s - %dx%d",
			m_szViewName, width, height);
		MultiByteToWideChar(CP_UTF8, 0, szBuf, -1, wszBuf, sizeof(wszBuf));
#else
		MultiByteToWideChar(CP_UTF8, 0, m_szViewName, -1, wszBuf, sizeof(wszBuf));
#endif
		SetWindowText(m_hWnd, wszBuf);
	}

	AdjustWindowRectEx(&rcClient, GetWindowLong(m_hWnd, GWL_STYLE), FALSE, GetWindowLong(m_hWnd, GWL_EXSTYLE));

	// change width and height
	SetWindowPos(m_hWnd, 0, 0, 0, width + ptDiff.x, height + ptDiff.y,
		SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	centerWindow();

	HLSize size(width, height);
	HLDirector2D::getSingleton()->reshapeProjection(size);
}

HLGLView* HLGLView::sharedOpenGLView()
{
	static HLGLView* instance = NULL;
	if (!instance)
	{
		instance = new HLGLView();
		if (!instance->Create())
		{
			delete instance;
			instance = NULL;
		}
	}
	return instance;
}

NS_HL_END