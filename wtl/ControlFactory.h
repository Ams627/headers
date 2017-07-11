#pragma once

// For use with the Windows Template Library (WTL). Requires boost from http://www.boost.org.
// For information on object factories see Modern C++ Design: Generic Programming and Design Patterns Applied,
// by Andrei Alexandrescu ISBN: 0201704315

#include <map>
#include <boost/utility.hpp>

#define WM_CONTROLINIT WM_APP + 1
#define WM_CONTROLEXIT WM_APP + 2

namespace {
struct GUIDCompare {
    bool operator()(const GUID g1, const GUID g2) const {
        const DWORD* p1 = reinterpret_cast<const DWORD*>(&g1);
        const DWORD* p2 = reinterpret_cast<const DWORD*>(&g2);
        bool stop = false;
        bool result = false;
        for (int i = 0; i < sizeof(GUID) / sizeof(DWORD) && !stop; ++i)
        {
            if (p1[i] != p2[i])
            {
                stop = true;
                result = p1[i] < p2[i];
            }
        }
        return result;
    }
};
}


class FactoryControl : public CWindowImpl<FactoryControl, CWindow>
{
};

template <class T> class ControlFactory //: public boost::noncopyable
{
    typedef T* (*ControlCreatorFunc)();
    typedef std::map<GUID, ControlCreatorFunc, GUIDCompare> MAP;
    MAP controlIDMap_;
public:
    bool RegisterControl(GUID idcontrol, ControlCreatorFunc f)
    {
        return controlIDMap_.insert(MAP::value_type(idcontrol, f)).second;
    }
    T* CreateObject(GUID idcontrol)
    {
        MAP::const_iterator p =
        controlIDMap_.find(idcontrol);

         if (p != controlIDMap_.end())
         {
             return (p->second)();
         }
         else
         {
             return NULL;
         }

        return NULL;
    }
    ControlFactory(void) {}
    ~ControlFactory(void) {}
};


__declspec(selectany) ControlFactory<CWindow> g_controlFactory;

  inline CWindow* CreateFactoryControl(GUID idcontrol)
  {
      return g_controlFactory.CreateObject(idcontrol);
  }

  //    const GUID IDDCONTROL = idcontrol;

// Macro to register the control with the control factory:
#define REGISTER_CONTROL(idcontrol, classname) \
    namespace {\
    CWindow* Create() {return new classname;}\
    bool registered = g_controlFactory.RegisterControl(idcontrol, Create);\
}

// helper functions:


  typedef std::map<UINT, GUID> IDDTOGUIDMAP;
  typedef std::map<UINT, GUID>::const_iterator IDDTOGUIDMAPIT;

  inline BOOL CALLBACK EnumDialogChildren(HWND hwnd, LPARAM lParam)
  {
      IDDTOGUIDMAP* pIddmap = reinterpret_cast<IDDTOGUIDMAP*>(lParam);

      UINT idd = GetDlgCtrlID(hwnd);

      IDDTOGUIDMAPIT mapit = pIddmap->find(idd);

      if (mapit != pIddmap->end())
      {
          CWindow* pWindow = CreateFactoryControl(mapit->second);
          if (pWindow != NULL)
          {
              FactoryControl* pFactoryControl = (FactoryControl*)(pWindow);
              pFactoryControl->SubclassWindow(hwnd);
          }
      }
      return TRUE;
  }


    inline void SubclassDialogItems(HWND dialogWindow, IDDTOGUIDMAP iddmap)
    {
        EnumChildWindows(dialogWindow, EnumDialogChildren, (LPARAM)&iddmap);
    }






#define BEGIN_IDDMAP(theClass) BOOL SubclassDialogItems()

