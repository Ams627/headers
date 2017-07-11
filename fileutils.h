#pragma once
#include <ams/errorutils.h>
namespace ams
{
struct LineReaderException : public std::exception
{
	LineReaderException(std::string msg) : std::exception(msg.c_str()) {}
	LineReaderException(char* msg) : std::exception(msg) {}
	virtual ~LineReaderException() {};
};

class LineReader
{
	std::string filename_;
	std::function<void(std::string)> linefunc_;
public:
	LineReader(std::string filename, std::function<void(std::string)> func) : filename_(filename), linefunc_(func)
	{

	}

	void Read()
	{
		std::ifstream ifs(filename_);
		if (!ifs)
		{
			throw LineReaderException("cannot open file " + filename_);
		}
		std::string line;
		while (std::getline(ifs, line))
		{
			linefunc_(line);
		}
	}
};

#ifdef _WIN64
// reads entire file into memory before sending each line to the specified function
class FastLineReader
{
    char* buffer_;
    uint64_t bufferSize_;
    std::string filename_;
    std::function<void(std::string)> func_;
    HANDLE hEvent_;
public:
    FastLineReader(std::string filename, std::function<void(std::string)> func, HANDLE hEvent = nullptr) : func_(func), filename_(filename), hEvent_(hEvent)
    {
        bufferSize_ = GetFileSize(filename);
        if ((bufferSize_ & 0xFFFFFFFF00000000) != 0)
        {
            throw QException(filename + ": file too big");
        }
        if (bufferSize_ == 0)
        {
            throw QException(filename + ": zero length file");
        }
        bufferSize_ = TwoPower(bufferSize_);
        HANDLE heap = GetProcessHeap();
        buffer_ = reinterpret_cast<char*>(HeapAlloc(heap, HEAP_NO_SERIALIZE, bufferSize_));
    }

    FastLineReader(FastLineReader&& source)
    {
        buffer_ = source.buffer_;
        bufferSize_ = source.bufferSize_;
        filename_ = source.filename_;
        func_ = source.func_;
        hEvent_ = source.hEvent_;

        // mark memory as non-allocated - we don't want to free it twice:
        source.buffer_ = nullptr;
    }

    static uint64_t TwoPower(uint64_t v)
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        return v;
    }

    static uint64_t GetFileSize(std::string filename)
    {
        WIN32_FILE_ATTRIBUTE_DATA fileInfo;

        BOOL res = GetFileAttributesEx(filename.c_str(), GetFileExInfoStandard, (void*)&fileInfo);
        return (1LL * fileInfo.nFileSizeHigh << 32) + fileInfo.nFileSizeLow;
    }

    void Read()
    {
        HANDLE h = CreateFile(filename_.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (h == INVALID_HANDLE_VALUE)
        {
            DWORD error = GetLastError();
            std::string errorStr = GetWinErrorAsString(error);
            throw QException(std::string("") + "Cannot open file: INVALID_HANDLE_VALUE returned for " + filename_ + " error was " + errorStr);
        }

        DWORD bytesActuallyRead;
        BOOL result = ReadFile(h, buffer_, static_cast<DWORD>(bufferSize_), &bytesActuallyRead, 0);
        if (!result)
        {
            DWORD error = GetLastError();
            std::string serror = "ReadFileEx failed: error is " + ams::GetWinErrorAsString(error);
            throw QException(serror);
        }
        size_t nearIndex = 0;
        size_t farIndex = 0;

        while (farIndex < bytesActuallyRead)
        {
            while (farIndex < bytesActuallyRead && (buffer_[farIndex] == 13 || buffer_[farIndex] == 10))
            {
                farIndex++;
            }
            nearIndex = farIndex;
            while (farIndex < bytesActuallyRead && buffer_[farIndex] != 13 && buffer_[farIndex] != 10)
            {
                farIndex++;
            }
            if (nearIndex != farIndex)
            {
                size_t sindex;
                if (farIndex != bytesActuallyRead)
                {
                    sindex = farIndex;
                }
                else
                {
                    sindex = farIndex - 1;
                }
                std::string s(buffer_ + nearIndex, buffer_ + sindex);
                func_(s);
            }
        }
        CloseHandle(h);
    }
    std::string GetFilename() { return filename_; }
    HANDLE GetEvent() { return hEvent_; }
    virtual ~FastLineReader()
    {
        if (buffer_ != nullptr)
        {
            HeapFree(GetProcessHeap(), 0, buffer_);
        }
    }
};
#endif

inline void AddSlashIfMissing(std::string& s)
{
    if (s.back() != '/' && s.back() != '\\')
    {
        s += '\\';
    }
}

}
