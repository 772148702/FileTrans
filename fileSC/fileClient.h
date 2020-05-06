//
// Created by Shinelon on 2020/5/3.
//

#ifndef HANDY_FILECLIENT_H
#define HANDY_FILECLIENT_H
#include "conn.h"
#include "common.h"
#include "memory"
using namespace handy;
struct FileClient{
    FileClient(EventBase *base, const std::string &host, unsigned short port,bool isGetInput=true);
    void GetInputStart();
    void SendFile(char* path);
    std::shared_ptr<TcpConnection> m_pConn;
    Task  task;
    EventBase * m_pBase;
};


#endif //HANDY_FILECLIENT_H
