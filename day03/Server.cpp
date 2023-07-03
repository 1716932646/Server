#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "util.h"
#include <cstring>
#include <iostream>
#include <unistd.h>

#define READ_BUF_SIZE 1024


/**
 * ***** InetAddress 
 * ***** Socket
 * ***** Epoll  
*/


void handleReadEvent(int);

int main() {
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    Socket *serv_sock = new Socket();
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    Epoll *ep = new Epoll();
    serv_sock->setnonblocking();
    ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);
    

    while (true) {
        std::vector<epoll_event> events = ep->poll();
        int nfds = events.size();
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == serv_sock->getFd()) {   // 新客户端连接
                InetAddress *clnt_addr = new InetAddress();
                Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr)); // 通过accept方法得到fd创建sockfd
                printf("new client fd: %d Ip: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));

                clnt_sock->setnonblocking();
                ep->addFd(clnt_sock->getFd(), EPOLLIN | EPOLLET);
            } else if (events[i].events & EPOLLIN) {   // 可读事件
                handleReadEvent(events[i].data.fd);
            } else {
                printf("do other things...\n");
            }
        }
    }

    delete serv_addr;
    delete serv_sock;
    return 0;
}

void handleReadEvent(int sockfd) {
    char buf[READ_BUF_SIZE];

    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));

        if (read_bytes > 0) {
            printf("message from client fd: %d %s", sockfd, buf);
        } else if (read_bytes == -1 && errno == EINTR) {   // 正常中断，继续读
            printf("reading continue...");
            continue;
        }  else if ((read_bytes == -1) && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("finished reading once errno: %d\n", errno);
            break;
        } else if (read_bytes == 0) {   // 客户端断开连接
            printf("client fd: %d disconnected\n", sockfd);
            close(sockfd);
            break;
        }
    }
}