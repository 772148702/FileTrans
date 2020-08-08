//
// Created by Shinelon on 2020/4/17.
//

#include "conn.h"
#include "net.h"
#include "fcntl.h"
#include <atomic>
namespace  handy {

    void handy::TcpConnection::HandleConnection(const TcpConnPtr &conn) {

    }

    void handy::TcpConnection::HandleWrite(const TcpConnPtr &conn) {
        int rc = 1;
        int fd = m_pChannel->m_fd;

        ssize_t sended = Isend(m_outbuffer.Begin(),m_outbuffer.Size());
        m_outbuffer.Consume(sended);
        if(m_outbuffer.Empty()&&m_writeCb) {
            m_writeCb(conn);
        }
//        if(m_outbuffer.Empty()&&m_writeCb) {
//            m_pChannel->EnableWrite(false);
//        }

    }

    void handy::TcpConnection::HandleRead(const TcpConnPtr &conn) {
        int rc = 1;
        int fd = m_pChannel->m_fd;
        while (true) {
            m_inbuffer.MakeRoom();
            rc = 0;
            if(m_pChannel->Fd()>=0) {
                rc = read(fd, m_inbuffer.End(), m_inbuffer.Space());
                trace("channel %lld fd %d readed %d bytes", (long long) m_pChannel->m_id, m_pChannel->Fd(), rc);
            }
            if(rc==-1&&(errno == EINTR)) {
                continue;
            } else if(rc==-1 &&(errno == EAGAIN || errno == EWOULDBLOCK)) {
                if(m_readCB)
                     m_readCB(conn);
                return;
            } else if(m_pChannel->Fd() == -1 || rc == 0 || rc == -1) {
                CleanUp(conn);
                break;
            }  else  {
                m_inbuffer.AddSize(rc);
            }
        }
    }



    void handy::TcpConnection::Attach(EventBase *base, int fd, handy::Ip4Addr local, handy::Ip4Addr peer) {
      //  m_pChannel = new Channel(fd);
        m_local = local;
        m_peer = peer;

        m_pChannel = new Channel(base,fd,kReadEvent|kWriteEvent);
        TcpConnPtr con = shared_from_this();
        con->m_pChannel->SetReadCb([=] {
            con->HandleRead(con);
        });
        con->m_pChannel->SetWriteCb([=]{
            con->HandleWrite(con);
        });
    }

    void TcpConnection::Send(Buffer &buf) {
            if(m_pChannel) {
                if(m_pChannel->WriteEnable()) {
                    m_outbuffer.Absorb(buf);
                }
                //send data
                if(buf.Size()) {
                    ssize_t sended = Isend(buf.Begin(), buf.Size());
                    buf.Consume(sended);
                }
                //save rest data
                if (buf.Size()) {
                    m_outbuffer.Absorb(buf);
                    if (!m_pChannel->WriteEnable()) {
                        m_pChannel->EnableWrite(true);
                    }
                }
            } else {
                warn("connection %s - %s closed, but still writing %lu bytes", m_local.ToString().c_str(), m_peer.ToString().c_str(), buf.Size());
            }

    }

    ssize_t TcpConnection::Isend(const char *buf, size_t len) {
        size_t sended = 0;
        while(len>sended) {
            ssize_t wd = ::write(m_pChannel->Fd(),buf+sended,len - sended);
            trace("channel %lld fd %d write %ld bytes", (long long) m_pChannel->m_id, m_pChannel->Fd(), wd);
            if(wd>0) {
                sended+=wd;
                continue;
            } else if (wd==-1&&errno==EINTR) {
                continue;
            } else if (wd==-1 &&(errno==EAGAIN||errno==EWOULDBLOCK)) {
                if(!m_pChannel->WriteEnable()) {
                    m_pChannel->EnableWrite(true);
                }
                break;
            } else {
                error("write error: channel %lld fd %d wd %ld %d %s", (long long) m_pChannel->m_id, m_pChannel->Fd(), wd, errno, strerror(errno));
                break;
            }
        }
        return sended;
    }

    void TcpConnection::CleanUp(const TcpConnPtr &con) {
            if(m_readCB&&m_inbuffer.Size()) {
                m_readCB(con);
            }

            if(m_disconnectCb) {
                m_disconnectCb(con);
            }
            m_readCB = m_writeCb=m_connCb=m_disconnectCb= nullptr;
            Channel * ch = m_pChannel;
            m_pChannel = nullptr;
            delete ch;
    }

    void TcpConnection::Send(const char *buf, size_t len) {
        if(m_pChannel) {
            if(m_outbuffer.Empty()) {
                ssize_t  sended = Isend(buf,len);
                buf+=sended;
                len-=sended;
            }
            if(len) {
                m_outbuffer.Append(buf,len);
            }
        } else {
            warn("connection %s - %s closed, but still writing %lu bytes", m_local.ToString().c_str(), m_peer.ToString().c_str(), len);
        }
    }



    void TcpConnection::Connect(EventBase *base, const std::string &host, unsigned short port, int timeout,
                                const std::string &localip) {
            Ip4Addr addr(host,port);
            int fd = socket(AF_INET,SOCK_STREAM,0);
            fatalif(fd < 0, "socket failed %d %s", errno, strerror(errno));
            net::setNonBlock(fd);
            int t = util::addFdFlag(fd, FD_CLOEXEC);
            fatalif(t, "addFdFlag FD_CLOEXEC failed %d %s", t, strerror(t));
            int r = 0;
            if (localip.size()) {
                Ip4Addr addr(localip, 0);
                r = ::bind(fd, (struct sockaddr *) &addr.GetAddr(), sizeof(struct sockaddr));
                error("bind to %s failed error %d %s", addr.ToString().c_str(), errno, strerror(errno));
            }
            if (r == 0) {
                r = ::connect(fd, (sockaddr *) &addr.GetAddr(), sizeof(sockaddr_in));
                if (r != 0 && errno != EINPROGRESS) {
                    error("connect to %s error %d %s", addr.ToString().c_str(), errno, strerror(errno));
                }
            }
            sockaddr_in local;
            socklen_t alen = sizeof(local);
            if (r == 0) {
                r = getsockname(fd, (sockaddr *) &local, &alen);
                if (r < 0) {
                    error("getsockname failed %d %s", errno, strerror(errno));
                }
            }

           Attach(base, fd, Ip4Addr(local), addr);
    }




    void handy::Channel::HandleRead() {
        m_readCb();
    }

    void handy::Channel::HandleWrite() {
        m_writeCb();
    }

    void handy::Channel::SetReadCb(handy::Task t) {
        m_readCb = t;
    }

    void handy::Channel::SetWriteCb(handy::Task t) {
        m_writeCb = t;
    }

    void Channel::Close() {
        if(m_fd>=0) {
            trace("close channel %ld fd %d",(long)m_id,m_fd);
            m_poller->RemoveChannel(this);
            ::close(m_fd);
            m_fd = -1;
            HandleRead();
        }
    }

    Channel::Channel(EventBase * base, int fd, int events) {
            fatalif(net::setNonBlock(m_fd)<0,"channel set no block failed");
            static std::atomic<int64_t> id(0);
            m_id =++id;
            m_fd = fd;
            net::setNonBlock(m_fd);
            m_event = events;
            m_poller = base->m_EventImp->m_poller;
            m_poller->AddChannel(this);
    }
    int Channel::Fd() {
       return m_fd;
    }
    bool Channel::ReadEnabled() {
        return m_event&kReadEvent;
    }
    bool Channel::WriteEnable() {
        return m_event&kWriteEvent;
    }

    void Channel::EnableWrite(bool enable) {
        if(enable) {
            m_event |=kWriteEvent;
        } else  {
            m_event &=~kWriteEvent;
        }
        m_poller->UpdateChannel(this);
    }
}