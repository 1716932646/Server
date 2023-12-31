#pragma once

#include <sys/epoll.h>
#include <vector>

class Epoll {
private:
    int _epfd;
    struct epoll_event *_events;

public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t op);
    std::vector<epoll_event> poll(int timeout = -1);
};