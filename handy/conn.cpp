//
// Created by Shinelon on 2020/4/17.
//

#include "conn.h"

void handy::TcpConnection::HandleConnection() {

}

void handy::TcpConnection::HandleWrite() {
    int rc=1;
    int fd = m_pChannel->m_fd;
    while(true) {
        if(m_outbuffer.Space()<=0) {
            return;
        }
        rc = write(fd,m_outbuffer.Begin(),m_outbuffer.Space());
        if(rc<0 &&(errno==EAGAIN||errno==EWOULDBLOCK)) {
            return ;
        } else if(rc<0&&(errno==EINTR)) {
            continue;
        } else {
            m_outbuffer.Consume(rc);
        }
    }
}

void handy::TcpConnection::HandleRead() {
       int rc=1;
       int fd = m_pChannel->m_fd;
       while(true) {
           m_inbuffer.MakeRoom();
           rc = read(fd,m_inbuffer.Begin(),m_inbuffer.Space());
           if(rc<0 &&(errno==EAGAIN||errno==EWOULDBLOCK)) {
               return ;
           } else if(rc<0&&(errno==EINTR)) {
               continue;
           } else {
               m_inbuffer.AddSize(rc);
           }
       }
}

void handy::TcpConnection::Attach(int fd, handy::Ip4Addr local, handy::Ip4Addr peer) {
    m_pChannel = std::make_shared<Channel>(fd);
    m_local = local;
    m_peer = peer;
}

handy::TcpConnection::TcpConnection() {


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
