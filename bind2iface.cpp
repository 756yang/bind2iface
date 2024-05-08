//////////////////////////////////////////////////////////////////////////////
//
//  Bind interface for Winsock API
//
//  Copyright (c) Yanglan Network.  All rights reserved.
//
//  This DLL will detour the Winsock API:
//  bind(), connect(), sendto(), WSAConnect(), WSASendTo(),
//  WSAConnectByNameA(), WSAConnectByNameW()
//  note: WSAConnectByList() cannot bind specified interface
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>
#include <malloc.h>
#include <mutex>
#include "detours.h"

#if 0
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x000
#endif

#include "alter_bind.cpp"
#include "bind_trampo.cpp"
#include "detour_bind.cpp"


int WINAPI BoundEntryPoint(VOID)
{
    LONG error;
    alter_ifindex();
    if(ipv4_ifindex == 0 && ipv6_ifindex == 0) return TrueEntryPoint();

    trbind = (int (WSAAPI *)(
        SOCKET,
        const sockaddr*,
        int
    )) DetourFindFunction("ws2_32.dll", "bind");

    trconnect = (int (WSAAPI *)(
        SOCKET,
        const sockaddr*,
        int
    )) DetourFindFunction("ws2_32.dll", "connect");

    trsendto = (int (WSAAPI *)(
        SOCKET,
        const char*,
        int,
        int,
        const sockaddr*,
        int
    )) DetourFindFunction("ws2_32.dll", "sendto");

    trWSAConnect = (int (WSAAPI *)(
        SOCKET,
        const sockaddr*,
        int,
        LPWSABUF,
        LPWSABUF,
        LPQOS,
        LPQOS
    )) DetourFindFunction("ws2_32.dll", "WSAConnect");

    trWSASendTo = (int (WSAAPI *)(
        SOCKET,
        LPWSABUF,
        DWORD,
        LPDWORD,
        DWORD,
        const sockaddr*,
        int,
        LPWSAOVERLAPPED,
        LPWSAOVERLAPPED_COMPLETION_ROUTINE
    )) DetourFindFunction("ws2_32.dll", "WSASendTo");

#if 0
    trWSAConnectByList = (BOOL (PASCAL *)(
        SOCKET,
        PSOCKET_ADDRESS_LIST,
        LPDWORD,
        LPSOCKADDR,
        LPDWORD,
        LPSOCKADDR,
        const timeval*,
        LPWSAOVERLAPPED
    )) DetourFindFunction("ws2_32.dll", "WSAConnectByList");
#endif

#if 1||(_WIN32_WINNT >= 0x0A00)
    trWSAConnectByNameA = (BOOL (PASCAL *)(
        SOCKET,
        LPCSTR,
        LPCSTR,
        LPDWORD,
        LPSOCKADDR,
        LPDWORD,
        LPSOCKADDR,
        const timeval*,
        LPWSAOVERLAPPED
    )) DetourFindFunction("ws2_32.dll", "WSAConnectByNameA");

    trWSAConnectByNameW = (BOOL (PASCAL *)(
        SOCKET,
        LPWSTR,
        LPWSTR,
        LPDWORD,
        LPSOCKADDR,
        LPDWORD,
        LPSOCKADDR,
        const timeval*,
        LPWSAOVERLAPPED
    )) DetourFindFunction("ws2_32.dll", "WSAConnectByNameW");
#endif

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)trbind, (PVOID)bifbind);
    DetourAttach(&(PVOID&)trconnect, (PVOID)bifconnect);
    DetourAttach(&(PVOID&)trsendto, (PVOID)bifsendto);
    DetourAttach(&(PVOID&)trWSAConnect, (PVOID)bifWSAConnect);
    DetourAttach(&(PVOID&)trWSASendTo, (PVOID)bifWSASendTo);
#if 0
    DetourAttach(&(PVOID&)trWSAConnectByList, (PVOID)bifWSAConnectByList);
#endif
#if _WIN32_WINNT >= 0x0A00
    DetourAttach(&(PVOID&)trWSAConnectByNameA, (PVOID)bifWSAConnectByNameA);
    DetourAttach(&(PVOID&)trWSAConnectByNameW, (PVOID)bifWSAConnectByNameW);
#endif
    error = DetourTransactionCommit();

    if(error != NO_ERROR) {
        printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
               " Error detouring Winsock2 API: %ld\n", error);
    }
    return TrueEntryPoint();
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    LONG error;
    (void)hinst;
    (void)reserved;

    if(DetourIsHelperProcess()) {
        return TRUE;
    }

    if(dwReason == DLL_PROCESS_ATTACH) {
        DetourRestoreAfterWith();
        TrueEntryPoint = (int (WINAPI *)(VOID))DetourGetEntryPoint(NULL);
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)TrueEntryPoint, (PVOID)BoundEntryPoint);
        error = DetourTransactionCommit();
        if(error != NO_ERROR) {
            printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
                   " Error detouring EntryPoint(): %ld\n", error);
        }
    }
    else if(dwReason == DLL_PROCESS_DETACH) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        if(trbind != NULL) DetourDetach(&(PVOID&)trbind, (PVOID)bifbind);
        if(trconnect != NULL) DetourDetach(&(PVOID&)trconnect, (PVOID)bifconnect);
        if(trsendto != NULL) DetourDetach(&(PVOID&)trsendto, (PVOID)bifsendto);
        if(trWSAConnect != NULL) DetourDetach(&(PVOID&)trWSAConnect, (PVOID)bifWSAConnect);
        if(trWSASendTo != NULL) DetourDetach(&(PVOID&)trWSASendTo, (PVOID)bifWSASendTo);
#if 0
        if(trWSAConnectByList != NULL) DetourDetach(&(PVOID&)trWSAConnectByList, (PVOID)bifWSAConnectByList);
#endif
#if _WIN32_WINNT >= 0x0A00
        if(trWSAConnectByNameA != NULL) DetourDetach(&(PVOID&)trWSAConnectByNameA, (PVOID)bifWSAConnectByNameA);
        if(trWSAConnectByNameW != NULL) DetourDetach(&(PVOID&)trWSAConnectByNameW, (PVOID)bifWSAConnectByNameW);
#endif
        DetourDetach(&(PVOID&)TrueEntryPoint, (PVOID)BoundEntryPoint);
        error = DetourTransactionCommit();
    }
    return TRUE;
}
//
///////////////////////////////////////////////////////////////// End of File.
