#include <cstring>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <vector>

#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER_SIZE 1024


// 设置非阻塞
void setnonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}


int main() {

    // 创建socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket error");

    // 创建服务器地址，清空地址，绑定tcp、ip、port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    // 绑定
    errif(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "bind error");

    // 监听
    errif(listen(sockfd, SOMAXCONN) == -1, "listen error");

    // 创建epoll
    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll_create error");

    // 创建多个/单个事件
    struct epoll_event events[MAX_EVENTS], ev;
    memset(events, 0, sizeof(events));
    memset(&ev, 0, sizeof(ev));
    
    // 绑定事件
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    // 设置非阻塞
    setnonblocking(sockfd);
    // 改变事件
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    // 存储客户端套接字描述符
    std::vector<int> client_fds;


    // 循环
    while (true) {
        // 等待事件发生
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(nfds == -1, "epoll wait error");

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == sockfd) {    // 新客户端连接
                struct sockaddr_in clnt_addr;
                memset(&clnt_addr, 0, sizeof(clnt_addr));
                socklen_t len = sizeof(clnt_addr);

                int clnt_sockfd = accept(sockfd, (struct sockaddr*)&clnt_addr, &len);
                errif(clnt_sockfd == -1, "socket accept error");
                printf("new client fd: %d Ip: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                memset(&ev, 0, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);

                // 添加所有客户端的sockfd
                client_fds.push_back(clnt_sockfd);

            } else if (events[i].events & EPOLLIN) {    // 可读事件
                
                // 发送给服务器的buff
                char buf[READ_BUFFER_SIZE];

                // 发送给其他客户端的buff
                char bufs[READ_BUFFER_SIZE + READ_BUFFER_SIZE];
                // 获取发送者的sockfd
                int sender_id = events[i].data.fd;

                while (true) {
                    memset(buf, 0, sizeof(buf));
                    // 把读取的内容保存到buf中
                    ssize_t read_bytes = read(sender_id, buf, sizeof(buf));
                    
                    // 读取内容
                    if (read_bytes > 0) {
                        memset(bufs, 0, sizeof(bufs));
                        printf("message from client fd: %d %s", sender_id, buf);

                        int n = snprintf(bufs, sizeof(bufs), "message from client fd: %d %s", sender_id, buf);

                        // 把读取的信息给所有的客户端发送过去，除了发送者的
                        for (int receive_fd : client_fds) {
                            if (receive_fd != sender_id) {
                                write(receive_fd, bufs, sizeof(bufs));
                            }
                        }

                    } else if (read_bytes == -1 && errno == EINTR) {   // 客户端正常中断，继续读取
                        printf("continue reading\n");
                        continue;
                    } else if (read_bytes == -1 && ((errno == EAGAIN) | (errno == EWOULDBLOCK))) { // 非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if (read_bytes == 0) {   // 客户端断开连接
                        // 断开就把sockfd去掉
                        printf("client fd %d disconnected\n", sender_id);
                        for (auto it = client_fds.begin(); it != client_fds.end(); ++it) {
                            if (*it == sender_id) {
                                client_fds.erase(it);
                                break;
                            }
                        }
                        close(sender_id);
                        break;
                    }
                }
            } else {
                printf("other things\n");
            }
        }
    }

    // 关闭socket
    close(sockfd);
    
    return 0;
}