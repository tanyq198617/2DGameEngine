#ifndef __HLGLVIEW_H__
#define __HLGLVIEW_H__

#include "HLMacros.h"
#include "hltime.h"

NS_HL_BEGIN

typedef LRESULT (*CUSTOM_WND_PROC)(UINT message, WPARAM wParam, LPARAM lParam, BOOL* pProcessed);

class HLGLView
{
public:
	void setViewName(const char* name)
	{
		strncpy(m_szViewName, name, 20);
	}
	void swapBuffers();

	void setMenuResource(LPCWSTR menu);
	void setWndProc(CUSTOM_WND_PROC proc);

	void setWinSize(int width, int height);

private:
	bool Create();
	bool initGL();
	void destroyGL();

public:
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	// win32 platform function
    HWND getHWnd();
   
    void centerWindow();

    typedef void (*LPFN_ACCELEROMETER_KEYHOOK)( UINT message,WPARAM wParam, LPARAM lParam );
    void setAccelerometerKeyHook( LPFN_ACCELEROMETER_KEYHOOK lpfnAccelerometerKeyHook );
    
    // static function
    /**
    @brief    get the shared main open gl window
    */
    static HLGLView* sharedOpenGLView();

private:
	HLGLView();

	bool m_bCaptured;
	HWND m_hWnd;
	HDC  m_hDC;
	HGLRC m_hRC;
	LPFN_ACCELEROMETER_KEYHOOK m_lpfnAccelerometerKeyHook;

	LPCWSTR m_menu;
	CUSTOM_WND_PROC m_wndproc;

	char m_szViewName[20];
};

NS_HL_END

#endif