#pragma once
#include <windows.h>
#define _WIN32_DCOM 
#include <comdef.h>
#include <objbase.h>
#include <shlobj.h>
#include <tchar.h>

namespace Ams {

inline int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

inline LPITEMIDLIST BrowseForFolder(HWND hwnd, const TCHAR* startFolder, LPTSTR path, int nCSIDL = NULL)
{
    LPITEMIDLIST pidlRoot = NULL;
    LPITEMIDLIST pidlSelected = NULL;
    BROWSEINFO bi = {0};
    LPMALLOC pMalloc = NULL;

    SHGetMalloc(&pMalloc);

    if(nCSIDL)
    {
        SHGetFolderLocation(hwnd, nCSIDL, NULL, NULL, &pidlRoot);
    }

    else
    {
        pidlRoot = NULL;
    }

    bi.hwndOwner = hwnd;
    bi.pidlRoot = pidlRoot;
    TCHAR buf[_MAX_PATH];
    bi.pszDisplayName = buf;
    bi.lpszTitle = "Choose a folder";
    bi.ulFlags = BIF_USENEWUI|BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = reinterpret_cast<LPARAM>(startFolder);

    pidlSelected = SHBrowseForFolder(&bi);

    if(pidlRoot)
    {
        pMalloc->Free(pidlRoot);
    }
    pMalloc->Release();

    if (pidlSelected)
    {
        SHGetPathFromIDList(pidlSelected, path);
    }

    return pidlSelected;
}


inline int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    TCHAR* folder = reinterpret_cast<TCHAR*>(lpData);
    if (uMsg == BFFM_INITIALIZED)
    {
        ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, reinterpret_cast<LPARAM>(folder));
        ::SendMessage(hwnd, BFFM_SETEXPANDED, TRUE, reinterpret_cast<LPARAM>(folder));
    }
    return 0;
}

}