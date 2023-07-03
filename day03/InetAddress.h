#pragma once
#include <arpa/inet.h>

// 看文件名也能看出来，这个是地址类，通过这个类构建地址

class InetAddress {
public:

    // 地址结构体
    struct sockaddr_in addr;
    // 地址的长度
    socklen_t addrlen;


    InetAddress();
    InetAddress(const char* ip, uint16_t port);

    ~InetAddress();
};