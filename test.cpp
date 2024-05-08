
#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>   // Need for SO_UPDATE_CONNECT_CONTEXT
#include <stdio.h>

#include "verify.cpp"

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

SOCKET
OpenAndConnect(LPWSTR NodeName, LPWSTR PortName) 
{
    SOCKET ConnSocket = INVALID_SOCKET;
    int ipv6only = 0;
    int iResult;
    BOOL bSuccess;
    SOCKADDR_STORAGE LocalAddr = {0};
    SOCKADDR_STORAGE RemoteAddr = {0};
    DWORD dwLocalAddr = sizeof(LocalAddr);
    DWORD dwRemoteAddr = sizeof(RemoteAddr);
  
    ConnSocket = socket(AF_INET6, SOCK_STREAM, 0);
    if (ConnSocket == INVALID_SOCKET){
        wprintf(L"socket failed with error: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }

    iResult = setsockopt(ConnSocket, IPPROTO_IPV6,
        IPV6_V6ONLY, (char*)&ipv6only, sizeof(ipv6only) );
    if (iResult == SOCKET_ERROR){
        wprintf(L"setsockopt for IPV6_V6ONLY failed with error: %d\n",
            WSAGetLastError());
        closesocket(ConnSocket);
        return INVALID_SOCKET;       
    }

    bSuccess = WSAConnectByNameW(ConnSocket, NodeName, 
            PortName, &dwLocalAddr,
            (SOCKADDR*)&LocalAddr,
            &dwRemoteAddr,
            (SOCKADDR*)&RemoteAddr,
            NULL,
            NULL);
    if (!bSuccess){
        wprintf(L"WsaConnectByName failed with error: %d\n", WSAGetLastError());
        closesocket(ConnSocket);
        return INVALID_SOCKET;       

    
    }

    iResult = setsockopt(ConnSocket, SOL_SOCKET,
        SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
    if (iResult == SOCKET_ERROR){
        wprintf(L"setsockopt for SO_UPDATE_CONNECT_CONTEXT failed with error: %d\n",
            WSAGetLastError());
        closesocket(ConnSocket);
        return INVALID_SOCKET;       
    }

    return ConnSocket;
}

int __cdecl wmain(int argc, wchar_t **argv)
{
    Verify("bind", (PVOID)bind); printf("\n");
    Verify("connect", (PVOID)connect); printf("\n");
    Verify("sendto", (PVOID)sendto); printf("\n");
    Verify("WSAConnect", (PVOID)WSAConnect); printf("\n");
    Verify("WSASendTo", (PVOID)WSASendTo); printf("\n");
#if 0
    Verify("WSAConnectByList", (PVOID)WSAConnectByList); printf("\n");
#endif
#if _WIN32_WINNT >= 0x0A00
#pragma warning(suppress : 4996)
    Verify("WSAConnectByNameA", (PVOID)WSAConnectByNameA); printf("\n");
    Verify("WSAConnectByNameW", (PVOID)WSAConnectByNameW); printf("\n");
#endif
    //-----------------------------------------
    // Declare and initialize variables
    WSADATA wsaData;
    int iResult;

    SOCKET s = INVALID_SOCKET;

    // Validate the parameters
    if (argc != 3) {
        wprintf(L"usage: %ws <Nodename> <Portname>\n", argv[0]);
        wprintf(L"wsaconnectbyname establishes a connection to a specified host and port.\n");
        wprintf(L"%ws www.contoso.com 8080\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    wprintf(L"WsaConnectByName with following parameters:\n");
    wprintf(L"\tNodename = %ws\n", argv[1]);
    wprintf(L"\tPortname (or port) = %ws\n\n", argv[2]);

    //--------------------------------
    // Call our function that uses the WsaConnectByName. 
    
    s = OpenAndConnect(argv[1], argv[2]);
    if ( s == INVALID_SOCKET ) {
        wprintf(L"WsaConnectByName failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    else
    {
        wprintf(L"WsaConnectByName succeeded\n");
        
        closesocket(s);
        WSACleanup();
        return 0;
    }
}

