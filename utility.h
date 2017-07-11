#pragma once
#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <algorithm>

#ifdef UNICODE
typedef std::wstring unistring;
#else
typedef std::string unistring;
#endif

namespace ams
{
inline BOOL CreateDirectoryRec(unistring dir)
{
    BOOL createResult = FALSE;

    if (!dir.empty() && dir != _T("/"))
    {

        std::replace(dir.begin(), dir.end(), '\\', '/');
        // Break directory up into parts
        std::vector<unistring> parts;
        size_t findres;
        size_t n = 0;

        do
        {
            findres = dir.find_first_of('/', n + 1);
            unistring partial = dir.substr(n, findres - n);
            n = findres + 1;
            parts.push_back(partial);
        } while (findres != std::string::npos);

        unistring current;

        createResult = TRUE;
        for (auto p = parts.begin(); p != parts.end() && createResult; ++p)
        {
            current += *p;
            if (isalpha(current[0]) && current[1] == ':' && current.length() == 2)
            {
                current += '/';
            }

            auto atts = GetFileAttributes(current.c_str());
            if (atts == INVALID_FILE_ATTRIBUTES)
            {
                createResult = CreateDirectory(current.c_str(), NULL);

                // It's not a failure to attempt to create a directory if it already exists:
                DWORD error = GetLastError();
                if (error == ERROR_ALREADY_EXISTS)
                {
                    createResult = TRUE;
                }
            }
            current += '/';
        }
    }

    return createResult;
}

// Trim both ends of a string:
template <class T> void Trim(T& s)
{
    if (!s.empty())
    {
        T::iterator i1 = s.begin();
        for (; i1 != s.end() && isspace(*i1); i1++) {}
        if (i1 == s.end())
        {
            s.clear();
        }
        else
        {
            T::iterator i2 = s.end() - 1;
            for (; i2 > i1 &&  isspace(*i2); i2--) {}
            s = T(i1, i2 + 1);
        }
    }
}

inline bool EndsWith(const std::string& s, const std::string t)
{
	return s.length() >= t.length() && s.substr(s.length() - t.length(), t.length()) == t;
}

template <class T> T FindNonSpace(T& it)
{
    while (isspace(*it))
    {
        it++;
    }
    return it;
}

inline void OKMessageBox(TCHAR *s)
{
    ::MessageBox(NULL, s, _T("Message"), MB_OK);
}


// gives instance numbers to all instances of a class inherited from this
// but you must inherit like this:
// class Myclass : public HasInstanceNumber<Myclass> {...}

template <class T, typename I=int> class HasInstanceNumber
{
    typedef std::map<I, T*> MAP;
    static MAP s_map;
    static I s_instanceNumber;
    I instanceNumber_;
public:
    HasInstanceNumber<T, I>():instanceNumber_(s_instanceNumber++) {s_map[instanceNumber_] = static_cast<T*>(this);}
    ~HasInstanceNumber<T, I>() {s_map.erase(instanceNumber_);}
    inline I GetInstanceNumber() {return instanceNumber_;}
    static T* GetInstanceFromInstanceNumber(I instanceNumber)
    {
        T* result;
        MAP::iterator it = s_map.find(instanceNumber);
        if (it != s_map.end())
        {
            result = it->second; 
        }
        else
        {
            result = 0;
        }
        return result;
    }
};

// define statics:
template <class T, typename I> I HasInstanceNumber<T, I>::s_instanceNumber = 0;
// Note the rare second meaning of typename:
template <class T, typename I> typename HasInstanceNumber<T, I>::MAP HasInstanceNumber<T, I>::s_map;

// two dimensional array class - use round bracket to access an element:
template <class T> class C2D
{
    std::vector<T> m_v;
    int m_rows, m_cols;
public:
    C2D(int rows, int cols) {m_rows = rows; m_cols = cols; m_v.resize(rows*cols);}
    T& operator()(int row, int col) {return m_v[row * m_cols + col];}
};

} // end AMS

