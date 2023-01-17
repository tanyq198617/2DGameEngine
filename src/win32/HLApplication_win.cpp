#include "HLApplication.h"
#include "HLGLView.h"
#include "HLAutoreleasePool.h"
#include "HLDirector2D.h"
#include "HLInputSystem.cpp"
#include "HLBMFont.cpp"

namespace hoolai
{
	void openURL(std::string url)
	{
		int  unicodeLen = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, NULL, 0);
		wchar_t *  pUnicode;
		pUnicode = new  wchar_t[unicodeLen + 1];
		memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
		MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, pUnicode, unicodeLen);
		ShellExecute(NULL, L"open", pUnicode, NULL, NULL, SW_SHOWNORMAL);
		delete[] pUnicode;
	}
}

#if defined(DEBUG)
void log_win32(const char* fmt, ...)
{
	int size = 100;
	std::string str;
	va_list ap;
	while (1)
	{
		str.resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char*)str.c_str(), size, fmt, ap);
		va_end(ap);
		if (n > -1 && n < size)
		{
			str.resize(n);
			break;
		}
		else
		size *= 2;
	}
	int bufflen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	WCHAR* widebuff = new WCHAR[bufflen+1];
	memset(widebuff, 0, sizeof(WCHAR)*(bufflen + 1));
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, widebuff, bufflen);

	OutputDebugStringA("Hoolai: ");
	OutputDebugStringW(widebuff);
	OutputDebugStringA("\n");

	bufflen = WideCharToMultiByte(CP_ACP, 0, widebuff, -1, NULL, 0, NULL, NULL);
	char* buff = new char[bufflen + 1];
	memset(buff, 0, sizeof(char)* (bufflen + 1));
	WideCharToMultiByte(CP_ACP, 0, widebuff, -1, buff, bufflen, NULL, NULL);
	printf("Hoolai: ");
	puts(buff);

	delete[] widebuff;
	delete[] buff;
}
#endif

static void PVRFrameEnableControlWindow(bool bEnable)
{
    HKEY hKey = 0;

    // Open PVRFrame control key, if not exist create it.
    if(ERROR_SUCCESS != RegCreateKeyExW(HKEY_CURRENT_USER,
                                        L"Software\\Imagination Technologies\\PVRVFRame\\STARTUP\\",
                                        0,
                                        0,
                                        REG_OPTION_NON_VOLATILE,
                                        KEY_ALL_ACCESS,
                                        0,
                                        &hKey,
                                        NULL))
    {
        return;
    }

    const WCHAR* wszValue = L"hide_gui";
    const WCHAR* wszNewData = (bEnable) ? L"NO" : L"YES";
    WCHAR wszOldData[256] = {0};
    DWORD   dwSize = sizeof(wszOldData);
    LSTATUS status = RegQueryValueExW(hKey, wszValue, 0, NULL, (LPBYTE)wszOldData, &dwSize);
    if (ERROR_FILE_NOT_FOUND == status              // the key not exist
            || (ERROR_SUCCESS == status                 // or the hide_gui value is exist
                && 0 != wcscmp(wszNewData, wszOldData)))    // but new data and old data not equal
    {
        dwSize = sizeof(WCHAR) * (wcslen(wszNewData) + 1);
        RegSetValueEx(hKey, wszValue, 0, REG_SZ, (const BYTE *)wszNewData, dwSize);
    }

    RegCloseKey(hKey);
}

NS_HL_BEGIN

static HLApplication* pSharedApplication = NULL;
LARGE_INTEGER       m_nAnimationInterval;

int HLApplication::run()
{
    HLAutoreleasePool autoreleasePool;

    PVRFrameEnableControlWindow(false);

    // Main message loop:
    MSG msg;
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nLast;
    LARGE_INTEGER nNow;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&nLast);

    _delegate->applicationDidFinishLaunching();

    HLGLView* pMainWnd = HLGLView::sharedOpenGLView();
    pMainWnd->centerWindow();
    ShowWindow(pMainWnd->getHWnd(), SW_SHOW);

    while (1)
    {
        if (! PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // Get current time tick.
            QueryPerformanceCounter(&nNow);

            // If it's the time to draw next frame, draw it, else sleep a while.
            if (nNow.QuadPart - nLast.QuadPart > m_nAnimationInterval.QuadPart)
            {
                nLast.QuadPart = nNow.QuadPart;
                HLDirector2D::getSingleton()->mainLoop();
            }
            else
            {
                Sleep(0);
            }
            continue;
        }

        if (WM_QUIT == msg.message)
        {
            // Quit message loop.
            break;
        }

        // Deal with windows message.
//if (! m_hAccelTable || ! TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
//{
        TranslateMessage(&msg);
        DispatchMessage(&msg);
//}

        switch(msg.message)
        {
        case WM_KEYDOWN:
        {
            if(__sysHLEditBox)
            {
                if(msg.wParam==0x08)
                {
                    __sysHLEditBox->deleteBackward();
                }
            }
        }
        break;
        case WM_KEYUP:
        break;
        case WM_SYSKEYUP:
            break;
        case WM_SYSKEYDOWN:
            break;
        case WM_HOTKEY:
            break;
        case WM_TIMER:
            break;
        case WM_PAINT:
            break;
        case WM_MOUSEMOVE:
            break;
        case WM_LBUTTONUP:
            break;
        case WM_RBUTTONDOWN:
            break;
        default:
            if(__sysHLEditBox)
            {
                if(msg.wParam < 0x80)
                {
					if(msg.wParam==0x0d)
					{
						std::string tmp;
						tmp.insert((std::string::size_type)0,1,0x0a);
                        __sysHLEditBox->insertText(tmp);
					}
                    else if(msg.wParam>=0x20)
                    {
                        std::string tmp;
                        tmp.insert((std::string::size_type)0,1,msg.wParam);
                        __sysHLEditBox->insertText(tmp);
                    }
                }
                else
                {
                    int i = 0;
                    unsigned char ch[8];
                    std::string tmp;
                    memset(ch,0,8);
                    UnicodetoUTF8(msg.wParam, ch);
                    while(ch[i] != 0)
                    {
                        tmp.insert((std::string::size_type)i,1,ch[i]);
                        i++;
                    }
                    __sysHLEditBox->insertText(tmp);
                }
            }
            break;
        }
    }

    return (int) msg.wParam;
}

void HLApplication::setAnimationInterval(double interval)
{
    LARGE_INTEGER nFreq;
    QueryPerformanceFrequency(&nFreq);
    m_nAnimationInterval.QuadPart = (LONGLONG)(interval * nFreq.QuadPart);
}

void HLApplication::swapBuffers()
{
    HLGLView::sharedOpenGLView()->swapBuffers();
}

HLLanguageType HLApplication::getCurrentLanguage()
{
	if (_currLanguage == kLanguageUnknown)
	{
		_currLanguage = kLanguageEnglish;
		if (_supportLanguages.size() > 0)
		{
			_currLanguage = _supportLanguages[0];
		}
	}
	return _currLanguage;
}

NS_HL_END
