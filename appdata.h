#pragma once
#include <ShlObj.h>
#include <ams/utility.h>
#include <tchar.h>

namespace ams
{
#if 0
}// fool visual assist
#endif

typedef std::basic_string<TCHAR> Tstring;

class AppData
{
    std::string folder_;
public:
    AppData(std::string company, std::string appname, bool create)
    {
        std::vector<char> appdataFolder(_MAX_PATH);
        HRESULT hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, &appdataFolder[0]);
        if (!SUCCEEDED(hr))
        {
            throw std::exception("Bad!");
        }

        std::string folder = &appdataFolder[0];
        if (folder[folder.length() - 1] != '\\')
        {
            folder += _T("\\");
        }

        folder += company;
        folder += _T("\\");
        folder += appname;

        DWORD atts = GetFileAttributes(folder.c_str());
        if (atts != INVALID_FILE_ATTRIBUTES)
        {
            if (!(atts & FILE_ATTRIBUTE_DIRECTORY))
            {
                throw;
            }
        }
        else if (create)
        {
            ams::CreateDirectoryRec(folder);
        }

        // store computed and possibly created folder in member data
        folder_ = folder;
    }

    void CreateRoaming() const 
    {
        
    }

    void CreateLocal() const 
    {

    }

    void GetRoaming(std::string& roaming) const 
    {
        roaming = folder_;
    }
    
    void GetLocal(std::string& local) const 
    {

    }

    void GetRoamingPathname(std::string& path, const std::string filename) const 
    {
        path = folder_ + _T("\\") + filename;
    }

    void GetLocalPathname(std::string& path, const std::string filename) const 
    {

    }


};
}// namespace ams

