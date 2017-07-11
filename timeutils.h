#pragma once
#include <windows.h>

typedef unsigned __int64 QWORD;

inline QWORD GetNanosecondTime()
{
    FILETIME filetime;
    GetSystemTimeAsFileTime(&filetime);
    QWORD result = filetime.dwHighDateTime;
    result <<= 32;
    result += filetime.dwLowDateTime;
    return result;
}
