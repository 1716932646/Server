#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#include "util.h"

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

    // 创建服务器地址
    struct sockaddr_in clit_addr;
    bzero(&clit_addr, sizeof(clit_addr));
    // 计算服务器地址长度，accept需要
    socklen_t len = sizeof(clit_addr);

    // 接收
    int clit_sockfd = accept(sockfd, (struct sockaddr *)&clit_addr, &len);
    errif(clit_sockfd == -1, "accept error");

    // 循环
    while (true) {
        // 缓存区
        char buf[1024];
        memset(buf, 0, sizeof(buf));

        ssize_t read_bytes = read(clit_sockfd, buf, sizeof(buf));

        // 读取到内容
        if (read_bytes > 0) {
            printf("message from client fd %d : %s", clit_sockfd, buf);
        } else if (read_bytes == 0) { // 客户端关闭连接
            printf("client fd %d is diconnected\n", clit_sockfd);
            close(clit_sockfd);
            break;
        } else if (read_bytes == -1) { // 服务器崩溃
            close(clit_sockfd);
            errif("true", "socket read error");
        }
    }

    // 关闭socket
    close(sockfd);
    
    return 0;

}