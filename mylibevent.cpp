#include "mylibevent.h"
#include <QDebug>

#include <event.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void socketAccetp(int fd, short events, void *arg){
    evutil_socket_t sockfd;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    sockfd = ::accept(fd, (struct sockaddr*)&client, &len);
    evutil_make_socket_nonblocking(sockfd);

//    getpeername(fd, (struct sockaddr*)&client, &len);
    char *ip;
    ip = inet_ntoa(client.sin_addr);
    qDebug() << "accept a client from " <<fd <<" ip["<<ip<<"]";
    struct event_base *base = (event_base*)arg;
    // 为了动态创建一个event结构体
    struct event *ev = event_new(nullptr, -1, 0, nullptr, nullptr);
    // 将动态创建的结构体作为event的回调参数
    event_assign(ev, base, sockfd, EV_READ| EV_PERSIST, socketRead, (void*)ev);
    event_add(ev, nullptr);
}

void socketRead(int fd, short events, void *arg){
    char msg[4096] = {0};
    struct event *ev = (struct event*)arg;
    int flag = 0;

#if 0
    while(true){
        int len = ::read(fd, msg, sizeof(msg) -1);
        if(len > 0){
            qDebug() << "socketRrad[" << msg << "}\n";
        }else{
            qDebug() << "len = "<< len <<"\n";
            event_free(ev);
            close(fd);
            return ;
        }
    }
#else
    int len = ::read(fd, msg, sizeof(msg) -1);

    if(len <= 0){
        qDebug() << "socketRead error , len:" << len;
        event_free(ev);
        close(fd);
        return ;
    }else{
        qDebug()<<"read fd:"<< fd << " len:["<<len << "] msg["<<strlen(msg)<<"]["<<msg<<"]";
        char buffer[64] = {0};
        sprintf(buffer, "%s\r\n", "ICY 200 OK");
        ::write(fd, buffer, strlen(buffer));
        qDebug() << "send data: "<<buffer;
    }
#endif
}

int socketServerInit(int port, int listen_num){
    int errno_save;
    evutil_socket_t listener;

    listener = ::socket(AF_INET, SOCK_STREAM, 0);
    if(listener == -1){
        return -1;
    }

    // 允许多次绑定同一个地址， 要用在socket 和bind之间
    evutil_make_listen_socket_reuseable(listener);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = 0;
    server.sin_port = htons(port);

    if(::bind(listener, (struct sockaddr *)&server, sizeof(server)) < 0){
        qDebug() << "socket bind error";
        goto error;
    }
    if(::listen(listener, listen_num) < 0){
        qDebug() << "socket listen error";
        goto error;
    }

    // 将套接字设置为非阻塞状态
    evutil_make_socket_nonblocking(listener);
    return listener;

    error:
        errno_save = errno;
        evutil_closesocket(listener);
        errno = errno_save;
        return -1;
}
