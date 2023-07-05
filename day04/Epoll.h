#ifndef EPOLL_H
#define EPOLL_H

#include <vector>
#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>

class Channel;
class Epoll {
private:
    int _epfd;
    struct epoll_event *events;

public:
    Epoll();
    ~Epoll();

    void updateChannel(Channel*);
    std::vector<Channel*> pool(int timeout = -1);
};

#endif