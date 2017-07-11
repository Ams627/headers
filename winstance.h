#pragma once
#include <algorithm>
#include <ams/errorutils.h> // for RR and reporter

namespace ams
{

#pragma data_seg("shared")
__declspec(selectany) HWND g_hwnd = 0;
#pragma data_seg()

#pragma comment(linker, "/SECTION:shared,RWS")


inline bool SingleWinstance()
{
    char modname[_MAX_PATH];
    DWORD len = GetModuleFileName(NULL, modname, _MAX_PATH);
    std::string mutexName(modname);
    mutexName += "{D7025902-D912-450a-8DD9-92D711E93A22}";
    std::replace(mutexName.begin(), mutexName.end(), '\\', '/');
    HANDLE h;
    RR h = CreateMutex(NULL, TRUE, mutexName.c_str());
    return (GetLastError() != ERROR_ALREADY_EXISTS);
}

inline void SendMaximizeMessage()
{
    ::SendMessage(g_hwnd, WM_USER + 1, 0, 0);
}

inline void SetWinstanceHWND(HWND wnd)
{
    g_hwnd = wnd;
}

}
