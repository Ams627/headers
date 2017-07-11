#include <sstream>
#include <tchar.h>

#define AOUTPUT(hwnd) {std::stringstream ss; ss << #hwnd " is:" << hwnd << "\n"; OutputDebugString(ss.str().c_str());}
#define WOUTPUT(hwnd) {std::wstringstream ss; ss << L#hwnd L" is:" << hwnd; OutputDebugString(ss.str().c_str());OutputDebugString(L"\n");}
#define WOUTPUT2(hwnd, ostring) {std::wstringstream ss; ss << L#hwnd L" is:" << hwnd; ostring = ss.str(); OutputDebugString(ss.str().c_str()); OutputDebugString(L"\n");}
#define AOUTPUTH(x) {std::stringstream ss; ss << #x " is:" << std::uppercase << std::hex << x << "\n"; OutputDebugString(ss.str().c_str());}
#define WOUTPUTH(x) {std::wstringstream ss; ss << L#x L" is:" << std::uppercase << std::hex << x; OutputDebugString(ss.str().c_str());OutputDebugString(L"\n");}
#define PP(x) {std::cout << #x " is: " << x << "\n";}
#define PPW(x) {std::wcout << #x " is: " << x << L"\n";}

#define AOUTPUTTHREADID {DWORD threadid = GetCurrentThreadId(); OUTPUTH(threadid);}
#define WOUTPUTTHREADID {DWORD threadid = GetCurrentThreadId(); WOUTPUTH(threadid);}

#ifdef UNICODE
#define OUTPUT WOUTPUT
#define OUTPUTTHREADID WOUTPUTTHREADID
#define OUTPUTH WOUTPUTH
#define FUNC WOUTPUT(__FUNCTIONW__)
#else
#define OUTPUT AOUTPUT
#define OUTPUTTHREADID AOUTPUTTHREADID
#define OUTPUTH AOUTPUTH
#define FUNC OUTPUT(__FUNCTION__)
#endif



#define OUTPUT_NM(x) {std::stringstream ss; ss << #x " is:";\
    if (x == HDN_ENDTRACK) ss << "HDN_ENDTRACK";\
    if (x == HDN_ITEMCHANGINGA) ss << "HDN_ITEMCHANGINGA";\
    if (x == HDN_ITEMCHANGINGW) ss << "HDN_ITEMCHANGINGW";\
    if (x == HDN_ITEMCHANGEDA) ss << "HDN_ITEMCHANGEDA";\
    if (x == HDN_ITEMCHANGEDW) ss << "HDN_ITEMCHANGEDW";\
    if (x == HDN_ITEMCLICKA) ss << "HDN_ITEMCLICKA";\
    if (x == HDN_ITEMCLICKW) ss << "HDN_ITEMCLICKW";\
    if (x == HDN_ITEMDBLCLICKA) ss << "HDN_ITEMDBLCLICKA";\
    if (x == HDN_ITEMDBLCLICKW) ss << "HDN_ITEMDBLCLICKW";\
    if (x == HDN_DIVIDERDBLCLICKA) ss << "HDN_DIVIDERDBLCLICKA";\
    if (x == HDN_DIVIDERDBLCLICKW) ss << "HDN_DIVIDERDBLCLICKW";\
    if (x == HDN_BEGINTRACKA) ss << "HDN_BEGINTRACKA";\
    if (x == HDN_BEGINTRACKW) ss << "HDN_BEGINTRACKW";\
    if (x == HDN_ENDTRACKA) ss << "HDN_ENDTRACKA";\
    if (x == HDN_ENDTRACKW) ss << "HDN_ENDTRACKW";\
    if (x == HDN_TRACKA) ss << "HDN_TRACKA";\
    if (x == HDN_TRACKW) ss << "HDN_TRACKW";\
    if (x == HDN_GETDISPINFOA) ss << "HDN_GETDISPINFOA";\
    if (x == HDN_GETDISPINFOW) ss << "HDN_GETDISPINFOW";\
    if (x == HDN_BEGINDRAG) ss << "HDN_BEGINDRAG";\
    if (x == HDN_ENDDRAG) ss << "HDN_ENDDRAG";\
    if (x == HDN_FILTERCHANGE) ss << "HDN_FILTERCHANGE";\
    if (x == HDN_FILTERBTNCLICK) ss << "HDN_FILTERBTNCLICK";\
    if (x == HDN_ITEMCHANGING) ss << "HDN_ITEMCHANGING";\
    if (x == HDN_ITEMCHANGED) ss << "HDN_ITEMCHANGED";\
    if (x == HDN_ITEMCLICK) ss << "HDN_ITEMCLICK";\
    if (x == HDN_ITEMDBLCLICK) ss << "HDN_ITEMDBLCLICK";\
    if (x == HDN_DIVIDERDBLCLICK) ss << "HDN_DIVIDERDBLCLICK";\
    if (x == HDN_BEGINTRACK) ss << "HDN_BEGINTRACK";\
    if (x == HDN_ENDTRACK) ss << "HDN_ENDTRACK";\
    if (x == HDN_TRACK) ss << "HDN_TRACK";\
    if (x == HDN_GETDISPINFO) ss << "HDN_GETDISPINFO";\
    if (x == HDN_ITEMCHANGING) ss << "HDN_ITEMCHANGING";\
    if (x == HDN_ITEMCHANGED) ss << "HDN_ITEMCHANGED";\
    if (x == HDN_ITEMCLICK) ss << "HDN_ITEMCLICK";\
    if (x == HDN_ITEMDBLCLICK) ss << "HDN_ITEMDBLCLICK";\
    if (x == HDN_DIVIDERDBLCLICK) ss << "HDN_DIVIDERDBLCLICK";\
    if (x == HDN_BEGINTRACK) ss << "HDN_BEGINTRACK";\
    if (x == HDN_ENDTRACK) ss << "HDN_ENDTRACK";\
    if (x == HDN_TRACK) ss << "HDN_TRACK";\
    if (x == HDN_GETDISPINFO) ss << "HDN_GETDISPINFO";\
    ss << std::endl; OutputDebugString(ss.str().c_str());}

#define OUTPUTRECT(x) {std::stringstream ss; ss << "rectangle " #x " is:" << "left: " <<\
                       x.left << " top: " << x.top << " width: " << x.Width() << " height: " <<\
                       x.Height() << std::endl; OutputDebugString(ss.str().c_str()); }

