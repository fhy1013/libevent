#ifndef LIBEVENT_H
#define LIBEVENT_H

void socketAccetp(int fd, short events, void *arg);
void socketRead(int fd, short events, void *arg);
int socketServerInit(int port, int listen_num);

#endif // LIBEVENT_H
