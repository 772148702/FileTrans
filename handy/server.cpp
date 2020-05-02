//
// Created by Shinelon on 2020/4/22.
//


#include "server.h"
#include "net.h"
#include <fcntl.h>
#include "conn.h"
namespace handy {
    int handy::TcpServer::Bind(const std::string &host, unsigned short port, bool reusePort) {
        m_addr = Ip4Addr(host,port);
        int fd = socket(AF_INET,SOCK_STREAM,0);
        int r = net::setReuseAddr(fd);
        r = net::setReusePort(fd,reusePort);
        r = util::addFdFlag(fd,reusePort);
        r = ::bind(fd, (struct sockaddr *) &m_addr.GetAddr(), sizeof(struct sockaddr));
        if(r) {
            close(fd);
            error("bind to %s failed %d %s",m_addr.ToString().c_str(), errno, strerror(errno));
            return errno;
        }
        r = listen(fd,20);
        fatalif(r,"listen failed %d %s",errno,strerror(errno));
        info("fd %d listening at %s",fd,m_addr.ToString().c_str());
        m_listenChannel = new Channel(m_base, fd,kReadEvent);
        m_listenChannel->SetReadCb([this] {HandleAccept();});
    }

    void TcpServer::HandleAccept() {
        struct sockaddr_in raddr;
        socklen_t  rsz = sizeof(raddr);
        int lfd = m_listenChannel->Fd();
        int cfd;
        while(lfd>=0&&(cfd=accept(lfd,(struct sockaddr* )&raddr,&rsz))>= 0) {
            sockaddr_in peer,local;
            socklen_t  alen = sizeof(peer);
            int r = getpeername(cfd,(sockaddr *)&peer,&alen);
            if(r<0) {
                error("get peer name failed %d %s",errno,strerror(errno));
                continue;
            }
            r = getsockname(cfd,(sockaddr* )&local,&alen);
            if(r<0) {
                error("getsocakname failed %d %s",errno,strerror((errno)));
                continue;
            }
            r = util::addFdFlag(cfd,FD_CLOEXEC);
            fatalif(r,"addFdFlag FD_CLOEXEC failed");
            EventBase * b = m_base->AllocBase();
            auto addcon = [=] {
                TcpConnPtr conn = m_createCb();
                conn->Attach(b,cfd,local,peer);
                if(m_readCb) {
                    conn->SetReadCb(m_readCb);
                }
                if(m_writeCb) {
                    conn->SetWriteCb(m_writeCb);
                }
                if(m_disconnectionCb) {
                    conn->SetDisconnectCb(m_disconnectionCb);
                }
                if(m_connectionCb) {
                    conn->SetConnCb(m_connectionCb);
                }
                 m_connectionCb(conn);
            };
            if(b==m_base) {
                addcon();
            } else {
                b->SafeCall(addcon);
            }

        }
    }

    TcpServerPtr
    TcpServer::StartServer(EventBases *bases, const std::string &host, unsigned short port, bool reusePort) {
        handy::TcpServerPtr p(new TcpServer(bases));
        int r = p->Bind(host,port,reusePort);
        if(r) {
            error("bind to %s:%d failed %d %s", host.c_str(), port, errno, strerror(errno));
        }
        return r == 0 ? p : NULL;
    }

    void TcpServer::SetReadCb(const TcpCallBack &cb) {
                m_readCb = cb;
    }

    void TcpServer::SetWriteCb(const TcpCallBack &cb) {
                m_writeCb = cb;
    }

    void TcpServer::SetConnectionCb(const TcpCallBack &cb) {
                m_connectionCb = cb;
    }

    void TcpServer::SetDisconnectionCb(const TcpCallBack &cb) {
                m_disconnectionCb = cb;
    }
}

