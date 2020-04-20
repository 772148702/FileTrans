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

        unsigned short Port() const {
            return (unsigned short)ntohs(m_addr.sin_port);
        }

        unsigned int IpInt() const {
            return ntohl(m_addr.sin_addr.s_addr);
        }

        bool IsIpValid() const {
            return m_addr.sin_addr.s_addr != INADDR_NONE;
        }

        struct sockaddr_in &GetAddr() {
            return m_addr;
        }

        static std::string HostToIp(const std::string &host) {
            Ip4Addr addr(host, 0);
            return addr.Ip();
        }

    private:
        struct sockaddr_in m_addr;
    };

    typedef  std::function<void()> Task;
    struct  Channel {
        Channel(int fd) {
            m_fd = fd;
            m_event = EPOLLIN|EPOLLOUT|EPOLLET;
        }

        void SetReadCb(Task);
        void SetWriteCb(Task);

        void HandleRead();
        void HandleWrite();

        int m_fd,m_event;
        std::function<void()> m_readCb,m_writeCb;
    };


    struct TcpConnection {
        TcpConnection();
        void Attach(int fd,Ip4Addr local, Ip4Addr peer);
        void HandleConnection();
        void HandleWrite();
        void HandleRead();
        Buffer m_inbuffer, m_outbuffer;
        Ip4Addr m_local,m_peer;
        std::shared_ptr<Channel> m_pChannel;
    };

}
#endif //HANDY_CONN_H
