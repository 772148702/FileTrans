//
// Created by Shinelon on 2020/4/22.
//

#ifndef HANDY_SERVER_H
#define HANDY_SERVER_H

#include "conn.h"

namespace  handy {
    struct TcpServer;
    struct EventBases;
    typedef std::shared_ptr<TcpServer> TcpServerPtr;
    struct TcpServer: private noncopyable {
        TcpServer(EventBases * bases):m_createCb([]{return  TcpConnPtr (new TcpConnection);}),
        m_base(bases->AllocBase()),m_bases(bases),m_listenChannel(nullptr){}
        int Bind(const std::string& host, unsigned short port, bool reusePort = true);
        static TcpServerPtr StartServer(EventBases *bases, const std::string &host, unsigned short port, bool reusePort=true);
        void HandleAccept();
        void SetReadCb(const TcpCallBack &cb);
        void SetWriteCb(const TcpCallBack &cb);
        void SetConnectionCb(const TcpCallBack &cb);
        void SetDisconnectionCb(const TcpCallBack &cb);
        EventBases* m_bases;
        EventBase*  m_base;
        TcpCallBack m_readCb,m_writeCb,m_connectionCb,m_disconnectionCb;
        Channel* m_listenChannel;
        Ip4Addr m_addr;
        std::function<TcpConnPtr()> m_createCb;
    };
}

#endif //HANDY_SERVER_H
