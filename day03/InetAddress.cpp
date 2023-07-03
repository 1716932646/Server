#include "InetAddress.h"
#include <cstring>

// 客户端地址
InetAddress::InetAddress() : addrlen(sizeof(addr)) {
    memset(&addr, 0, sizeof(addr));
}

// 设置服务器地址
InetAddress::InetAddress(const char* ip, uint16_t port) : addrlen(sizeof(addr)) {
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addrlen = sizeof(addr);
}

InetAddress::~InetAddress() {

}