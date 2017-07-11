#pragma once
namespace ams
{
    inline uint64_t GetCurrentFiletime()
    {
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        return ((1LL * ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
    }

    inline int64_t LiDiff(LARGE_INTEGER li1, LARGE_INTEGER li2)
    {
        return li1.QuadPart - li2.QuadPart;
    }

}