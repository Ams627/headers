#include <iostream>
#include <iomanip>
#include <string>
#include <DbgHelp.h>
#include <eh.h>
#include <ams/errorutils.h>

#pragma comment(lib, "dbghelp.lib")

namespace {
    void GetStatusCode(std::string& s, int code)
    {
        s.clear();
#define XX(c) if (code == c) {s = #c;}
        XX(STATUS_WAIT_0)
        XX(STATUS_ABANDONED_WAIT_0)
        XX(STATUS_USER_APC)
        XX(STATUS_TIMEOUT)
        XX(STATUS_PENDING)
        XX(DBG_EXCEPTION_HANDLED)
        XX(DBG_CONTINUE)
        XX(STATUS_SEGMENT_NOTIFICATION)
        XX(DBG_TERMINATE_THREAD)
        XX(DBG_TERMINATE_PROCESS)
        XX(DBG_CONTROL_C)
        XX(DBG_CONTROL_BREAK)
        XX(DBG_COMMAND_EXCEPTION)
        XX(STATUS_GUARD_PAGE_VIOLATION)
        XX(STATUS_DATATYPE_MISALIGNMENT)
        XX(STATUS_BREAKPOINT)
        XX(STATUS_SINGLE_STEP)
        XX(DBG_EXCEPTION_NOT_HANDLED)
        XX(STATUS_ACCESS_VIOLATION)
        XX(STATUS_IN_PAGE_ERROR)
        XX(STATUS_INVALID_HANDLE)
        XX(STATUS_NO_MEMORY)
        XX(STATUS_ILLEGAL_INSTRUCTION)
        XX(STATUS_NONCONTINUABLE_EXCEPTION)
        XX(STATUS_INVALID_DISPOSITION)
        XX(STATUS_ARRAY_BOUNDS_EXCEEDED)
        XX(STATUS_FLOAT_DENORMAL_OPERAND)
        XX(STATUS_FLOAT_DIVIDE_BY_ZERO)
        XX(STATUS_FLOAT_INEXACT_RESULT)
        XX(STATUS_FLOAT_INVALID_OPERATION)
        XX(STATUS_FLOAT_OVERFLOW)
        XX(STATUS_FLOAT_STACK_CHECK)
        XX(STATUS_FLOAT_UNDERFLOW)
        XX(STATUS_INTEGER_DIVIDE_BY_ZERO)
        XX(STATUS_INTEGER_OVERFLOW)
        XX(STATUS_PRIVILEGED_INSTRUCTION)
        XX(STATUS_STACK_OVERFLOW)
        XX(STATUS_CONTROL_C_EXIT)
        XX(STATUS_FLOAT_MULTIPLE_FAULTS)
        XX(STATUS_FLOAT_MULTIPLE_TRAPS)
        XX(STATUS_REG_NAT_CONSUMPTION)
    }
}

namespace ams {
class SEException
{
private:
    DWORD n_;
    EXCEPTION_POINTERS* p_;
public:
    SEException(DWORD n, EXCEPTION_POINTERS* p) : n_(n), p_(p) {}
    ~SEException() {}
    unsigned int getSeNumber() {return n_;}
    template <class T> void Report(T& str)
    {
        using namespace std;
        str << "Exception " << setw(8) << setfill('0') << uppercase << hex << 
            p_->ExceptionRecord->ExceptionCode << "\n";


        DWORD64 address = (DWORD64)p_->ExceptionRecord->ExceptionAddress;
        str << "address:" << setw(8) << setfill('0') << uppercase << hex << p_->ExceptionRecord->ExceptionAddress << "\n";

        DWORD disp = 0;
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof IMAGEHLP_LINE64;

        RR SymGetLineFromAddr64(GetCurrentProcess(), address, &disp, &line);
        DWORD error = GetLastError();

        string smessage;
        GetStatusCode(smessage, n_);
        str << smessage << "\n" << line.FileName << " line " << dec << line.LineNumber << endl;
    }
    static void TransFunc(unsigned int u, EXCEPTION_POINTERS* pExp)
    {
        throw SEException(u, pExp);
    }
};

class WincppException
{
public:
    WincppException(){Init();}

    template <typename T> WincppException(T x){
        Init(); __try {x();} __finally {}
    }
    template <typename T, typename P1> WincppException(T x, P1 p1){
        Init(); __try {x(p1);} __finally {}
    }
    template <typename T, typename P1, typename P2> WincppException(T x, P1 p1, P2 p2){
        Init(); __try {x(p1, p2);} __finally {}
    }
    template <typename T, typename P1, typename P2, typename P3> WincppException(T x, P1 p1, P2 p2, P3 p3){
        Init(); __try {x(p1, p2, p3);} __finally {}
    }
    template <typename T, typename P1, typename P2, typename P3, typename P4> WincppException(T x, P1 p1, P2 p2, P3 p3, P4 p4){
        Init(); __try {x(p1, p2, p3, p4);} __finally {}
    }
    template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5> WincppException(T x, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5){
        Init(); __try {x(p1, p2, p3, p4, p5);} __finally {}
    }

private:
    inline void Init()
    {
        SymInitialize(GetCurrentProcess(), NULL, TRUE);
        SymSetOptions(SYMOPT_LOAD_LINES);
        _set_se_translator(TransFunc);
    }

    static void TransFunc(unsigned int u, EXCEPTION_POINTERS* pExp)
    {
        throw SEException(u, pExp);
    }
};

}//namespace ams