#pragma once

// For use with the Windows Template Library (WTL). Requires boost from http://www.boost.org.
// For information on object factories see Modern C++ Design: Generic Programming and Design Patterns Applied,
// by Andrei Alexandrescu ISBN: 0201704315

// NOTE...all dialogs produced by the factory MUST INHERIT FROM FactoryDialog like this:
// class MyDialog : public FactoryDialog {...}

// The minimum requirement for a dialog is:
//
//
// class MyDialog: public FactoryDialog
// {
// public:
//     BEGIN_MSG_MAP(MyDialog)
//         MSG_WM_INITDIALOG(OnInitDialog)
//     END_MSG_MAP()
// 
//     MyDialog()
//     {
//         IDD = IDD_TESTDIALOG;
//     }
// 
//     LRESULT OnInitDialog(HWND hwnd, LPARAM lParam)
//     {
//         return TRUE;
//     }
// };
// // Register the dialog with the factory:
// REGISTER_DIALOG(IDD_TESTDIALOG, MyDialog)




#include <map>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>

template <class T> class DialogFactory : public boost::noncopyable
{
    typedef T* (*DialogCreatorFunc)();
    typedef std::map<UINT, DialogCreatorFunc> MAP;
    MAP iddmap_;
public:
    bool RegisterDialog(UINT idd, DialogCreatorFunc f)
    {
        return iddmap_.insert(MAP::value_type(idd, f)).second;
    }
    T* CreateObject(UINT idd)
    {
        MAP::const_iterator p = iddmap_.find(idd);
        if (p != iddmap_.end())
        {
            return (p->second)();
        }
        else
        {
            return NULL;
        }
    }
    DialogFactory(void) {}
    ~DialogFactory(void) {}
};

// base class dialog from which all others must inherit:
class FactoryDialog : public CDialogImpl<FactoryDialog>
{
public:
    UINT IDD;
//    BEGIN_MSG_MAP(NULL)
//    END_MSG_MAP()

};

extern DialogFactory<FactoryDialog> g_dialogFactory;


// Macro to register the dialog with the dialog factory:
#define REGISTER_DIALOG(idd, classname) \
    namespace {\
    const UINT IDDS = idd;\
    FactoryDialog* Create() {return new classname;}\
    bool registered = g_dialogFactory.RegisterDialog(IDDS, Create);\
    }

__declspec(selectany) DialogFactory<FactoryDialog> g_dialogFactory;

inline FactoryDialog* CreateFactoryDialog(UINT idd)
{
    return g_dialogFactory.CreateObject(idd);
}

inline INT_PTR DoFactoryDialogModal(UINT idd)
{
    boost::scoped_ptr<FactoryDialog> pDlg(CreateFactoryDialog(idd));
    return pDlg->DoModal();
}
