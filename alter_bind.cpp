//////////////////////////////////////////////////////////////////////////////
//
//  Bind interface for Winsock API
//
//  Copyright (c) Yanglan Network.  All rights reserved.
//
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <malloc.h>
#include <mutex>
#endif
// just for mute warning
static void alter_ifindex(void);
static void alter_get_ifaddr(void);
static void alter_ifaddr(struct sockaddr *addr);

static std::mutex ifaddr_mutex;
static IF_INDEX ipv4_ifindex = 0;
static IF_INDEX ipv6_ifindex = 0;
static DWORD last_cache = 0;
static struct in_addr ipv4_addr = {0};
static struct in6_addr ipv6_addr = {0};

static void alter_ifindex(void)
{
    LPCSTR env_iface = "SOCK_BIND_IFACE_INDEX";
    LPCSTR env_iface6 = "SOCK_BIND_IFACE_INDEX_V6";
    DWORD dwRet;
    char val_iface[16];
    LPSTR val_iface2;
    // 查找 SOCK_BIND_IFACE_INDEX 环境变量并分析
    dwRet = GetEnvironmentVariableA(env_iface, val_iface, 16);
    if(0 == dwRet) ipv4_ifindex = 0;
    else if(16 < dwRet) {
        val_iface2 = (LPSTR)malloc(dwRet);
        if(NULL != val_iface2) {
            dwRet = GetEnvironmentVariableA(env_iface, val_iface2, dwRet);
            if(0 == dwRet) ipv4_ifindex = 0;
            else ipv4_ifindex = (IF_INDEX)strtol(val_iface2, NULL, 10);
            free(val_iface2);
        }
        else ipv4_ifindex = 0;
    }
    else ipv4_ifindex = (IF_INDEX)strtol(val_iface, NULL, 10);
    // 查找 SOCK_BIND_IFACE_INDEX_V6 环境变量并分析
    dwRet = GetEnvironmentVariableA(env_iface6, val_iface, 16);
    if(0 == dwRet) ipv6_ifindex = 0;
    else if(16 < dwRet) {
        val_iface2 = (LPSTR)malloc(dwRet);
        if(NULL != val_iface2) {
            dwRet = GetEnvironmentVariableA(env_iface6, val_iface2, dwRet);
            if(0 == dwRet) ipv6_ifindex = 0;
            else ipv6_ifindex = (IF_INDEX)strtol(val_iface2, NULL, 10);
            free(val_iface2);
        }
        else ipv6_ifindex = 0;
    }
    else ipv6_ifindex = (IF_INDEX)strtol(val_iface, NULL, 10);
    // 特殊值要求通过路由决定网络接口
    if(ipv4_ifindex == (IF_INDEX)(-1)) {
        struct sockaddr_in sa = {0};
        sa.sin_family = AF_INET;
        inet_pton(AF_INET, "1.1.1.1", &sa.sin_addr);
        GetBestInterfaceEx((struct sockaddr*)&sa, &ipv4_ifindex);
    }
    if(ipv6_ifindex == (IF_INDEX)(-1)) {
        struct sockaddr_in6 sa6 = {0};
        sa6.sin6_family = AF_INET6;
        inet_pton(AF_INET6, "2001:4860:4860::8888", &sa6.sin6_addr);
        GetBestInterfaceEx((struct sockaddr*)&sa6, &ipv6_ifindex);
    }
    if(ipv4_ifindex == (IF_INDEX)(-1)) ipv4_ifindex = ipv6_ifindex;
    if(ipv6_ifindex == (IF_INDEX)(-1)) ipv6_ifindex = ipv4_ifindex;
    // 打印接口绑定日志
    if(ipv4_ifindex != 0) {
        printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
               " IPv4 bind interface IF_INDEX: %lu\n", ipv4_ifindex);
    }
    if(ipv6_ifindex != 0) {
        printf("bind2iface" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: "
               " IPv6 bind interface IF_INDEX: %lu\n", ipv6_ifindex);
    }
}

inline int equalArray(const void *p, const void *m, uint32_t size)
{
    int32_t r, d;
    r = 32 - 4 * (size & 0x3);
    d = size >> 2;
    const uint32_t *pos = (uint32_t*)((uint8_t*)p + 4 * d);
    const uint32_t *mpos = (uint32_t*)((uint8_t*)m + 4 * d);
    for(d = -d; d != 0; ++d)
        if(pos[d] != mpos[d]) return 0;
    if(r != 32) if((pos[0] << r) != (mpos[0] << r)) return 0;
    return 1;
}

inline uint32_t bigend_byte2uint32(const uint8_t *p)
{
    uint32_t ret = 0;
    for(int i = 0;i < 4;i++) ret = (ret << 8) | p[i];
    return ret;
}

/**
 * 获取接口IPv4地址的绑定优先级
 * 返回值 与IP范围的关系
 * 0 in 未定义
 * 1 in 0.0.0.0/8
 * 2 in 224.0.0.0/3
 * 3 in 127.0.0.0/8
 * 4 in 169.254.0.0/16
 * 5 in 198.18.0.0/15
 * 6 in 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
 * 7 in 100.64.0.0/10
 * 8 in 其他(通常为公网IPv4)
 */
static int get_ipv4_scores(const void *addr)
{
    uint32_t ip = bigend_byte2uint32((const uint8_t*)addr);
    if(ip < 0x01000000) return 1;
    if(ip >= 0xe0000000) return 2;
    if(ip >= 0x7f000000 && ip < 0x80000000) return 3;
    if(ip >= 0xa9fe0000 && ip < 0xa9ff0000) return 4;
    if(ip >= 0xc6120000 && ip < 0xc6140000) return 5;
    if(ip >= 0x0a000000 && ip < 0x0b000000) return 6;
    if(ip >= 0xac100000 && ip < 0xac200000) return 6;
    if(ip >= 0xc0a80000 && ip < 0xc0a90000) return 6;
    if(ip >= 0x64400000 && ip < 0x64800000) return 7;
    return 8;
}

/**
 * 获取接口IPv6地址的绑定优先级(ip_part为IPv6的最高四字节)
 * 返回值 与IPv6范围的关系
 * 0 in 未定义
 * 1 in ::/128, fe80::/10, ff00::/8
 * 2 in ::1/128
 * 3 in 其他(通常为特殊IPv6)
 * 4 in fec0::/10
 * 6 in fc00::/7
 * 8 in 2000::/3
 */
static int get_ipv6_scores(const void *addr6)
{
    uint32_t ip_part = bigend_byte2uint32((const uint8_t*)addr6);
    if(ip_part == 0 || ip_part >= 0xff000000) return 1;
    if(ip_part >= 0xfe800000 && ip_part < 0xfe810000) return 1;
    if(ip_part == 1) return 2;
    if(ip_part >= 0xfec00000) return 4;
    if(ip_part >= 0xfc000000 && ip_part < 0xfe000000) return 6;
    if(ip_part >= 0x20000000 && ip_part < 0x40000000) return 8;
    return 3;
}

static void alter_get_ifaddr(void)
{
    if(ipv4_ifindex == 0 && ipv6_ifindex == 0) return;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast;
    PIP_ADAPTER_ADDRESSES pCurrAddresses, pAddresses = NULL;
    ULONG outBufLen = 15000;
    ULONG flags = GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_MULTICAST|GAA_FLAG_SKIP_DNS_SERVER|GAA_FLAG_SKIP_FRIENDLY_NAME;
    DWORD dwRetVal = 0;
    for(int i = 0; i < 3; ++i) {
        pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
        if (pAddresses == NULL) {
            memset(&ipv4_addr, 0xff, sizeof(struct in_addr));
            memset(&ipv6_addr, 0xff, sizeof(struct in6_addr));
            return;
        }
        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, pAddresses, &outBufLen);
        if(dwRetVal != ERROR_BUFFER_OVERFLOW) break;
        free(pAddresses);
        pAddresses = NULL;
    }

    if(dwRetVal != NO_ERROR) {
        if(pAddresses) free(pAddresses);
        memset(&ipv4_addr, 0xff, sizeof(struct in_addr));
        memset(&ipv6_addr, 0xff, sizeof(struct in6_addr));
        return;
    }

    int ip_scores, scores;
    uint8_t *pip_addr, *paddr;
    for(pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next) {
        if(ipv4_ifindex == pCurrAddresses->IfIndex) {
            ip_scores = 0;
            pip_addr = NULL;
            for(pUnicast = pCurrAddresses->FirstUnicastAddress; pUnicast != NULL; pUnicast = pUnicast->Next) {
                if (AF_INET == pUnicast->Address.lpSockaddr->sa_family) {
                    paddr = (uint8_t*)&((struct sockaddr_in*)pUnicast->Address.lpSockaddr)->sin_addr;
                    scores = get_ipv4_scores(paddr);
                    if(scores > ip_scores) {
                        ip_scores = scores;
                        pip_addr = paddr;
                    }
                }
            }
            if(pip_addr) memcpy(&ipv4_addr, pip_addr, sizeof(struct in_addr));
        }
        if(ipv6_ifindex == pCurrAddresses->Ipv6IfIndex) {
            ip_scores = 0;
            pip_addr = NULL;
            for(pUnicast = pCurrAddresses->FirstUnicastAddress; pUnicast != NULL; pUnicast = pUnicast->Next) {
                if (AF_INET6 == pUnicast->Address.lpSockaddr->sa_family) {
                    paddr = (uint8_t*)&((struct sockaddr_in6*)pUnicast->Address.lpSockaddr)->sin6_addr;
                    scores = get_ipv6_scores(paddr);
                    if(pUnicast->PreferredLifetime != 0) {
                        if(scores >= 4 && pUnicast->SuffixOrigin == IpSuffixOriginRandom) scores++;
                        if(scores > ip_scores) {
                            ip_scores = scores;
                            pip_addr = paddr;
                        }
                    }
                }
            }
            if(pip_addr) memcpy(&ipv6_addr, pip_addr, sizeof(struct in6_addr));
        }
    }
    if(pAddresses) free(pAddresses);
}

/**
 * Change socket address to iface
 */
static void alter_ifaddr(struct sockaddr *addr)
{
    unsigned short family = addr->sa_family;
    ifaddr_mutex.lock();
    LONG now_delay = (LONG)(last_cache - GetTickCount());
    if(now_delay <= 0 || now_delay - 2000 > 0 || last_cache == 0) {
        last_cache -= now_delay - 2000;
        alter_get_ifaddr();
    }
    ifaddr_mutex.unlock();
    if(family == AF_INET){
        ((struct sockaddr_in*)addr)->sin_addr = ipv4_addr;
    }
    else if(family == AF_INET6){
        ((struct sockaddr_in6*)addr)->sin6_addr = ipv6_addr;
    }
}

//
///////////////////////////////////////////////////////////////// End of File.
