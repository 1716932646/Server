#include "Socket.h"
#include "util.h"
#include "InetAddress.h"

#include <fcntl.h>
#include <unistd.h>

Socket::Socket() : _fd(-1) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(_fd == -1, "socket create error!");
}

Socket::Socket(int fd) : _fd(fd) {
    errif(_fd == -1, "socket create error!");
}

int Socket::getFd() {
    return _fd;
}

void Socket::bind(InetAddress* address) {
    errif(::bind(_fd, (struct sockaddr*)&address->addr, address->addrlen) == -1, "socket bind error!");
}

void Socket::listen() {
    errif(::listen(_fd, SOMAXCONN) == -1, "socket listen error!");
}

void Socket::setnonblocking() {
    fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
}


int Socket::accept(InetAddress* address) {
    int clnt_sockfd = ::accept(_fd, (struct sockaddr *)&address->addr, &address->addrlen);
    errif(clnt_sockfd == -1, "socket accept error!");
    return clnt_sockfd;
}

Socket::~Socket() {
    if (_fd != -1) {
        close(_fd);
    }
    _fd = -1;
}
