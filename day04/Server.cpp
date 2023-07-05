#include <iostream>
#include <cstring>
#include <errno.h>
#include <unistd.h>

#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include "Epoll.h"

#define READ_BUFFER_SIZE 1024

void handleReadEvent(int);

int main() {
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    Socket *serv_sock = new Socket();
    serv_sock->bind(serv_addr);
    serv_sock->listen();

    Epoll *ep = new Epoll();
    serv_sock->setnonblocking();
    Channel *serv_channel = new Channel(ep, serv_sock->getFd());
    serv_channel->enableReading();

    while (true) {
        std::vector<Channel*> activeChannels = ep->pool();
        int nfds = activeChannels.size();
        for (int i = 0; i < nfds; i++) {
            int chfd = activeChannels[i]->getFd();
            if (chfd == serv_sock->getFd()) {
                InetAddress *clnt_addr = new InetAddress();
                Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));
                printf("new client fd: %d Ip: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), htons(clnt_addr->addr.sin_port));

                clnt_sock->setnonblocking();
                Channel *clnt_channel = new Channel(ep, clnt_sock->getFd());
                clnt_channel->enableReading();
            } else if (activeChannels[i]->getRevents() & EPOLLIN) {
                handleReadEvent(activeChannels[i]->getFd());
            } else {
                printf("do other things\n");
            }
        }
    }

    delete serv_addr;
    delete serv_sock;

    return 0;
}

void handleReadEvent(int sockfd) {
    char buf[READ_BUFFER_SIZE];

    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));

        if (read_bytes > 0) {
            printf("message from client: %d %s\n", sockfd, buf);
        } else if (read_bytes == -1 && errno == EINTR) {
            printf("continue reading\n");
            continue;
        } else if (read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            printf("reading finished errno: %d\n", errno);
            break;
        } else if (read_bytes == 0) {
            printf("client fd: %d disconnected\n", sockfd);
            close(sockfd);
            break;
        }
    }
}