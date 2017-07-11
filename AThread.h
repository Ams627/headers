#pragma once
#include <process.h>

typedef unsigned (__stdcall *THREADFUNC)(void *);

// Athread class - simple thread wrapper. We use _beginthreadex because it is safe to use with the C library
class AThread
{
    HANDLE hThread_;
    DWORD threadid_;
public:
    // type for a thread function:

    AThread(THREADFUNC startaddress, unsigned int stacksize, void *arg, bool suspend)
    {
        unsigned initflag = suspend ? CREATE_SUSPENDED : 0;
        hThread_ = (HANDLE) _beginthreadex(0, stacksize, startaddress, arg, initflag, reinterpret_cast<unsigned int *>(&threadid_));
    }

	AThread(AThread&& source)
	{
		hThread_ = source.hThread_;
		threadid_ = source.threadid_;
	}

    virtual ~AThread()
    {
    }

	operator HANDLE()
	{
		return hThread_;
	}

    DWORD GetThreadId()
    {
        return threadid_;
    }

    void Resume()
    {
        ResumeThread(hThread_);
    }
};