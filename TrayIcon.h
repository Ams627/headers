#pragma once
#include <tchar.h>
#include <ShellAPI.h>
#include <ams/errorutils.h>

#pragma warning(disable: 4312 4244)

class TrayIcon
{
    static bool wndclassRegistered;
    static UINT id_;
    UINT icon_; // resource ID of icon
    char* tiptext_; // tooltip for tray icon
    HWND wnd_;
    UINT message_;
    HMENU menu_;
    NOTIFYICONDATA nid_;
public:

    TrayIcon(HWND wnd, UINT message, UINT icon, UINT menu=0, char* tipText=NULL):
             wnd_(wnd), message_(message), 
             icon_(icon), tiptext_(tipText)
    {
        if (menu != 0)
        {
            menu_ = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(menu));
        }
        else
        {
            menu_ = 0;
        }
        memset(&nid_, 0, sizeof(nid_));
        nid_.cbSize = sizeof(nid_);
        nid_.uVersion = NOTIFYICON_VERSION;
        BOOL res = Shell_NotifyIcon(NIM_SETVERSION, &nid_);

        nid_.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(icon));
        nid_.uCallbackMessage = message_;
        nid_.uFlags = NIF_MESSAGE|NIF_ICON;
        nid_.hWnd = RegisterClassCreateWindow();
        nid_.uTimeout = 1000;
        if (tipText)
        {
            nid_.uFlags |= NIF_TIP;
            strcpy(nid_.szTip, tipText);
        }
        RR Shell_NotifyIcon(NIM_ADD, &nid_);
    }

    void SetWindow(HWND wnd)
    {
        wnd_ = wnd;
    }
    static LRESULT CALLBACK TrayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        static UINT msgnum = 0;
        TrayIcon* pTrayInstance = reinterpret_cast<TrayIcon*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (pTrayInstance && msg == pTrayInstance->message_)
        {
            UINT nmessage = static_cast<UINT>(lParam);
            if (nmessage == WM_RBUTTONDOWN)
            {
                RECT r;
                HMENU popupMenu = GetSubMenu(pTrayInstance->menu_, 0);
                POINT pt;
                GetCursorPos(&pt);
                TrackPopupMenu(popupMenu, TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x, pt.y, 0, pTrayInstance->wnd_,  &r);
            }
            ::PostMessage(pTrayInstance->wnd_, pTrayInstance->message_, wParam, lParam);
            return 0;
        }
        else
        {
            if (msg == WM_CREATE)
            {
                msg = msg;
            }
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }


    virtual ~TrayIcon(void)
    {
        Shell_NotifyIcon(NIM_DELETE, &nid_);
    }

    void Animate(HWND wnd)
    {
        RECT rectfrom;
        GetWindowRect(wnd, &rectfrom);
        RECT rectto;
        GetTrayWndRect(&rectto);
        DrawAnimatedRects(wnd, IDANI_CAPTION, &rectfrom, &rectto);
    }

    void ShowBalloon(char* text, char* title, UINT timeout, DWORD flags = 0)
    {
        nid_.cbSize=sizeof(NOTIFYICONDATA);
        nid_.uFlags = NIF_INFO;
        nid_.uTimeout = timeout;
        nid_.dwInfoFlags = flags;
        strcpy(nid_.szInfo, text ? text : _T(""));
        strcpy(nid_.szInfoTitle, title ? title : _T(""));
        Shell_NotifyIcon(NIM_MODIFY, &nid_);
    }

private:
    static BOOL CALLBACK FindTrayWnd(HWND hwnd, LPARAM lParam)
    {
        // lParam will contain a pointer to a CRect structure that will be used to 
        // store the coordinates of the tray

        TCHAR szClassName[256];
        GetClassName(hwnd, szClassName, 255);

        // Did we find the Main System Tray? If so, then get its size and keep going
        if (_tcscmp(szClassName, _T("TrayNotifyWnd")) == 0)
        {
            RECT *pRect = (RECT*) lParam;
            ::GetWindowRect(hwnd, pRect);
            return TRUE;
        }

        // Did we find the System Clock? If so, then adjust the size of 
        // the rectangle we have and quit (clock will be found after the 
        // system tray)
        if (_tcscmp(szClassName, _T("TrayClockWClass")) == 0)
        {
            RECT *lpRect = (RECT*) lParam;
            RECT rectClock;
            ::GetWindowRect(hwnd, &rectClock);

            // The system clock may be either to the right or above the system 
            // tray area. Adjust accordingly
            if (rectClock.bottom < lpRect->bottom-5)  // 5 = fudge factor
                lpRect->top = rectClock.bottom;
            else
                lpRect->right = rectClock.left;
            return FALSE;
        }

        return TRUE;
    }


    void GetTrayWndRect(LPRECT lprect)
    {
        const int DEFAULT_RECT_WIDTH = 150;
        const int DEFAULT_RECT_HEIGHT = 30;

        HWND hShellTrayWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
        if (hShellTrayWnd)
        {
            ::GetWindowRect(hShellTrayWnd, lprect);
            EnumChildWindows(hShellTrayWnd, FindTrayWnd, (LPARAM)lprect);
            return;
        }
        // OK, we failed to get the rect from the quick hack. Either explorer isn't
        // running or it's a new version of the shell with the window class names
        // changed (how dare Microsoft change these undocumented class names!) So, we
        // try to find out what side of the screen the taskbar is connected to. We
        // know that the system tray is either on the right or the bottom of the
        // taskbar, so we can make a good guess at where to minimize to
        APPBARDATA appBarData;
        appBarData.cbSize=sizeof(appBarData);
        if (SHAppBarMessage(ABM_GETTASKBARPOS,&appBarData))
        {
            // We know the edge the taskbar is connected to, so guess the rect of the
            // system tray. Use various fudge factor to make it look good
            switch(appBarData.uEdge)
            {
            case ABE_LEFT:
            case ABE_RIGHT:
                // We want to minimize to the bottom of the taskbar
                lprect->top    = appBarData.rc.bottom-100;
                lprect->bottom = appBarData.rc.bottom-16;
                lprect->left   = appBarData.rc.left;
                lprect->right  = appBarData.rc.right;
                break;

            case ABE_TOP:
            case ABE_BOTTOM:
                // We want to minimize to the right of the taskbar
                lprect->top    = appBarData.rc.top;
                lprect->bottom = appBarData.rc.bottom;
                lprect->left   = appBarData.rc.right-100;
                lprect->right  = appBarData.rc.right-16;
                break;
            }
            return;
        }

        // Blimey, we really aren't in luck. It's possible that a third party shell
        // is running instead of explorer. This shell might provide support for the
        // system tray, by providing a Shell_TrayWnd window (which receives the
        // messages for the icons) So, look for a Shell_TrayWnd window and work out
        // the rect from that. Remember that explorer's taskbar is the Shell_TrayWnd,
        // and stretches either the width or the height of the screen. We can't rely
        // on the 3rd party shell's Shell_TrayWnd doing the same, in fact, we can't
        // rely on it being any size. The best we can do is just blindly use the
        // window rect, perhaps limiting the width and height to, say 150 square.
        // Note that if the 3rd party shell supports the same configuraion as
        // explorer (the icons hosted in NotifyTrayWnd, which is a child window of
        // Shell_TrayWnd), we would already have caught it above
        if (hShellTrayWnd)
        {
            ::GetWindowRect(hShellTrayWnd, lprect);
            if (lprect->right - lprect->left > DEFAULT_RECT_WIDTH)
                lprect->left = lprect->right - DEFAULT_RECT_WIDTH;
            if (lprect->bottom - lprect->top > DEFAULT_RECT_HEIGHT)
                lprect->top = lprect->bottom - DEFAULT_RECT_HEIGHT;

            return;
        }

        // OK. Haven't found a thing. Provide a default rect based on the current work
        // area
        SystemParametersInfo(SPI_GETWORKAREA,0,lprect, 0);
        lprect->left = lprect->right - DEFAULT_RECT_WIDTH;
        lprect->top  = lprect->bottom - DEFAULT_RECT_HEIGHT;
    }


    HWND RegisterClassCreateWindow()
    {
        if (!wndclassRegistered)
        {
            wndclassRegistered = true;
            ATOM a;
            WNDCLASS wc = {0};
            wc.lpfnWndProc        = TrayProc;
            wc.lpszClassName      = "TrayIconWindowClass";
            RR a = RegisterClass(&wc);
        }
        HWND wnd;
        RR wnd = CreateWindow("TrayIconWindowClass", "", 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
        SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        return wnd;
    }

};

__declspec(selectany) bool TrayIcon::wndclassRegistered = false;
