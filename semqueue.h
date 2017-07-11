#pragma once

// the Remove method will throw EndThreadException if you set a the Stop Event
// you can use this to get out of a worker thread loop. A "blank" class (of
// the form class ETE {}; can be used for this purpose:
template <class T, class EndThreadException> class SemQueue
{
public:
    SemQueue(UINT queuesize)
    {
        // nameless semaphore with initial count zero and maximum count of queuesize:
        handles_[Semaphore] = CreateSemaphore(NULL, 0, queuesize, NULL);

        // the stop event - a thread waiting for a queued item must terminate on this
        // this nameless event is initially not signalled and manual reset
        handles_[StopEvent] = CreateEvent(NULL, TRUE, FALSE, NULL);
        InitializeCriticalSection(&lock_);
    }

    ~SemQueue()
    {
        CloseHandle(handles_[Semaphore]);
        CloseHandle(handles_[StopEvent]);
        DeleteCriticalSection(&lock_);
    }

    BOOL Add(const T& item)
    {
        BOOL result;
        EnterCriticalSection(&lock_);
        // increase semaphore count by 1:
        result = ReleaseSemaphore(handles_[Semaphore], 1, NULL);
        if (result)
        {
            queue_.push(item);
        }
        LeaveCriticalSection(&lock_);
        return result;
    }

    // move onto the front of the queue - no copying:
    BOOL Add(T&& item)
    {
        BOOL result;
        EnterCriticalSection(&lock_);
        // increase semaphore count by 1:
        result = ReleaseSemaphore(handles_[Semaphore], 1, NULL);
        if (result)
        {
            queue_.push(std::move(item));
        }
        LeaveCriticalSection(&lock_);
        return result;
    }

    T Remove()
    {
        DWORD waitResult = WaitForMultipleObjects(2, handles_, FALSE, INFINITE);
        if (waitResult == StopEvent)
        {
            // throw exception to get out of the thread loop:
            throw EndThreadException();
        }
        EnterCriticalSection(&lock_);
        // let's hope it has a move constructor:
        T result = std::move(queue_.front());
        queue_.pop();
        LeaveCriticalSection(&lock_);
        return result;
    }

    void Stop()
    {
        SetEvent(handles_[StopEvent]);
    } 


private:
    enum {StopEvent, Semaphore};
    HANDLE handles_[2];
    CRITICAL_SECTION lock_;
    std::queue<T> queue_;
};
