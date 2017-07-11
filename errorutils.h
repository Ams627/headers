#pragma once
#include <iostream>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

namespace ams{

class Exception
{

};



class XSymInitialize
{
    static BOOL Init()
    {
        return SymInitialize(GetCurrentProcess(), NULL, TRUE);
    }
    static BOOL init;
};

__declspec(selectany) BOOL XSymInitialize::init = XSymInitialize::Init();
    
#define HRESULTCHECK(func) ams::HRESULTERROR(func, __LINE__, __FILE__, __FUNCTION__) = 

struct HRESULTERROR
{
    static bool init_;
    void* p_;
    HRESULT h_;
    int line_;
    char* file_;
    char* function_;

    HRESULTERROR(void *p, int line, char* file, char* func): p_(p), line_(line), file_(file), function_(func) {}

    HRESULT operator=(HRESULT h)
    {
        h_ = h;
        if (h != S_OK)
        {
            throw *this;
        }
        return h;
    };

    template <class T> void DisplayError(T& ss)
    {
        ss << "Error at line " << line_ << " in file: " << file_ << " in function " << function_;
        std::string funcname;
        GetNameFromAddress32(funcname, (DWORD)p_, false);
        ss << " on call to function: " << funcname << " HRESULT returned was 0x" << std::hex << h_;
    }
};

class WinLastError {
    DWORD lastError_;
    int line_;
    char *file_;
public:
    WinLastError() : line_(0), file_(""), lastError_(GetLastError()) {}
    WinLastError(DWORD error) : line_(0), file_(""), lastError_(error) {}
    WinLastError(DWORD error, int line, char* file) : line_(line), file_(file), lastError_(error) {}
};

class WinException
{
    DWORD error_;
    int line_;
    char* filename_;
    char* funcname_;
public:

    WinException(DWORD error, int line, char* filename, char* funcname):
        error_(error), line_(line), filename_(filename), funcname_(funcname){}

    template <typename U> void DisplayError(U& str)
    {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
            );

        str << "Error at line " << line_ << " in file " << (filename_ ? filename_ : "") << "\n";
        str << "Message: " << (char*)lpMsgBuf;
        LocalFree(lpMsgBuf);
    }
};

inline std::string GetWinErrorAsString(DWORD error)
{
    LPVOID lpMsgBuf = 0;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_MAX_WIDTH_MASK |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
        );

    std::string result(static_cast<char*>(lpMsgBuf));
    result = std::to_string(error) + ": " + result;
    return result;
}


inline std::string GetSockErrorAsString()
{
    int error = WSAGetLastError();
	LPVOID lpMsgBuf = 0;
	FormatMessage(
	        FORMAT_MESSAGE_ALLOCATE_BUFFER |
	        FORMAT_MESSAGE_FROM_SYSTEM |
	        FORMAT_MESSAGE_IGNORE_INSERTS,
	        NULL,
	        error,
	        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	        (LPTSTR) &lpMsgBuf,
	        0,
	        NULL
	        );

	std::string result(static_cast<char*>(lpMsgBuf));
    result = std::to_string(error) + ":" + result;
    return result;
}

inline std::string GetSockErrorAsString(int error)
{
	LPVOID lpMsgBuf;
	FormatMessage(
	        FORMAT_MESSAGE_ALLOCATE_BUFFER |
	        FORMAT_MESSAGE_FROM_SYSTEM |
	        FORMAT_MESSAGE_IGNORE_INSERTS,
	        NULL,
	        error,
	        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	        (LPTSTR) &lpMsgBuf,
	        0,
	        NULL
	        );

	std::string result(static_cast<char*>(lpMsgBuf));
    return result;
}
} // namespace ams


class MainReporter
{
};

class Reporter2 : public MainReporter
{
    char* name_;
    char* function_;
    int line_;
    int error_;

    union 
    {
        int i;
        BOOL b;
        HANDLE h;
        HWND hwnd;
        long l;
        __int64 i64;
    } errorValue_;

public:
    template <class T> Reporter2(const T errorValue, int line = 0, char *file = NULL, char* function = NULL)
    {
        line_ = line;
        name_ = file;
        function_ = function;
        errorValue_.i = errorValue;
    }

    template <> Reporter2(HANDLE errorValue, int line, char *file, char* function)
    {
        line_ = line;
        name_ = file;
        function_ = function;
        errorValue_.h = errorValue;
    }

    template <> Reporter2(HWND errorValue, int line, char *file, char* function)
    {
        line_ = line;
        name_ = file;
        function_ = function;
        errorValue_.hwnd = errorValue;
    }

    void operator=(const HANDLE x)
    {
        if (x == errorValue_.h)
        {
            error_ = GetLastError();
            throw ams::WinException(error_, line_, name_, function_);
        }
    }
    void operator=(const BOOL x)
    {
        if (x == errorValue_.b)
        {
            error_ = GetLastError();
            throw ams::WinException(error_, line_, name_, function_);
        }
    }

    void operator=(const HWND x)
    {
        if (x == errorValue_.hwnd)
        {
            error_ = GetLastError();
            throw ams::WinException(error_, line_, name_, function_);
        }
    }

};


class Reporter : public ams::Exception
{
        static char *name_;
        static int line_;
        int error_;
        int value_;
public:
        template <typename T> Reporter(const T& x, int line = 0, char *file = NULL)
        {
                line_ = line;
                name_ = file;
                if (x == 0)
                {
                        error_ = GetLastError();
                        throw *this;
                }
        }

        template <typename T> void operator=(const T& x)
        {
                if (x == 0)
                {
                        error_ = GetLastError();
                        throw *this;
                }
        }
        template <typename T> void DisplayError(T& str)
        {
                LPVOID lpMsgBuf;
                FormatMessage(
                        FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        GetLastError(),
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                        (LPTSTR) &lpMsgBuf,
                        0,
                        NULL
                        );

                str << "Error at line " << line_ << " in file " << (name_ ? name_ : "") << "\n";
                str << "Message: " << (char*)lpMsgBuf;
                LocalFree(lpMsgBuf);
        }
};

__declspec(selectany) char* Reporter::name_;
__declspec(selectany) int Reporter::line_;


#define RR Reporter(1, __LINE__, __FILE__) =
#define CHECKNOT(x) Reporter2(x, __LINE__, __FILE__, __FUNCTION__) = 

// nothrow version:
#define RRNT(x) Reporter x(1, __LINE__, __FILE__);


#define PPD(x) std::cout << #x " is " << x << std::endl
#define PPX(x) std::cout << #x " is " << std::hex << x << std::endl

/* example:

int main(int argc, char* argv[])
try {
        RR CreateWindow("wonk", "crank", 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
catch (Reporter& r) {
        r.DisplayError();
}

*/

#define WINEXCEPTION(x) ams::WinException x(GetLastError(), __LINE__, __FILE__, __FUNCTION__);



#define WMTOSTRING(x) if(message_ == x) s = #x;
class WinMessagePrinter {
    UINT message_;
public:
    WinMessagePrinter(UINT message):message_(message) {}
    char* GetString()
    {
        char *s = "";
        WMTOSTRING(WM_NULL)
        WMTOSTRING(WM_CREATE)
        WMTOSTRING(WM_DESTROY)
        WMTOSTRING(WM_MOVE)
        WMTOSTRING(WM_SIZE)
        WMTOSTRING(WM_ACTIVATE)
        WMTOSTRING(WM_SETFOCUS)
        WMTOSTRING(WM_KILLFOCUS)
        WMTOSTRING(WM_ENABLE)
        WMTOSTRING(WM_SETREDRAW)
        WMTOSTRING(WM_SETTEXT)
        WMTOSTRING(WM_GETTEXT)
        WMTOSTRING(WM_GETTEXTLENGTH)
        WMTOSTRING(WM_PAINT)
        WMTOSTRING(WM_CLOSE)
        WMTOSTRING(WM_QUERYENDSESSION)
        WMTOSTRING(WM_QUIT)
        WMTOSTRING(WM_QUERYOPEN)
        WMTOSTRING(WM_ERASEBKGND)
        WMTOSTRING(WM_SYSCOLORCHANGE)
        WMTOSTRING(WM_ENDSESSION)
        //        WMTOSTRING(WM_SYSTEMERROR)
        WMTOSTRING(WM_SHOWWINDOW)
        //        WMTOSTRING(WM_CTLCOLOR)
        WMTOSTRING(WM_WININICHANGE)
        WMTOSTRING(WM_SETTINGCHANGE)
        WMTOSTRING(WM_DEVMODECHANGE)
        WMTOSTRING(WM_ACTIVATEAPP)
        WMTOSTRING(WM_FONTCHANGE)
        WMTOSTRING(WM_TIMECHANGE)
        WMTOSTRING(WM_CANCELMODE)
        WMTOSTRING(WM_SETCURSOR)
        WMTOSTRING(WM_MOUSEACTIVATE)
        WMTOSTRING(WM_CHILDACTIVATE)
        WMTOSTRING(WM_QUEUESYNC)
        WMTOSTRING(WM_GETMINMAXINFO)
        WMTOSTRING(WM_PAINTICON)
        WMTOSTRING(WM_ICONERASEBKGND)
        WMTOSTRING(WM_NEXTDLGCTL)
        WMTOSTRING(WM_SPOOLERSTATUS)
        WMTOSTRING(WM_DRAWITEM)
        WMTOSTRING(WM_MEASUREITEM)
        WMTOSTRING(WM_DELETEITEM)
        WMTOSTRING(WM_VKEYTOITEM)
        WMTOSTRING(WM_CHARTOITEM)
        WMTOSTRING(WM_SETFONT)
        WMTOSTRING(WM_GETFONT)
        WMTOSTRING(WM_SETHOTKEY)
        WMTOSTRING(WM_GETHOTKEY)
        WMTOSTRING(WM_QUERYDRAGICON)
        WMTOSTRING(WM_COMPAREITEM)
        WMTOSTRING(WM_COMPACTING)
        WMTOSTRING(WM_COMMNOTIFY)
        WMTOSTRING(WM_WINDOWPOSCHANGING)
        WMTOSTRING(WM_WINDOWPOSCHANGED)
        WMTOSTRING(WM_POWER)
        WMTOSTRING(WM_COPYDATA)
        WMTOSTRING(WM_CANCELJOURNAL)
        WMTOSTRING(WM_NOTIFY)
        WMTOSTRING(WM_INPUTLANGCHANGEREQUEST)
        WMTOSTRING(WM_INPUTLANGCHANGE)
        WMTOSTRING(WM_TCARD)
        WMTOSTRING(WM_HELP)
        WMTOSTRING(WM_USERCHANGED)
        WMTOSTRING(WM_NOTIFYFORMAT)
        WMTOSTRING(WM_CONTEXTMENU)
        WMTOSTRING(WM_STYLECHANGING)
        WMTOSTRING(WM_STYLECHANGED)
        WMTOSTRING(WM_DISPLAYCHANGE)
        WMTOSTRING(WM_GETICON)
        WMTOSTRING(WM_SETICON)
        WMTOSTRING(WM_NCCREATE)
        WMTOSTRING(WM_NCDESTROY)
        WMTOSTRING(WM_NCCALCSIZE)
        WMTOSTRING(WM_NCHITTEST)
        WMTOSTRING(WM_NCPAINT)
        WMTOSTRING(WM_NCACTIVATE)
        WMTOSTRING(WM_GETDLGCODE)
        WMTOSTRING(WM_NCMOUSEMOVE)
        WMTOSTRING(WM_NCLBUTTONDOWN)
        WMTOSTRING(WM_NCLBUTTONUP)
        WMTOSTRING(WM_NCLBUTTONDBLCLK)
        WMTOSTRING(WM_NCRBUTTONDOWN)
        WMTOSTRING(WM_NCRBUTTONUP)
        WMTOSTRING(WM_NCRBUTTONDBLCLK)
        WMTOSTRING(WM_NCMBUTTONDOWN)
        WMTOSTRING(WM_NCMBUTTONUP)
        WMTOSTRING(WM_NCMBUTTONDBLCLK)
        WMTOSTRING(WM_KEYFIRST)
        WMTOSTRING(WM_KEYDOWN)
        WMTOSTRING(WM_KEYUP)
        WMTOSTRING(WM_CHAR)
        WMTOSTRING(WM_DEADCHAR)
        WMTOSTRING(WM_SYSKEYDOWN)
        WMTOSTRING(WM_SYSKEYUP)
        WMTOSTRING(WM_SYSCHAR)
        WMTOSTRING(WM_SYSDEADCHAR)
        WMTOSTRING(WM_KEYLAST)
        WMTOSTRING(WM_IME_STARTCOMPOSITION)
        WMTOSTRING(WM_IME_ENDCOMPOSITION)
        WMTOSTRING(WM_IME_COMPOSITION)
        WMTOSTRING(WM_IME_KEYLAST)
        WMTOSTRING(WM_INITDIALOG)
        WMTOSTRING(WM_COMMAND)
        WMTOSTRING(WM_SYSCOMMAND)
        WMTOSTRING(WM_TIMER)
        WMTOSTRING(WM_HSCROLL)
        WMTOSTRING(WM_VSCROLL)
        WMTOSTRING(WM_INITMENU)
        WMTOSTRING(WM_INITMENUPOPUP)
        WMTOSTRING(WM_MENUSELECT)
        WMTOSTRING(WM_MENUCHAR)
        WMTOSTRING(WM_ENTERIDLE)
        WMTOSTRING(WM_CTLCOLORMSGBOX)
        WMTOSTRING(WM_CTLCOLOREDIT)
        WMTOSTRING(WM_CTLCOLORLISTBOX)
        WMTOSTRING(WM_CTLCOLORBTN)
        WMTOSTRING(WM_CTLCOLORDLG)
        WMTOSTRING(WM_CTLCOLORSCROLLBAR)
        WMTOSTRING(WM_CTLCOLORSTATIC)
        WMTOSTRING(WM_MOUSEFIRST)
        WMTOSTRING(WM_MOUSEMOVE)
        WMTOSTRING(WM_LBUTTONDOWN)
        WMTOSTRING(WM_LBUTTONUP)
        WMTOSTRING(WM_LBUTTONDBLCLK)
        WMTOSTRING(WM_RBUTTONDOWN)
        WMTOSTRING(WM_RBUTTONUP)
        WMTOSTRING(WM_RBUTTONDBLCLK)
        WMTOSTRING(WM_MBUTTONDOWN)
        WMTOSTRING(WM_MBUTTONUP)
        WMTOSTRING(WM_MBUTTONDBLCLK)
        WMTOSTRING(WM_MOUSELAST)
#if (_WIN32_WINNT > 0x500)
        WMTOSTRING(WM_MOUSEWHEEL)
#endif
        WMTOSTRING(WM_PARENTNOTIFY)
        WMTOSTRING(WM_ENTERMENULOOP)
        WMTOSTRING(WM_EXITMENULOOP)
        WMTOSTRING(WM_NEXTMENU)
        WMTOSTRING(WM_SIZING)
        WMTOSTRING(WM_CAPTURECHANGED)
        WMTOSTRING(WM_MOVING)
        WMTOSTRING(WM_POWERBROADCAST)
        WMTOSTRING(WM_DEVICECHANGE)
        WMTOSTRING(WM_MDICREATE)
        WMTOSTRING(WM_MDIDESTROY)
        WMTOSTRING(WM_MDIACTIVATE)
        WMTOSTRING(WM_MDIRESTORE)
        WMTOSTRING(WM_MDINEXT)
        WMTOSTRING(WM_MDIMAXIMIZE)
        WMTOSTRING(WM_MDITILE)
        WMTOSTRING(WM_MDICASCADE)
        WMTOSTRING(WM_MDIICONARRANGE)
        WMTOSTRING(WM_MDIGETACTIVE)
        WMTOSTRING(WM_MDISETMENU)
        WMTOSTRING(WM_ENTERSIZEMOVE)
        WMTOSTRING(WM_EXITSIZEMOVE)
        WMTOSTRING(WM_DROPFILES)
        WMTOSTRING(WM_MDIREFRESHMENU)
        WMTOSTRING(WM_IME_SETCONTEXT)
        WMTOSTRING(WM_IME_NOTIFY)
        WMTOSTRING(WM_IME_CONTROL)
        WMTOSTRING(WM_IME_COMPOSITIONFULL)
        WMTOSTRING(WM_IME_SELECT)
        WMTOSTRING(WM_IME_CHAR)
        WMTOSTRING(WM_IME_KEYDOWN)
        WMTOSTRING(WM_IME_KEYUP)
        WMTOSTRING(WM_MOUSEHOVER)
        WMTOSTRING(WM_MOUSELEAVE)
        WMTOSTRING(WM_CUT)
        WMTOSTRING(WM_COPY)
        WMTOSTRING(WM_PASTE)
        WMTOSTRING(WM_CLEAR)
        WMTOSTRING(WM_UNDO)
        WMTOSTRING(WM_RENDERFORMAT)
        WMTOSTRING(WM_RENDERALLFORMATS)
        WMTOSTRING(WM_DESTROYCLIPBOARD)
        WMTOSTRING(WM_DRAWCLIPBOARD)
        WMTOSTRING(WM_PAINTCLIPBOARD)
        WMTOSTRING(WM_VSCROLLCLIPBOARD)
        WMTOSTRING(WM_SIZECLIPBOARD)
        WMTOSTRING(WM_ASKCBFORMATNAME)
        WMTOSTRING(WM_CHANGECBCHAIN)
        WMTOSTRING(WM_HSCROLLCLIPBOARD)
        WMTOSTRING(WM_QUERYNEWPALETTE)
        WMTOSTRING(WM_PALETTEISCHANGING)
        WMTOSTRING(WM_PALETTECHANGED)
        WMTOSTRING(WM_HOTKEY)
        WMTOSTRING(WM_PRINT)
        WMTOSTRING(WM_PRINTCLIENT)
        WMTOSTRING(WM_HANDHELDFIRST)
        WMTOSTRING(WM_HANDHELDLAST)
        WMTOSTRING(WM_PENWINFIRST)
        WMTOSTRING(WM_PENWINLAST)
        //        WMTOSTRING(WM_COALESCE_FIRST)
        //        WMTOSTRING(WM_COALESCE_LAST)
//        WMTOSTRING(WM_DDE_FIRST)
//        WMTOSTRING(WM_DDE_INITIATE)
//        WMTOSTRING(WM_DDE_TERMINATE)
//        WMTOSTRING(WM_DDE_ADVISE)
//        WMTOSTRING(WM_DDE_UNADVISE)
//        WMTOSTRING(WM_DDE_ACK)
//        WMTOSTRING(WM_DDE_DATA)
//        WMTOSTRING(WM_DDE_REQUEST)
//        WMTOSTRING(WM_DDE_POKE)
//        WMTOSTRING(WM_DDE_EXECUTE)
//        WMTOSTRING(WM_DDE_LAST)
//        WMTOSTRING(WM_USER)
//        WMTOSTRING(WM_APP);
          return s;
    }
    friend std::ostream& operator<<(std::ostream&, const WinMessagePrinter);
};

inline std::ostream& operator<<(std::ostream &str, WinMessagePrinter msg)
{
    str << msg.GetString();
    return str;
}
