#pragma once

class AIOCPException : public std::exception
{
public:
    AIOCPException(std::string msg) : std::exception(msg.c_str()) {}
};

class AIOCP
{
    HANDLE hPortHandle_;
    std::vector<HANDLE> assocHandles_;
public:
    AIOCP()
    {
        // Create an IO completion port with number of threads allowed to run = number of CPUs
        hPortHandle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
        if (hPortHandle_ == NULL)
        {
            DWORD error = GetLastError();
            std::ostringstream oss;
            oss << "CreateIoCompletionPort failed: GetLastError returned " << error;
            throw AIOCPException(oss.str());
        }
    }

    // Associate a system object (e.g. file or socket) with the completion port:
    void Associate(HANDLE hAssoc, ULONG_PTR completionKey)
    {
        HANDLE h = CreateIoCompletionPort(hAssoc, hPortHandle_, completionKey, 0);
        if (h != hPortHandle_)
        {
            DWORD error = GetLastError();
            std::ostringstream oss;
            oss << "CreateIoCompletionPort failed: GetLastError returned " << error;
            throw AIOCPException(oss.str());
        }
        assocHandles_.push_back(hAssoc);
    }

    template <class T> void Associate(int keystart, T start, T end)
    {
        for (auto p = start; p != end; ++p)
        {
            HANDLE h = CreateIoCompletionPort(*p, hPortHandle_, keystart++, 0);
            if (h != hPortHandle_)
            {
                DWORD error = GetLastError();
                std::ostringstream oss;
                oss << "CreateIoCompletionPort failed: GetLastError returned " << error;
                throw AIOCPException(oss.str());
            }
            assocHandles_.push_back(*p);
        }
    }


    operator HANDLE() {return hPortHandle_;}

    virtual ~AIOCP()
    {
        CloseHandle(hPortHandle_);
    }
};