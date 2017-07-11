#pragma once

// Get the address of an extension function from the Microsoft winsock extension dll mswsock.dll
namespace ams
{
    inline void* GetExtensionFunction(GUID guid)
    {
        // create a socket just once - unfortunately it MUST be valid even for just getting
        // extension functions:
        static SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        assert(s != INVALID_SOCKET);
        void* result;
        DWORD bytesreturned;

        BOOL res = WSAIoctl(s,
            SIO_GET_EXTENSION_FUNCTION_POINTER,
            &guid,
            sizeof(guid),
            &result,
            sizeof(void*),
            &bytesreturned,   // unused for SIO_GET_EXTENSION_FUNCTION_POINTER
            NULL,
            NULL
            ); 
        return result;
    }
}
