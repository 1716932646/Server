#include "Channel.h"
#include "Epoll.h"

Channel::Channel(Epoll* ep, int fd) : _ep(ep), _fd(fd), _events(0), _revents(0), _isEpoll(false) {

}

Channel::~Channel() {
    
}

void Channel::enableReading() {
    _events = EPOLLIN | EPOLLET;
    _ep->updateChannel(this);
}

int Channel::getFd() {
    return _fd;
}

uint32_t Channel::getEvents() {
    return _events;
}

uint32_t Channel::getRevents() {
    return _revents;
}

bool Channel::getInEpoll() {
    return _isEpoll;
}

void Channel::setInEpoll() {
    _isEpoll = true;
}

void Channel::setRevents(uint32_t ev) {
    _revents = ev;
}

