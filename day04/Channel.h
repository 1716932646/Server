#ifndef CHANNEL_H
#define CHANNEL_H

#include "Epoll.h"
#include <sys/epoll.h>


/**
 * 控制Epoll中事件的一个类
*/

class Epoll;
class Channel {
private:
    Epoll* _ep;
    int _fd;

    // 事件
    uint32_t _events;
    
    // epoll返回该channel时文件描述符正在发生的事件
    uint32_t _revents;

    // 是否在Epoll树中
    bool _isEpoll;

public:
    Channel(Epoll* ep, int fd);
    ~Channel();

    
    void enableReading();
    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();

    bool getInEpoll();
    void setInEpoll();

    void setRevents(uint32_t);

};


#endif