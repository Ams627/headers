#pragma once

class UniqueProcError
{
};

class UniqueProc
{
    bool alreadyRunning_;
public:
    enum Namespc
    {
        UNIQUEPROC_GLOBAL,
        UNIQUEPROC_LOCAL
    };

    UniqueProc(Namespc ns)
    {
        std::string name = (ns == UNIQUEPROC_GLOBAL) ? "Global\\" : "";
        name += "{6C38C288-F010-424C-913B-207ED0B14611}";

        HANDLE h = CreateMutexA(NULL, TRUE, name.c_str());
        if (h == NULL)
        {
            throw UniqueProcError();
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            alreadyRunning_ = true;
        }
        else
        {
            alreadyRunning_ = false;
        }
    }

    bool IsAlreadyRunning() const
    {
        return alreadyRunning_;
    }
};