#pragma once

class InetAddress;

// socket连接的几个方法
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