#pragma once
#include <windows.h>

inline void ioerror(char *s)
{
    std::cout << "ERROR " << s << std::endl;
}


inline void OpenStation()
{
    HWINSTA hwinstaUser;
    hwinstaUser=OpenWindowStation("winsta0", FALSE, MAXIMUM_ALLOWED);
    if(!hwinstaUser)
            ioerror("OpenWindowStation");
    if(!SetProcessWindowStation(hwinstaUser))
            ioerror("SetProcessWindowStation");
    HDESK hdeskUser=OpenDesktop("Default", 0, FALSE, MAXIMUM_ALLOWED); /* Winlogon */
    if(!hdeskUser)
            ioerror("OpenDesktop");
    if(!SetThreadDesktop(hdeskUser))
            ioerror("SetThreadDesktop");
}




inline void MsgBox(const char *s)
{
    MessageBox(NULL, s, "CGI SCRIPT", MB_OK);
}

