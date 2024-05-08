

int WSAAPI bifbind(
    SOCKET         s,
    const sockaddr *name,
    int            namelen
) {
    struct sockaddr *paddr = (struct sockaddr*)name;
    unsigned short family = name->sa_family;
    if(family == AF_INET && ipv4_ifindex != 0) {
        paddr = (struct sockaddr*)_malloca(namelen);
        memcpy(paddr, name, namelen);
        if(((struct sockaddr_in*)paddr)->sin_addr.S_un.S_addr == INADDR_ANY) alter_ifaddr(paddr);
    }
    if(family == AF_INET6 && ipv6_ifindex != 0) {
        paddr = (struct sockaddr*)_malloca(namelen);
        memcpy(paddr, name, namelen);
        if(equalArray(&((struct sockaddr_in6*)paddr)->sin6_addr, &in6addr_any, 16)) alter_ifaddr(paddr);
    }
    int ret = trbind(s, paddr, namelen), error;
    if(ret != 0) if((error = WSAGetLastError()) != WSAEINVAL)
        printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
               " Error bind return in bind(): %d\n", error);
    return ret;
}

int WSAAPI bifconnect(
    SOCKET         s,
    const sockaddr *name,
    int            namelen
) {
    struct sockaddr *paddr;
    unsigned short family = name->sa_family;
    if((family == AF_INET && ipv4_ifindex != 0) || (family == AF_INET6 && ipv6_ifindex != 0)) {
        paddr = (struct sockaddr*)_malloca(namelen);
        memcpy(paddr, name, namelen);
        //((struct sockaddr_in*)paddr)->sin_addr.S_un.S_addr = INADDR_ANY;
        //((struct sockaddr_in6*)paddr)->sin6_addr = in6addr_any;
        ((struct sockaddr_in*)paddr)->sin_port = 0;
        alter_ifaddr(paddr);
        int ret = trbind(s, paddr, namelen), error;
        if(ret != 0) if((error = WSAGetLastError()) != WSAEINVAL) {
            printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
                   " Error bind return in connect(): %d\n", error);
            return ret;
        }
    }
    return trconnect(s, name, namelen);
}

int WSAAPI bifsendto(
    SOCKET         s,
    const char     *buf,
    int            len,
    int            flags,
    const sockaddr *to,
    int            tolen
) {
    struct sockaddr *paddr;
    unsigned short family = to->sa_family;
    if((family == AF_INET && ipv4_ifindex != 0) || (family == AF_INET6 && ipv6_ifindex != 0)) {
        paddr = (struct sockaddr*)_malloca(tolen);
        memcpy(paddr, to, tolen);
        //((struct sockaddr_in*)paddr)->sin_addr.S_un.S_addr = INADDR_ANY;
        //((struct sockaddr_in6*)paddr)->sin6_addr = in6addr_any;
        ((struct sockaddr_in*)paddr)->sin_port = 0;
        alter_ifaddr(paddr);
        int ret = trbind(s, paddr, tolen), error;
        if(ret != 0) if((error = WSAGetLastError()) != WSAEINVAL) {
            printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
                   " Error bind return in sendto(): %d\n", error);
            return ret;
        }
    }
    return trsendto(s, buf, len, flags, to, tolen);
}

int WSAAPI bifWSAConnect(
    SOCKET         s,
    const sockaddr *name,
    int            namelen,
    LPWSABUF       lpCallerData,
    LPWSABUF       lpCalleeData,
    LPQOS          lpSQOS,
    LPQOS          lpGQOS
) {
    struct sockaddr *paddr;
    unsigned short family = name->sa_family;
    if((family == AF_INET && ipv4_ifindex != 0) || (family == AF_INET6 && ipv6_ifindex != 0)) {
        paddr = (struct sockaddr*)_malloca(namelen);
        memcpy(paddr, name, namelen);
        //((struct sockaddr_in*)paddr)->sin_addr.S_un.S_addr = INADDR_ANY;
        //((struct sockaddr_in6*)paddr)->sin6_addr = in6addr_any;
        ((struct sockaddr_in*)paddr)->sin_port = 0;
        alter_ifaddr(paddr);
        int ret = trbind(s, paddr, namelen), error;
        if(ret != 0) if((error = WSAGetLastError()) != WSAEINVAL) {
            printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
                   " Error bind return in WSAConnect(): %d\n", error);
            return ret;
        }
    }
    return trWSAConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS);
}

int WSAAPI bifWSASendTo(
    SOCKET                             s,
    LPWSABUF                           lpBuffers,
    DWORD                              dwBufferCount,
    LPDWORD                            lpNumberOfBytesSent,
    DWORD                              dwFlags,
    const sockaddr                     *lpTo,
    int                                iTolen,
    LPWSAOVERLAPPED                    lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
) {
    struct sockaddr *paddr;
    unsigned short family = lpTo->sa_family;
    if((family == AF_INET && ipv4_ifindex != 0) || (family == AF_INET6 && ipv6_ifindex != 0)) {
        paddr = (struct sockaddr*)_malloca(iTolen);
        memcpy(paddr, lpTo, iTolen);
        //((struct sockaddr_in*)paddr)->sin_addr.S_un.S_addr = INADDR_ANY;
        //((struct sockaddr_in6*)paddr)->sin6_addr = in6addr_any;
        ((struct sockaddr_in*)paddr)->sin_port = 0;
        alter_ifaddr(paddr);
        int ret = trbind(s, paddr, iTolen), error;
        if(ret != 0) if((error = WSAGetLastError()) != WSAEINVAL) {
            printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
                   " Error bind return in WSASendTo(): %d\n", error);
            return ret;
        }
    }
    return trWSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iTolen, lpOverlapped, lpCompletionRoutine);
}

#if 0
BOOL PASCAL bifWSAConnectByList(
    SOCKET               s,
    PSOCKET_ADDRESS_LIST SocketAddress,
    LPDWORD              LocalAddressLength,
    LPSOCKADDR           LocalAddress,
    LPDWORD              RemoteAddressLength,
    LPSOCKADDR           RemoteAddress,
    const timeval        *timeout,
    LPWSAOVERLAPPED      Reserved
) {
    return FALSE;
}
#endif

//#define _WIN32_WINNT_WIN10 0x0A00
#if _WIN32_WINNT >= 0x0A00
static BOOL bifWSAConnectByNameCheck(SOCKET s) {
#if 0
    printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
           " Call detouring of WSAConnectByName().\n");
#endif
    DWORD ipv6only;
    int ret, family, slen, error;
    struct sockaddr_storage saddr;
    WSAPROTOCOL_INFOW proto_info;
    slen = sizeof(proto_info);
    ret = getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&proto_info, &slen);
    if(ret != 0) {
        error = WSAGetLastError();
        printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
               " Error SO_PROTOCOL_INFO in WSAConnectByName(): %d\n", error);
        return TRUE;
    }
    family = proto_info.iAddressFamily;
    if(family != AF_INET && family != AF_INET6) {
        printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
               " Error iAddressFamily in WSAConnectByName(): %d\n", family);
        return TRUE;
    }
    ret = getsockname(s, (struct sockaddr*)&saddr, &slen);
    if(ret == 0) {
        printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
               " Error already bind or connect in WSAConnectByName().\n");
        return TRUE;
    }
    if((error = WSAGetLastError()) != WSAEINVAL) {
        printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
               " Error SOCKET operation in WSAConnectByName().\n");
        return TRUE;
    }
    if(family == AF_INET) {
        struct sockaddr_in *addr = (struct sockaddr_in*)&saddr;
        slen = sizeof(struct sockaddr_in);
        memset(addr, 0, slen);
        addr->sin_family = AF_INET;
        addr->sin_port = 0;
        alter_ifaddr((struct sockaddr*)addr);
        ret = trbind(s, (struct sockaddr*)addr, slen);
        if(ret != 0) {
            error = WSAGetLastError();
            printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
                   " Error bind return in WSAConnectByName(): %d\n", error);
            return FALSE;
        }
    }
    else {
        slen = sizeof(ipv6only);
        ret = getsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&ipv6only, &slen);
        if(ret != 0) return TRUE;
        if(ipv6only == 0) return TRUE;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6*)&saddr;
        slen = sizeof(struct sockaddr_in6);
        memset(addr6, 0, slen);
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = 0;
        alter_ifaddr((struct sockaddr*)addr6);
        ret = trbind(s, (struct sockaddr*)addr6, slen);
        if(ret != 0) {
            error = WSAGetLastError();
            printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
                   " Error bind return in WSAConnectByName(): %d\n", error);
            return FALSE;
        }
    }
    return TRUE;
}
#else
#pragma warning(suppress : 4100)
static BOOL bifWSAConnectByNameCheck(SOCKET s) {
    return TRUE;
}
#endif

BOOL PASCAL bifWSAConnectByNameA(
    SOCKET          s,
    LPCSTR          nodename,
    LPCSTR          servicename,
    LPDWORD         LocalAddressLength,
    LPSOCKADDR      LocalAddress,
    LPDWORD         RemoteAddressLength,
    LPSOCKADDR      RemoteAddress,
    const timeval   *timeout,
    LPWSAOVERLAPPED Reserved
) {
    if(!bifWSAConnectByNameCheck(s)) return FALSE;
    return trWSAConnectByNameA(s, nodename, servicename, LocalAddressLength, LocalAddress, RemoteAddressLength, RemoteAddress, timeout, Reserved);
}

BOOL PASCAL bifWSAConnectByNameW(
    SOCKET          s,
    LPWSTR          nodename,
    LPWSTR          servicename,
    LPDWORD         LocalAddressLength,
    LPSOCKADDR      LocalAddress,
    LPDWORD         RemoteAddressLength,
    LPSOCKADDR      RemoteAddress,
    const timeval   *timeout,
    LPWSAOVERLAPPED Reserved
) {
    if(!bifWSAConnectByNameCheck(s)) return FALSE;
    return trWSAConnectByNameW(s, nodename, servicename, LocalAddressLength, LocalAddress, RemoteAddressLength, RemoteAddress, timeout, Reserved);
}

