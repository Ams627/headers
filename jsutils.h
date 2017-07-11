#pragma once

// this file contains Javascript and JSON related functions:

namespace ams {
// return the number of milliseconds since the JS Epoch - midnight 1970-Jan-01
inline uint64_t GetJSDateMilliseconds()
{
    // hundred nanosec intervals from the windows epoch to the javascript one:
    static const uint64_t msecsW2J = 116444736000000000LL;
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    uint64_t result = ((1LL * ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
    result -= msecsW2J;
    return result;
}

}

