//
// Created by Shinelon on 2020/4/17.
//

#ifndef HANDY_CONN_H
#define HANDY_CONN_H

#include <cstring>
#include "sys/socket.h"
#include "netinet/in.h"
#include "string"
#include "port_posix.h"
#include "logging.h"
#include "buffer.h"
#include "memory"
#include "sys/epoll.h"
#include "eventBase.h"
#include "poller.h"
namespace  handy {
    struct Ip4Addr {

        Ip4Addr(const std::string &host, unsigned short port) {
            memset(&m_addr, 0, sizeof(m_addr));
            m_addr.sin_family = AF_INET;
            m_addr.sin_port = htons(port);
            if (host.size()) {
                m_addr.sin_addr = port::getHostByName(host);
            } else {
                m_addr.sin_addr.s_addr=INADDR_ANY;
            }
            if(m_addr.sin_addr.s_addr==INADDR_NONE) {
                error("cannot resove %s to ip",host.c_str());
            }
        }
        Ip4Addr(unsigned short port = 0) : Ip4Addr("", port) {}
        Ip4Addr(const struct sockaddr_in &addr) : m_addr(addr) {}
        std::string ToString() const {
            uint32_t uip = m_addr.sin_addr.s_addr;
            return util::format("%d.%d.%d.%d:%d", (uip >> 0) & 0xff, (uip >> 8) & 0xff, (uip >> 16) & 0xff, (uip >> 24) & 0xff, ntohs(m_addr.sin_port));
        }
        std::string Ip() const {
            uint32_t uip = m_addr.sin_addr.s_addr;
            return util::format("%d.%d.%d.%d", (uip >> 0) & 0xff, (uip >> 8) & 0xff, (uip >> 16) & 0xff, (uip >> 24) & 0xff);
        }
        unsigned short Port() const {return (unsigned short)ntohs(m_addr.sin_port);}
        unsigned int IpInt() const {return ntohl(m_addr.sin_addr.s_addr);}
        bool IsIpValid() const {return m_addr.sin_addr.s_addr != INADDR_NONE;}
        struct sockaddr_in &GetAddr() {return m_addr;}
        static std::string HostToIp(const std::string &host) {Ip4Addr addr(host, 0);return addr.Ip();}
    private:
        struct sockaddr_in m_addr;
    };

    typedef  std::function<void()> Task;
    struct EventBase;
    struct PollerBase;
    struct TcpConnection;
    typedef std::shared_ptr<handy::TcpConnection> TcpConnPtr;
    typedef std::function<void(const TcpConnPtr&)> TcpCallBack;
    struct  Channel {
        Channel(EventBase * base, int fd, int events);
        int Fd();
        void Close();
        void SetReadCb(Task);
        void SetWriteCb(Task);
        void HandleRead();
        void HandleWrite();
        void EnableWrite(bool);
        bool ReadEnabled();
        bool WriteEnable();
    public:
        int m_fd,m_event;
        long m_id;
        PollerBase* m_poller;
        std::function<void()> m_readCb,m_writeCb;
    };


    struct TcpConnection:public std::enable_shared_from_this<TcpConnection>, private noncopyable {
        void Attach(EventBase *base,int fd,Ip4Addr local, Ip4Addr peer);
        void HandleConnection(const TcpConnPtr &conn);
        void Connect(EventBase *base, const std::string &host, unsigned short port, int timeout, const std::string &localip);
        template <class C = TcpConnection>
        static TcpConnPtr CreateConnection(EventBase *base, const std::string &host, unsigned short port, int timeout = 0, const std::string &localip = ""){
            TcpConnPtr conn(new C);
            conn->Connect(base,host,port,timeout,localip);
            return conn;
        }
        void HandleWrite(const TcpConnPtr &conn);
        void HandleRead(const TcpConnPtr &conn);
        void Send(Buffer &msg);
        void Send(const char* buf, size_t len);
        void SendOutput() { Send(m_outbuffer); }
        ssize_t Isend(const char *buf, size_t len);
        void SetReadCb(TcpCallBack cb) {m_readCB = cb;}
        void SetWriteCb(TcpCallBack cb) {m_writeCb = cb;}
        void SetConnCb(TcpCallBack cb) {m_connCb = cb;}
        void SetDisconnectCb(TcpCallBack cb) {m_disconnectCb =cb;}
        void CleanUp(const TcpConnPtr& con);
        Buffer m_inbuffer, m_outbuffer;
        Ip4Addr m_local,m_peer;
        Channel* m_pChannel;
        TcpCallBack  m_readCB,m_writeCb,m_connCb,m_disconnectCb;
    };

}
#endif //HANDY_CONN_H
