//////////////////////////////////////////////////////////////////////////////
//
//  Bind interface for Winsock API
//
//  Copyright (c) Yanglan Network.  All rights reserved.
//
static int (WINAPI * TrueEntryPoint)(VOID) = NULL;

static int (WSAAPI *trbind)(
    SOCKET         s,
    const sockaddr *name,
    int            namelen
) = NULL;

static int (WSAAPI *trconnect)(
    SOCKET         s,
    const sockaddr *name,
    int            namelen
) = NULL;

static int (WSAAPI *trsendto)(
    SOCKET         s,
    const char     *buf,
    int            len,
    int            flags,
    const sockaddr *to,
    int            tolen
) = NULL;

static int (WSAAPI *trWSAConnect)(
    SOCKET         s,
    const sockaddr *name,
    int            namelen,
    LPWSABUF       lpCallerData,
    LPWSABUF       lpCalleeData,
    LPQOS          lpSQOS,
    LPQOS          lpGQOS
) = NULL;

static int (WSAAPI *trWSASendTo)(
    SOCKET                             s,
    LPWSABUF                           lpBuffers,
    DWORD                              dwBufferCount,
    LPDWORD                            lpNumberOfBytesSent,
    DWORD                              dwFlags,
    const sockaddr                     *lpTo,
    int                                iTolen,
    LPWSAOVERLAPPED                    lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
) = NULL;

#if 0
static BOOL (PASCAL *trWSAConnectByList)(
    SOCKET               s,
    PSOCKET_ADDRESS_LIST SocketAddress,
    LPDWORD              LocalAddressLength,
    LPSOCKADDR           LocalAddress,
    LPDWORD              RemoteAddressLength,
    LPSOCKADDR           RemoteAddress,
    const timeval        *timeout,
    LPWSAOVERLAPPED      Reserved
) = NULL;
#endif

#if 1||(_WIN32_WINNT >= 0x0A00)
static BOOL (PASCAL *trWSAConnectByNameA)(
    SOCKET          s,
    LPCSTR          nodename,
    LPCSTR          servicename,
    LPDWORD         LocalAddressLength,
    LPSOCKADDR      LocalAddress,
    LPDWORD         RemoteAddressLength,
    LPSOCKADDR      RemoteAddress,
    const timeval   *timeout,
    LPWSAOVERLAPPED Reserved
) = NULL;

static BOOL (PASCAL *trWSAConnectByNameW)(
    SOCKET          s,
    LPWSTR          nodename,
    LPWSTR          servicename,
    LPDWORD         LocalAddressLength,
    LPSOCKADDR      LocalAddress,
    LPDWORD         RemoteAddressLength,
    LPSOCKADDR      RemoteAddress,
    const timeval   *timeout,
    LPWSAOVERLAPPED Reserved
) = NULL;
#endif
//
///////////////////////////////////////////////////////////////// End of File.
