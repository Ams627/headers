#pragma once
#undef max
#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <ams/errorutils.h>
#include <objbase.h>

namespace ams{

 class CompletionPort;
 
 struct ThreadParam
 {
     CompletionPort* iocp_;
     int threadIndex_;
     ThreadParam(CompletionPort* iocp, int threadIndex): iocp_(iocp), threadIndex_(threadIndex) {}
 };

class CompletionPort : public boost::noncopyable
{
public:
    static const int StartReading = -1;
    static const int StopReading = -2;
    DWORD milliseconds_;
private:
    typedef boost::function<void (BOOL, BYTE*, DWORD, int, DWORD, OVERLAPPED*) > ONPACKET;
    typedef boost::shared_ptr<ThreadParam> TParamPtr;
//    typedef boost::function<void (BOOL, BYTE*, OVERLAPPED*) > NEXTREAD;
    static const DWORD maxbufsize_ = 65536;
    static DWORD bufsize_;

    ONPACKET onpacket_;
//    NEXTREAD nextread_;
    std::vector<HANDLE> threads_;
    std::vector<TParamPtr> tparams_;
    HANDLE portHandle_;


public:
    CompletionPort(ONPACKET onpacket, int multiplier, int milliseconds):onpacket_(onpacket), milliseconds_(milliseconds)
    {
        int numberOfThreads = multiplier * GetNumberOfCPUs();
        portHandle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, numberOfThreads);

        for (int i = 0; i < numberOfThreads; i++)
        {
            TParamPtr p = TParamPtr(new ThreadParam(this, i));
            tparams_.push_back(p);
            DWORD tid;
            HANDLE hThread = CreateThread(NULL, NULL, Analyzer, tparams_[i].get(), CREATE_SUSPENDED, &tid);
            threads_.push_back(hThread);
        }

        for (size_t i = 0; i < threads_.size(); i++)
        {
            ResumeThread(threads_[i]);
        }
    }

    // Associate file HANDLE with an IOCP:
    void Associate(HANDLE h, ULONG_PTR k)
    {
        assert(k != -1);
        HANDLE newHandle = CreateIoCompletionPort(h, portHandle_, k, 0);
        assert(newHandle == portHandle_);
    }

    void Start(LPOVERLAPPED pol)
    {
        PostQueuedCompletionStatus(portHandle_, 0, StartReading, pol);
    }

    static DWORD WINAPI Analyzer(LPVOID pParam) // thread function
    {
        static int s_threadIndex = 0;
        int threadIndex = s_threadIndex++;
        bufsize_ = std::max(maxbufsize_, GetBytesPerSector());
        BYTE* buffer = reinterpret_cast<BYTE*>(VirtualAlloc(NULL, bufsize_, MEM_COMMIT, PAGE_READWRITE));


        ThreadParam*p = (ThreadParam*)(pParam);
        CompletionPort* _t = p->iocp_;
        
        bool stop = false;
        while (!stop)   
        {
            DWORD bytes;
            DWORD key;
            LPOVERLAPPED pol;
            BOOL res = GetQueuedCompletionStatus(_t->portHandle_, &bytes, &key, &pol, _t->milliseconds_);
            if (key == StopReading)
            {
                stop = true;
            }
            else if (res == 0 && pol == NULL)
            {
                _t->onpacket_(res, 0, 0, threadIndex, key, 0);
            }
            else
            {
                _t->onpacket_(res, buffer, bytes, threadIndex, key, pol);
            }
        }
        return 0;
    }

    static unsigned int GetBufSize()
    {
        return bufsize_;
    }

    static DWORD GetNumberOfCPUs()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return si.dwNumberOfProcessors;
    }

    static DWORD GetBytesPerSector()
    {
        static DWORD sectorsPerCluster;
        static DWORD bytesPerSector;
        static DWORD numberOfFreeClusters;
        static DWORD totalNumberOfClusters;
        static BOOL result =  GetDiskFreeSpace("c:\\",&sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters);
        RR result;
        return bytesPerSector;
    }
    operator HANDLE(){return portHandle_;}
};

__declspec(selectany) DWORD CompletionPort::bufsize_;
}