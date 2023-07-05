#include "Epoll.h"
#include "util.h"
#include "Channel.h"

#define MAX_EVENTS 1024

Epoll::Epoll() : _epfd(-1), events(nullptr) {
    _epfd = epoll_create1(0);
    errif(_epfd == -1, "epoll create error!");
    events = new epoll_event[MAX_EVENTS];
    memset(events, 0, sizeof(*events)*MAX_EVENTS);
}

Epoll::~Epoll() {
    if (_epfd != -1) {
        close(_epfd);
    }
    _epfd = -1;

    delete[] events;
}


// 保存每一个事件的指针和返回的事件，放入要等待的epoll中
std::vector<Channel*> Epoll::pool(int timeout) {
    std::vector<Channel*> activeChannels;
    int nfds = epoll_wait(_epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error!");

    for (int i = 0; i < nfds; ++i) {
        Channel *ch = (Channel*) events[i].data.ptr;
        ch->setRevents(events[i].events);
        activeChannels.push_back(ch);
    }

    return activeChannels;
}

// 通过事件指针，初始化上树或者修改
void Epoll::updateChannel(Channel* channel) {
    int fd = channel->getFd();
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents();

    if (!channel->getInEpoll()) {
        errif(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        channel->setInEpoll();
    } else {
        errif(epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll mod error");
    }
}

