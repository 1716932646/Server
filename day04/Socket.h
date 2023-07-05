#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>

class InetAddress;

class Socket {
private:
    int _fd;

public:
    Socket();
    Socket(int fd);
    ~Socket();

    void bind(InetAddress*);
    void listen();
    void setnonblocking();
    int accept(InetAddress*);

    int getFd();
};

#endif