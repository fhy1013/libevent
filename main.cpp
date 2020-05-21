#include "mylibevent.h"
#include <QCoreApplication>
#include <QDebug>
#include <QThread>

#include <event.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int port = 2101;
    int listen_num = 3;

    qDebug() << "socketServerInit begin ... ";
    int listener = socketServerInit(port, listen_num);
    if(listener == -1){
        qDebug() << "socketServerInit error";
        return a.exec();
    }
    qDebug() << "socketServerInit success";

    struct event_base *base = event_base_new();
    // 添加监听客户端请求连接事件
    struct event * ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, socketAccetp, base);

    event_add(ev_listen, nullptr);
    event_base_dispatch(base);

    //return a.exec();
    return 0;
}
