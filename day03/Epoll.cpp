#include "Epoll.h"
#include "util.h"
#include <cstring>
#include <unistd.h>


#define MAX_EVENTS 1024

Epoll::Epoll() : _epfd(-1), _events(nullptr) {
    _epfd = epoll_create1(0);
    errif (_epfd == -1, "epoll create error");
    _events = new epoll_event[MAX_EVENTS];
    memset(_events, 0, sizeof(*_events)*MAX_EVENTS);
}


Epoll::~Epoll() {
    if (_epfd != -1) {
        close(_epfd);
        _epfd = -1;
    }

    delete[] _events;
}


// 创建事件
void Epoll::addFd(int fd, uint32_t op) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
}

// 等待事件
std::vector<epoll_event> Epoll::poll(int timeout) {
    std::vector<epoll_event> activeEvents;
    int nfds = epoll_wait(_epfd, _events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; ++i) {
        activeEvents.push_back(_events[i]);
    }

    return activeEvents;
}

