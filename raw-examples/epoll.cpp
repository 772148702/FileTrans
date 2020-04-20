//
// Created by Shinelon on 2020/4/16.
//



#include <sys/socket.h>
#include <handy/logging.h>
#include <sys/poll.h>
#include "handy/net.h"
#include "sys/epoll.h"
#include "handy/conn.h"
#include "errno.h"
using namespace handy;
const int MaxEvents = 2000;
int CreateFd() {
    int fd = socket(AF_INET,SOCK_STREAM,0);
    net::setNonBlock(fd,true);
    net::setReusePort(fd,true);
    net::setReuseAddr(fd,true);
    sockaddr_in addr;
    addr.sin_family =AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8081);
    bind(fd,(struct sockaddr* )&addr,sizeof(struct sockaddr));
    listen(fd,5);
    info("begin to linsten %s %s",std::to_string(addr.sin_addr.s_addr).c_str(), std::to_string(addr.sin_port).c_str());
    return fd;
}


void AddEpoll(int efd,int fd, epoll_event& ev,Channel* channel) {
    ev.events = EPOLLIN |EPOLLOUT|EPOLLET ;
    ev.data.ptr = channel;
    epoll_ctl(efd,EPOLL_CTL_ADD,fd,&ev);
}

void UpdteEpoll(int efd,int fd,epoll_event& ev,int op) {
    ev.events |= op ;
    ev.data.fd = fd;
    epoll_ctl(efd,EPOLL_CTL_MOD,fd,&ev);
}

void DeleteEpoll(int efd,int fd,epoll_event& ev) {
    ev.events = POLLIN | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(efd,EPOLL_CTL_DEL,fd,&ev);
}

void HandleAccept(int listenfd,int efd,Channel* ch) {

    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);

    int newFd = accept(listenfd, (struct sockaddr *) &raddr, &rsz);
    net::setNonBlock(newFd,true);
    epoll_event ev;

    AddEpoll(efd, newFd, ev,ch);
    info("new connection address:%s:%s", std::to_string(raddr.sin_addr.s_addr).c_str(),
         std::to_string(raddr.sin_port).c_str());

}


struct EventLoop {

    void loop (int efd,epoll_event* evs,int maxEvents,int listenfd){
        int n;
        struct sockaddr_in raddr;
        socklen_t rsz = sizeof(raddr);
        while(1) {
            info("loop");
          n = epoll_wait(efd,evs,maxEvents,-1);
          info("%d",n);
          for(int i=0;i<n;i++) {
              epoll_event curEv = evs[i];
              Channel* ch = (Channel*)curEv.data.ptr;
              if(ch->m_fd==listenfd) {
                    if(curEv.events & EPOLLIN) {
                        int newFd = accept(listenfd, (struct sockaddr *) &raddr, &rsz);
                        net::setNonBlock(newFd,true);
                        epoll_event ev;
                        TcpConnection conn;
                        conn.Attach(newFd,raddr,raddr);
                        AddEpoll(efd, newFd, ev,conn.m_pChannel.get());
                        info("new connection address:%s", conn.m_peer.ToString().c_str());

                    }
              } else {
                  if(curEv.events&&EPOLLIN) {


                  }


              }
          }
        }
    }
};

int main() {
  int fd = CreateFd();
  Channel ch(fd);
  int efd = epoll_create1(EPOLL_CLOEXEC);
  epoll_event ev;
  AddEpoll(efd,fd,ev,&ch);
  epoll_event events[MaxEvents];
  EventLoop eLoop;
  eLoop.loop(efd,events,MaxEvents,fd);
}